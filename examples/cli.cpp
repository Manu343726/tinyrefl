#include <optional>
#include <regex>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include <fmt/format.h>

#include <tinyrefl/api.hpp>
#include <tinyrefl/matchers.hpp>
#include <tinyrefl/object_visitor.hpp>
#include <tinyrefl/types/array_view.hpp>
#include <tinyrefl/types/string.hpp>

#include "cli.hpp"
#include "cli.hpp.tinyrefl"
#include <tinyrefl/entities.hpp>

namespace matchers
{
constexpr auto isOption = tinyrefl::matchers::hasAttribute(
    tinyrefl::matchers::full_named("cli::option"));

constexpr auto hasCustomParse = tinyrefl::matchers::anyOf(
    isOption,
    tinyrefl::matchers::hasAttribute(
        tinyrefl::matchers::full_named("cli::parse")));

constexpr auto isCliEntryPoint = tinyrefl::matchers::hasAttribute(
    tinyrefl::matchers::full_named("cli::entrypoint"));
} // namespace matchers

namespace concepts
{
template<typename T>
constexpr bool hasCustomParse = tinyrefl::matches<T>(matchers::hasCustomParse);
}

namespace utils
{

std::size_t replace_all(
    std::string&       input,
    const std::string& substring,
    const std::string& replace)
{
    std::size_t            result = 0;
    std::string::size_type pos    = 0;

    while((pos = input.find(substring, pos)) != std::string::npos)
    {
        input.replace(pos, substring.size(), replace);
        pos += replace.size();
        result++;
    }

    return result;
}

template<typename Entity>
constexpr std::optional<tinyrefl::string> parseExpression(const Entity& entity)
{
    if(entity.has_attribute("cli::parse"))
    {
        return entity.attribute("cli::parse").arguments()[0].pad(1, 1);
    }
    else if(entity.has_attribute("cli::option"))
    {
        return entity.attribute("cli::option").arguments()[0].pad(1, 1);
    }
    else
    {
        return std::nullopt;
    }
}

void sanitize_for_regex(std::string& expression, const std::string& symbol)
{
    replace_all(expression, symbol, "\\" + symbol);
}

void sanitize_for_regex(std::string& expression)
{
    sanitize_for_regex(expression, "-");
    sanitize_for_regex(expression, "+");
    sanitize_for_regex(expression, "[");
    sanitize_for_regex(expression, "]");
    sanitize_for_regex(expression, "(");
    sanitize_for_regex(expression, ")");
    sanitize_for_regex(expression, "{");
    sanitize_for_regex(expression, "}");
}

template<typename T>
std::optional<T> parse_value(const std::string& value)
{
    if constexpr(std::is_enum_v<T>)
    {
        if(const auto rawValue = parse_value<std::underlying_type_t<T>>(value);
           rawValue and tinyrefl::metadata<T>().is_enum_value(*rawValue))
        {
            return tinyrefl::enum_cast<T>(*rawValue);
        }
        else
        {
            if(tinyrefl::metadata<T>().is_enum_value(value))
            {
                return tinyrefl::enum_cast<T>(value);
            }
            else
            {
                return std::nullopt;
            }
        }
    }
    else if constexpr(std::is_integral_v<T>)
    {
        try
        {
            return static_cast<T>(std::stoi(value));
        }
        catch(const std::logic_error&)
        {
            return std::nullopt;
        }
    }
    else if constexpr(std::is_class_v<T> and tinyrefl::has_metadata<T>())
    {
        return tinyrefl::from_json<T>(nlohmann::json::parse(value));
    }
    else
    {
        return std::nullopt;
    }
}

template<>
std::optional<std::string> parse_value(const std::string& value)
{
    return value;
}

} // namespace utils

template<typename T>
struct ParseResult
{
    operator bool() const
    {
        return _value.has_value();
    }

    T value() const
    {
        return _value.value_or(T{});
    }

    const std::string& details() const
    {
        return _details;
    }

    static ParseResult success(T value)
    {
        return {std::move(value)};
    }

    static ParseResult error(std::string details)
    {
        return {std::move(details), details_tag{}};
    }

    template<typename String, typename... Args>
    static ParseResult error(String&& message, Args&&... args)
    {
        return error(fmt::format(
            std::forward<String>(message), std::forward<Args>(args)...));
    }

private:
    std::optional<T> _value;
    std::string      _details;

    ParseResult(T value) : _value{std::move(value)} {}

    struct details_tag
    {
    };

    ParseResult(std::string details, details_tag) : _details{std::move(details)}
    {
    }
};

template<typename T>
ParseResult<T> parse(const std::string& value);

template<typename Class>
struct ClassParser
{
    ClassParser()
    {
        parseExpression();
    }

