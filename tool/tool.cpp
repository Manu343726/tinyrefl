#include <cppast/cpp_class.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/libclang_parser.hpp>
#include <cppast/parser.hpp>
#include <cppast/visitor.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iostream>
#include <string>
#include <functional>
#include <regex>
#include <unordered_set>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <llvm/Support/CommandLine.h>

namespace cl = llvm::cl;

bool is_outdated_file(const std::string& file)
{
    const auto input_file = cppfs::fs::open(file);
    const auto output_file = cppfs::fs::open(file + ".tinyrefl");

    assert(input_file.exists());

    return !output_file.exists() || input_file.modificationTime() > output_file.modificationTime();
}

static const std::string ATTRIBUTES_IGNORE = "tinyrefl::ignore";

template<typename Seq>
std::string sequence(const Seq& elems, const std::string& separator,
        const std::string& prefix = "", const std::string& suffix = "")
{
    std::ostringstream os;

    for(std::size_t i = 0; i < elems.size(); ++i)
    {
        os << prefix << elems[i] << suffix;

        if(i < elems.size() - 1)
        {
            os << separator;
        }
    }

    return os.str();
}

namespace cppast
{

std::ostream& operator<<(std::ostream& os, const cppast::cpp_attribute& attribute)
{
    if(attribute.scope().has_value())
    {
        os << attribute.scope().value() << "::";
    }

    if(attribute.arguments().has_value())
    {
        return os << attribute.name() << "(" << attribute.arguments().value().as_string() << ")";
    }
    else
    {
        return os << attribute.name();
    }
}

}

bool is_reflectable(const cppast::cpp_entity& e)
{
    return cppast::has_attribute(e, "tinyrefl::on").has_value();
}

void generate_symbol_declaration(std::ostream& os, const std::string& symbol)
{
    fmt::print(os,
"#ifndef TINYREFL_SYMBOL_{0}_DEFINED\n"
"#define TINYREFL_SYMBOL_{0}_DEFINED\n"
"namespace symbols {{\n"
    "CTTI_DEFINE_SYMBOL({0});\n"
"}} // symbols\n"
"#endif // TINYREFL_SYMBOL_{0}_DEFINED\n", symbol);
}

std::string unqualified_name(const cppast::cpp_entity& entity)
{
    auto rename_command = cppast::has_attribute(entity, "tinyrefl::rename");

    if(rename_command)
    {
        const auto arguments = rename_command.value().arguments();

        if(!arguments || arguments.value().empty())
        {
            throw std::runtime_error{fmt::format("[error] expected argument for [[tinyrefl::rename(name)]] attribute")};
        }
        else if(arguments.value().end() - arguments.value().begin() != 1)
        {
            throw std::runtime_error{fmt::format("[error] expected one argument for [[tinyrefl::rename(name)]] attribute")};
        }
        else if(arguments.value().front().kind != cppast::cpp_token_kind::string_literal)
        {
            throw std::runtime_error{fmt::format("[error] expected string literal as argument for [[tinyrefl::rename(name)]] attribute")};
        }
        else
        {
            return arguments.value().front().spelling;
        }
    }
    else
    {
        return entity.name();
    }
}

std::string full_qualified_name(const cppast::cpp_entity& entity)
{
    std::string name = entity.name();

    if(entity.kind() == cppast::cpp_entity_kind::base_class_t)
    {
        return cppast::to_string(static_cast<const cppast::cpp_base_class&>(entity).type());
    }
    else if(entity.parent().has_value() && entity.parent().value().kind() != cppast::cpp_entity_kind::file_t)
    {
        return fmt::format("{}::{}", full_qualified_name(entity.parent().value()), name);
    }
    else
    {
        return fmt::format("{}", name);
    }
}

std::string typelist(const std::vector<std::string>& args)
{
    return fmt::format("TINYREFL_SEQUENCE(({}))", sequence(args, ", "));

}

static std::unordered_set<std::string> string_registry;

void generate_string_definition(std::ostream& os, const std::string& str)
{
    const auto hash = std::hash<std::string>()(str);
    const auto guard = fmt::format("TINYREFL_DEFINE_STRING_{}", hash);

    os << "#if defined(TINYREFL_DEFINE_STRINGS) && !defined(" << guard << ")\n"
       << "#define " << guard << "\n"
       << "TINYREFL_DEFINE_STRING(" << str << ")\n"
       << "#endif //" << guard << "\n\n";
}

void generate_string_definitions(std::ostream& os)
{
    for(const std::string& str : string_registry)
    {
        generate_string_definition(os, str);
    }
}

