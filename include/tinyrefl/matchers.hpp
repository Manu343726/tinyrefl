#ifndef TINYREFL_MATCHERS_MATCHER_HPP_INCLUDED
#define TINYREFL_MATCHERS_MATCHER_HPP_INCLUDED

#include <tinyrefl/entities/entity.hpp>
#include <type_traits>

namespace tinyrefl
{

namespace matchers
{

template<typename T, typename Function>
struct matches_value_t
{
    constexpr matches_value_t(T value, Function function)
        : _value{std::move(value)}, _function{std::move(function)}
    {
    }

    template<typename Entity>
    constexpr bool operator()(const Entity& entity) const
    {
        return _value == _function(entity);
    }

private:
    T        _value;
    Function _function;
};

template<typename T, typename Function>
constexpr matches_value_t<std::decay_t<T>, std::decay_t<Function>>
    matches_value(T&& value, Function&& function)
{
    return {std::forward<T>(value), std::forward<Function>(function)};
}

#define TINYREFL_MATCHERS_GET_MEMBER(member)                                   \
    struct get_##member                                                        \
    {                                                                          \
        template<typename Entity>                                              \
        constexpr decltype(auto) operator()(const Entity& entity) const        \
        {                                                                      \
            return entity.member();                                            \
        }                                                                      \
    };                                                                         \
                                                                               \
    template<typename T>                                                       \
    constexpr auto matches_##member(T&& value)                                 \
    {                                                                          \
        return matches_value(std::forward<T>(value), get_##member{});          \
    }

TINYREFL_MATCHERS_GET_MEMBER(name);
TINYREFL_MATCHERS_GET_MEMBER(full_name);
TINYREFL_MATCHERS_GET_MEMBER(display_name);
TINYREFL_MATCHERS_GET_MEMBER(full_display_name);
TINYREFL_MATCHERS_GET_MEMBER(kind);
TINYREFL_MATCHERS_GET_MEMBER(parent);
TINYREFL_MATCHERS_GET_MEMBER(children);
TINYREFL_MATCHERS_GET_MEMBER(bases);

#undef TINYREFL_MATCHERS_GET_MEMBER

constexpr auto ofKind(const tinyrefl::entities::entity_kind kind)
{
    return matches_kind(kind);
}

#define TINYREFL_MATCHERS_KIND_MATCHER(kind, Kind)                             \
    constexpr auto is##kind()                                                  \
    {                                                                          \
        return ofKind(tinyrefl::entities::entity_kind::Kind);                  \
    }

TINYREFL_MATCHERS_KIND_MATCHER(namespace, NAMESPACE)
TINYREFL_MATCHERS_KIND_MATCHER(class, CLASS)
TINYREFL_MATCHERS_KIND_MATCHER(enum, ENUM)
TINYREFL_MATCHERS_KIND_MATCHER(enum_value, ENUM_VALUE)
TINYREFL_MATCHERS_KIND_MATCHER(base_class, BASE_CLASS)
TINYREFL_MATCHERS_KIND_MATCHER(member_variable, MEMBER_VARIABLE)
TINYREFL_MATCHERS_KIND_MATCHER(member_class, MEMBER_CLASS)
TINYREFL_MATCHERS_KIND_MATCHER(member_function, MEMBER_FUNCTION)
TINYREFL_MATCHERS_KIND_MATCHER(constructor, CONSTRUCTOR)
TINYREFL_MATCHERS_KIND_MATCHER(member_enum, MEMBER_ENUM)
TINYREFL_MATCHERS_KIND_MATCHER(object, OBJECT)
TINYREFL_MATCHERS_KIND_MATCHER(static_member_function, STATIC_MEMBER_FUNCTION)
TINYREFL_MATCHERS_KIND_MATCHER(static_member_variable, STATIC_MEMBER_VARIABLE)
TINYREFL_MATCHERS_KIND_MATCHER(function, FUNCTION)
TINYREFL_MATCHERS_KIND_MATCHER(variable, VARIABLE)
TINYREFL_MATCHERS_KIND_MATCHER(file, FILE)
TINYREFL_MATCHERS_KIND_MATCHER(unknown, UNKNOWN)

#undef TINYREFL_MATCHERS_KIND_MATCHER

template<typename InnerMatcher>
constexpr auto has(InnerMatcher&& innerMatcher)
{
    return tinyrefl::meta::combine(
        tinyrefl::meta::hof::map(get_children{}),
        tinyrefl::meta::hof::any_of(std::forward<InnerMatcher>(innerMatcher)));
}

template<typename String>
constexpr auto named(String&& name)
{
    return matches_name(std::forward<String>(name));
}

template<typename String>
constexpr auto display_named(String&& displayName)
{
    return matches_display_name(std::forward<String>(displayName));
}

template<typename... InnerMatchers>
constexpr auto allOf(InnerMatchers&&... innerMatchers)
{
    return tinyrefl::meta::hof::all(
        std::make_tuple(std::forward<InnerMatchers>(innerMatchers)...));
}

template<typename... InnerMatchers>
constexpr auto anyOf(InnerMatchers&&... innerMatchers)
{
    return tinyrefl::meta::hof::any(
        std::make_tuple(std::forward<InnerMatchers>(innerMatchers)...));
}

} // namespace matchers

template<typename Entity, typename Matcher>
constexpr bool matches(const Entity& entity, const Matcher& matcher)
{
    return matcher(entity);
}

} // namespace tinyrefl

#endif // TINYREFL_MATCHERS_MATCHER_HPP_INCLUDED