    ParseResult<Class> parse(const std::string& input) const
    {
        std::optional<ParseResult<Class>> parseError;
        Class                             parsedObject;
        std::smatch                       match;

        if(not std::regex_match(input, match, _regex))
        {
            return ParseResult<Class>::error(
                "Could not parse argument \"{}\" (type: {}) from expression \"{}\"",
                cli_parse_expression(),
                class_().name(),
                input);
        }

        tinyrefl::visit_class(
            class_(),
            tinyrefl::member_variable_visitor([&](const auto& member) {
                if(parseError)
                {
                    return;
                }

                if(const auto it = _memberToRegexGroup.find(member.name());
                   it != _memberToRegexGroup.end())
                {
                    using value_type = decltype(tinyrefl::reify(member.type()));
                    const auto        regexGroup  = it->second;
                    const std::string valueString = match[regexGroup].str();

                    if(const auto value =
                           utils::parse_value<value_type>(valueString))
                    {
                        member.get(parsedObject) = *value;
                    }
                    else
                    {
                        parseError = ParseResult<Class>::error(
                            "Could not convert input value \"{}\" to a value of type {}",
                            valueString,
                            member.type().full_name());
                    }
                }
            }));

        if(parseError)
        {
            return *parseError;
        }
        else
        {
            return ParseResult<Class>::success(std::move(parsedObject));
        }
    }

private:
    std::string                                             _pattern;
    std::regex                                              _regex;
    std::unordered_map<std::string, std::smatch::size_type> _memberToRegexGroup;

    void parseExpression()
    {
        std::string expression = cli_parse_expression();

        utils::sanitize_for_regex(expression);

        std::size_t totalGroups = 0;

        tinyrefl::visit_class(
            class_(),
            tinyrefl::member_variable_visitor([&](const auto& member) {
                const std::string memberName      = member.name();
                const std::string memberParameter = "\\{" + memberName + "\\}";
                const std::size_t generatedGroups =
                    ::utils::replace_all(expression, memberParameter, "(.+)");

                for(std::size_t consumedGroup = 0;
                    consumedGroup < generatedGroups;
                    ++consumedGroup)
                {
                    _memberToRegexGroup[memberName] =
                        consumedGroup + totalGroups +
                        1; // + 1 to ignore the first group, which is the full
                           // match
                }

                totalGroups += generatedGroups;
            }));

        _pattern = expression;
        _regex   = std::regex{expression};
    }

    static constexpr auto class_()
    {
        return tinyrefl::metadata<Class>();
    }

    static constexpr tinyrefl::string cli_parse_expression()
    {
        static_assert(concepts::hasCustomParse<Class>);
        return *utils::parseExpression(class_());
    }
};

template<typename Variant>
struct VariantParser;

template<typename... Ts>
struct VariantParser<std::variant<Ts...>>
{
    using variant = std::variant<Ts...>;

    ParseResult<variant> parse(const std::string& value) const
    {
        auto result = ParseResult<variant>::error(
            "No matching type found for expression \"{}\" in {}",
            tinyrefl::entities::type<variant>().full_name());

        tinyrefl::meta::foreach(
            std::make_tuple(tinyrefl::type_constant<Ts>()...),
            [&](const auto& type) {
                using T = typename decltype(type)::type;

                if(result)
                {
                    return;
                }

                if(const auto parseResult = ::parse<T>(value))
                {
                    result = parseResult;
                }
            });

        return result;
    }
};

template<typename T>
ParseResult<T> parse(const std::string& value)
{
    if constexpr(
        std::is_class_v<T> and tinyrefl::has_metadata<T>() and
        concepts::hasCustomParse<T>)
    {
        static const ClassParser<T> parser;
        return parser.parse(value);
    }
    else
    {
        if(const auto parsedValue = utils::parse_value<T>(value))
        {
            return ParseResult<T>::success(*parsedValue);
        }
        else
        {
            return ParseResult<T>::error(
                "Could not convert \"{}\" to a value of type {}",
                tinyrefl::entities::type<T>().full_name());
        }
    }
}

template<typename T>
std::string show_function_parameter_help(
    const tinyrefl::entities::function_parameter<T>& parameter)
{
    if(const auto parseExpression =
           utils::parseExpression(parameter.type().decayed()))
    {
        return fmt::format(
            "{} <{}> (type: {})",
            parameter.name(),
            parseExpression.value(),
            parameter.type().decayed().name());
    }
    else
    {
        return fmt::format(
            "{} (type: {})",
            parameter.name(),
            parameter.type().decayed().name());
    }
}

template<typename... Ts>
std::string show_function_parameters_help(
    const std::tuple<tinyrefl::entities::function_parameter<Ts>...>& parameters)
{
    std::array<std::string, sizeof...(Ts)> help;

    tinyrefl::meta::foreach(parameters, [&](const auto& parameter) {
        help[parameter.index()] = show_function_parameter_help(parameter);
    });

    return fmt::format("{}", fmt::join(help, " "));
}