template<typename T>
std::string value(const T& value)
{
    return fmt::format("TINYREFL_VALUE({})", value);
}

const std::string& string(const std::string& str)
{
    string_registry.insert(str);
    return str;
}

std::string string_constant(const std::string& str)
{
    return fmt::format("TINYREFL_STRING({})", string(str));
}

std::string type_reference(const cppast::cpp_entity& type_declaration)
{
    return fmt::format("TINYREFL_TYPE({}, {})",
        string(type_declaration.name()), string(full_qualified_name(type_declaration)));
}

std::string enum_declaration(const std::string& name, const std::vector<std::string>& values)
{
    return fmt::format("enum class {} {{{}}};", name, sequence(values, ", "));
}

std::string enum_value(const cppast::cpp_enum_value& enum_value)
{
    return fmt::format("TINYREFL_ENUM_VALUE({}, {}, {}, {})",
        string_constant(enum_value.name()), string_constant(full_qualified_name(enum_value)),
        type_reference(enum_value.parent().value()), value(string(full_qualified_name(enum_value))));
}

std::string member_pointer(const cppast::cpp_entity& member)
{
    return fmt::format("TINYREFL_MEMBER({}, {}, {}, {})",
        string_constant(member.name()), string_constant(full_qualified_name(member)), type_reference(member.parent().value()), value(string("&" + full_qualified_name(member))));
}

std::string string_literal(const std::string& str)
{
    return fmt::format("\"{}\"", str);
}

std::string typelist_string(const std::string& str)
{
    return fmt::format("tinyrefl::meta::string<{}>", sequence(str, ", ", "'", "'"));
}

std::string member_instance(const cppast::cpp_entity& member)
{
    return member_pointer(member);
}

void generate_member(std::ostream& os, const cppast::cpp_entity& member)
{
    fmt::print(os, "TINYREFL_REFLECT_MEMBER({})\n", member_pointer(member));
}

bool is_unknown_entity(const cppast::cpp_entity& entity)
{
    auto parent = entity.parent();

    if(entity.name().empty())
    {
        std::cerr << "[warning] Found " << cppast::to_string(entity.kind()) << " with empty name";

        if(parent.has_value())
        {
            std::cerr << " at " << full_qualified_name(parent.value());
        }

        std::cerr << "\n";

        return true;
    }

    if(parent.has_value() && is_unknown_entity(parent.value()))
    {
        return true;
    }

    return false;
}

void generate_class(std::ostream& os, const cppast::cpp_class& class_)
{
    std::vector<std::string> members;
    std::vector<std::string> base_classes;
    std::vector<std::string> enums;
    std::vector<std::string> classes;

    std::cout << " # " << full_qualified_name(class_) << " [attributes: "
                << sequence(class_.attributes(), ", ", "\"", "\"") << "]\n";


    cppast::visit(class_, [&](const cppast::cpp_entity& child, const cppast::visitor_info& info)
    {
        if(cppast::has_attribute(child, ATTRIBUTES_IGNORE) || info.is_old_entity() ||
           info.access != cppast::cpp_access_specifier_kind::cpp_public ||
           cppast::is_templated(child) ||
           child.parent() != class_ ||
           is_unknown_entity(child))
        {
            return;
        }

        switch(child.kind())
        {
            case cppast::cpp_entity_kind::member_function_t:
            case cppast::cpp_entity_kind::member_variable_t:
            {
                std::cout << "    - (member) " << child.name() << " [attributes: "
                          << sequence(child.attributes(), ", ", "\"", "\"") << "]\n";
                members.push_back(member_instance(child));
                generate_member(os, child);
                break;
            }
            case cppast::cpp_entity_kind::class_t:
            {
                if(child.name() != class_.name())
                {
                    std::cout << "    - (class) " << child.name() << " ("
                              << (static_cast<const cppast::cpp_class&>(child).is_declaration() ? "declaration" :
                                  static_cast<const cppast::cpp_class&>(child).is_definition() ? "definition" : "") << ") [attributes: "
                              << sequence(child.attributes(), ", ", "\"", "\"") << "]\n";
                    classes.push_back(full_qualified_name(child));
                }
                break;
            }
            case cppast::cpp_entity_kind::enum_t:
            {
                std::cout << "    - (enum) " << child.name() << " [attributes: "
                          << sequence(child.attributes(), ", ", "\"", "\"") << "]\n";
                enums.push_back(full_qualified_name(child));
                break;
            }
            default:
                break;
        }
    });

    for(const auto& base_class : class_.bases())
    {
        if(!cppast::has_attribute(base_class, ATTRIBUTES_IGNORE))
        {
            std::cout << "    - (base) " << full_qualified_name(base_class) << "\n";
            base_classes.push_back(full_qualified_name(base_class));
        }
    }

    fmt::print(os, "TINYREFL_REFLECT_CLASS({}, \n"
"// Base classes:\n"
"{},\n"
"// Members: \n"
"{},\n"
"// Member classes: \n"
"{},\n"
"// Member enums: \n"
"{}\n"
")\n",
        type_reference(class_),
        typelist(base_classes),
        typelist(members),
        typelist(classes),
        typelist(enums)
    );
}

void generate_enum_value(std::ostream& os, const cppast::cpp_enum_value& value)
{
    fmt::print(os, "TINYREFL_REFLECT_ENUM_VALUE({})\n", enum_value(value));
}

void generate_enum(std::ostream& os, const cppast::cpp_enum& enum_)
{
    std::cout << " # " << full_qualified_name(enum_) << " [attributes: "
                << sequence(enum_.attributes(), ", ", "\"", "\"") << "]\n";

    if(!cppast::has_attribute(enum_, ATTRIBUTES_IGNORE))
    {
        std::vector<std::string> values;

        cppast::visit(enum_, [&values, &os](const cppast::cpp_entity& entity, const cppast::visitor_info&)
        {
            if(entity.kind() == cppast::cpp_enum_value::kind() &&
               !is_unknown_entity(entity))
            {
                const auto& value = static_cast<const cppast::cpp_enum_value&>(entity);

                std::cout << "    - (enum value) " << full_qualified_name(entity) << "\n";

                generate_enum_value(os, value);
                values.push_back(enum_value(value));
            }
        });

        fmt::print(os, "TINYREFL_REFLECT_ENUM({}, {}, {})\n",
            string_constant(full_qualified_name(enum_)),
            type_reference(enum_),
            typelist(values)
        );
    }
}


void visit_ast_and_generate(const cppast::cpp_file& ast_root, const std::string& filepath)
{
    std::ofstream os{filepath + ".tinyrefl"};

    const auto include_guard = fmt::format("TINYREFL_GENERATED_FILE_{}_INCLUDED", std::hash<std::string>()(filepath));

    os << "#ifndef " << include_guard << "\n"
       << "#define " << include_guard << "\n\n"
       <<
#include "metadata_header.hpp"
       << std::endl;

    std::ostringstream body;

    cppast::visit(ast_root,
        [](const cppast::cpp_entity& e) {
            return !cppast::is_templated(e) &&
                   cppast::is_definition(e) &&
                   !cppast::has_attribute(e, ATTRIBUTES_IGNORE);
        },
        [&body](const cppast::cpp_entity& e, const cppast::visitor_info& info) {
            if(info.is_new_entity() && info.access == cppast::cpp_public && !is_unknown_entity(e))
            {
                switch(e.kind())
                {
                case cppast::cpp_entity_kind::class_t:
                    generate_class(body, static_cast<const cppast::cpp_class&>(e)); break;
                case cppast::cpp_entity_kind::enum_t:
                    generate_enum(body, static_cast<const cppast::cpp_enum&>(e)); break;
                default:
                    break;
                }
            }
        });

    generate_string_definitions(os);
    os << body.str();

    os << "\n#endif // " << include_guard << "\n";

    std::cout << "Done. Metadata saved in " << filepath << ".tinyrefl\n";
}

cppast::cpp_standard get_cpp_standard(const std::string& cpp_standard)
{
    static const std::unordered_map<std::string, cppast::cpp_standard> map{
        {"c++98", cppast::cpp_standard::cpp_98},
        {"c++03", cppast::cpp_standard::cpp_03},
        {"c++11", cppast::cpp_standard::cpp_11},
        {"c++14", cppast::cpp_standard::cpp_14},
        {"c++1z", cppast::cpp_standard::cpp_1z},
    };

    auto it = map.find(cpp_standard);

    if(it != map.end())
    {
        return it->second;
    }
    else
    {
        return cppast::cpp_standard::cpp_latest;
    }
}

struct compile_definition
{
    std::string macro;
    std::string value;

    compile_definition() = default;
    compile_definition(std::string macro, std::string value);
    compile_definition(const std::string& input);
};

compile_definition parse_compile_definition(const std::string& input)
{
    std::size_t equal_sign = input.find_first_of('=');

    if(equal_sign != std::string::npos)
    {
        return {input.substr(0, equal_sign), input.substr(equal_sign + 1)};
    }
    else
    {
        return {input, ""};
    }
}