template<typename Cli>
void show_help(
    const Cli& cli, const tinyrefl::string& name = "", std::size_t depth = 0)
{
    const std::string prefix(depth * 4, ' ');
    constexpr auto    class_ = tinyrefl::metadata<Cli>();

    fmt::print("{}{}:\n", prefix, (name.size() > 1 ? name : class_.name()));

    fmt::print("{} - subcommands:\n", prefix);
    tinyrefl::visit_member_variables(
        cli, [depth](const tinyrefl::string& name, const auto& member) {
            show_help(member, name, depth + 1);
        });

    fmt::print("{} - commands:\n", prefix);
    tinyrefl::visit_class(
        class_,
        tinyrefl::member_function_visitor([&prefix](const auto& function) {
            fmt::print(
                "{}    {} {}\n",
                prefix,
                function.name(),
                show_function_parameters_help(
                    function.signature().parameters()));
        }));
}

std::vector<tinyrefl::string> collect_args(int argc, char** argv)
{
    std::vector<tinyrefl::string> result;
    result.reserve(argc);

    std::copy(argv, argv + argc, std::back_inserter(result));

    return result;
}

template<typename... Ts>
ParseResult<std::tuple<Ts...>>
    join(const std::tuple<ParseResult<Ts>...>& results)
{
    bool        success = true;
    std::string details;
    const auto  values =
        tinyrefl::meta::tuple_map(results, [&](const auto& result) {
            success &= result;

            if(not result)
            {
                details += fmt::format("{}\n", result.details());
            }

            return result.value();
        });

    if(success)
    {
        return ParseResult<std::tuple<Ts...>>::success(std::move(values));
    }
    else
    {
        return ParseResult<std::tuple<Ts...>>::error(details);
    }
}

template<typename Function>
auto parse_function_args(
    const Function&                               function,
    const tinyrefl::array_view<tinyrefl::string>& args)
{
    return join(tinyrefl::meta::tuple_map(
        function.signature().parameters(), [&](const auto& parameter) {
            using T = decltype(tinyrefl::reify(parameter.type().decayed()));
            const auto arg = args[parameter.index()];
            return parse<T>(arg);
        }));
}

template<typename Cli, typename Function>
int run_function(
    Cli&                                          cli,
    const Function&                               function,
    const tinyrefl::array_view<tinyrefl::string>& args)
{
    if(const auto parsedArgs = parse_function_args(function, args))
    {
        try
        {
            std::apply(
                [&](auto&&... args) {
                    function(cli, std::forward<decltype(args)>(args)...);
                },
                parsedArgs.value());

            return 0;
        }
        catch(const std::exception& ex)
        {
            fmt::print(stderr, "Error: {}", ex.what());
            return 1;
        }
    }
    else
    {
        fmt::print(stderr, "Error parsing arguments: {}", parsedArgs.details());
        return 2;
    }
}

template<typename Cli>
int run(
    Cli&                                          cli,
    const tinyrefl::string&                       name,
    const tinyrefl::array_view<tinyrefl::string>& args)
{
    if(args.empty())
    {
        show_help(cli, name);
        return -1;
    }

    std::optional<int> result;

    tinyrefl::visit_member_variables(
        cli, [&](const tinyrefl::string name, auto& member) {
            if(result)
            {
                return;
            }

            if(args.front() == name)
            {
                result = run(member, name, args.mid(1));
            }
        });

    if(result)
    {
        return result.value();
    }

    tinyrefl::visit_class<Cli>(
        tinyrefl::member_function_visitor([&](const auto& function) {
            if(result)
            {
                return;
            }

            if(args.front() == function.name())
            {
                result = run_function(cli, function, args.mid(1));
            }
        }));

    if(not result)
    {
        fmt::print(stderr, "Command \"{}\" not found\n\n", args.front());
        show_help(cli, name);
        return 1;
    }

    return *result;
}

constexpr auto entrypoints = TINYREFL_MATCHES(matchers::isCliEntryPoint);

static_assert(
    tinyrefl::meta::tuple_size(entrypoints) != 0,
    "No cli entrypoints found, a class with [[cli::entrypoint]] should be declared");
static_assert(
    tinyrefl::meta::tuple_size(entrypoints) > 0 and
        tinyrefl::meta::tuple_size(entrypoints) < 2,
    "More than one cli entrypoints found");

int main(int argc, char** argv)
{
    using Entrypoint = decltype(tinyrefl::reify(std::get<0>(entrypoints)));
    Entrypoint entrypoint;

    const auto args = collect_args(argc, argv);

    return run(
        entrypoint, "entrypoint", tinyrefl::make_array_view(args).mid(1));
}