compile_definition::compile_definition(std::string macro, std::string value) :
    macro{std::move(macro)},
    value{std::move(value)}
{}

compile_definition::compile_definition(const std::string& input) :
    compile_definition{parse_compile_definition(input)}
{}

std::istream& operator>>(std::istream& is, compile_definition& compile_definition)
{
    std::string input;
    is >> input;
    compile_definition = parse_compile_definition(input);
    return is;
}

bool reflect_file(const std::string& filepath, const cppast::cpp_standard cpp_standard, cl::list<std::string>& include_dirs, cl::list<std::string>& definitions, cl::list<std::string>& warnings, const std::vector<std::string>& custom_flags)
{
    using parser_t = cppast::simple_file_parser<cppast::libclang_parser>;

    if(!is_outdated_file(filepath))
    {
        std::cout << "file " << filepath << " metadata is up to date, skipping\n";
        return true;
    }

    cppast::cpp_entity_index index;
    parser_t parser{type_safe::ref(index)};
    parser_t::config config;
    config.set_flags(cpp_standard);

    std::cout << "parsing file " << filepath << " "
        << cppast::to_string(cpp_standard) << " ";

    // Add definitions to identify that the translation unit
    // is being parsed by tinyrefl-tool
    definitions.addValue("TINYREFL_TOOL_RUNNING");
    definitions.addValue("__tinyrefl__");

    for(const auto& definition : definitions)
    {
        compile_definition def{definition};
        std::cout << "-D" << def.macro << "=" << def.value << " ";

        if(def.macro.empty())
        {
            std::cout << "(empty, ignored) ";
        }
        else
        {
            config.define_macro(def.macro, def.value);
        }
    }

    for(const std::string& include_dir : include_dirs)
    {
        std::cout << "-I" << include_dir << " ";

        if(include_dir.empty())
        {
            std::cout << "(empty, ignored) ";
        }
        else
        {
            config.add_include_dir(include_dir);
        }
    }

    for(const std::string& warning : warnings)
    {
        auto warning_flag = "-W" + warning;
        std::cout << warning_flag << " ";
        config.add_flag(warning_flag);
    }

    for(const auto& flag : custom_flags)
    {
        std::cout << flag << " ";
        config.add_flag(flag);
    }


    // Tell libclang to ignore unknown arguments
    std::cout << " -Qunused-arguments -Wno-unknown-warning-option";
    config.add_flag("-Qunused-arguments");
    config.add_flag("-Wno-unknown-warning-option");
    std::cout << " ...\n";

    try
    {
        auto file = parser.parse(filepath, config);

        if(file.has_value())
        {
            visit_ast_and_generate(file.value(), filepath);
            return true;
        }
        else
        {
            std::cerr << "error parsing input file\n";
        }
    } catch(const cppast::libclang_error& error)
    {
        std::cerr << "[error] " << error.what() << "\n";
    }

    return false;
}

int main(int argc, char** argv)
{
    cl::opt<std::string>          filename{cl::Positional, cl::desc("<input header>"), cl::Required};
    cl::list<std::string>         includes{"I", cl::Prefix, cl::ValueOptional, cl::desc("Include directories")};
    cl::list<std::string>         definitions{"D", cl::Prefix, cl::ValueOptional, cl::desc("Compile definitions")};
    cl::list<std::string>         warnings{"W", cl::Prefix, cl::ValueOptional, cl::desc("Warnings")};
    cl::opt<cppast::cpp_standard> stdversion{"std", cl::desc("C++ standard"), cl::values(
        clEnumValN(cppast::cpp_standard::cpp_98, "c++98", "C++ 1998 standard"),
        clEnumValN(cppast::cpp_standard::cpp_03, "c++03", "C++ 2003 standard"),
        clEnumValN(cppast::cpp_standard::cpp_11, "c++11", "C++ 2011 standard"),
        clEnumValN(cppast::cpp_standard::cpp_14, "c++14", "C++ 2014 standard"),
        clEnumValN(cppast::cpp_standard::cpp_1z, "c++17", "C++ 2017 standard")
    )};
    cl::list<std::string> custom_flags{cl::Sink, cl::desc("Custom compiler flags")};

    if(cl::ParseCommandLineOptions(argc, argv, "Tinyrefl codegen tool"))
    {
        if(reflect_file(filename, stdversion, includes, definitions, warnings, custom_flags))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 2;
    }
}
