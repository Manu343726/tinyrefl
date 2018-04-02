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

#ifdef __cpp_lib_experimental_filesystem
#include <experimental/filesystem>

using fs = std::experimental::filesystem;

bool is_outdated_file(const std::string& file)
{
    const fs::path output_file{file + ".tinyrefl"};
    const fs::path input_file{file};
    const auto last_output = fs::last_write_time(output_file);
    const auto last_input = fs::last_write_time(input_file);

    std::cout << output_file << " (" << last_output << ") vs " << input_file << " (" << last_input << ")\n";

    return fs::exists(output_file) && fs::last_write_time(output_file) < fs::last_write_time(input_file);
}

#else

bool is_outdated_file(const std::string& file)
{
    return true;
}

#endif // __cpp_lib_experimental_filesystem

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
           cppast::is_templated(child))
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
    fmt::format("TINYREFL_REFLECT_ENUM_VALUE({})\n", enum_value(value));
}

void generate_enum(std::ostream& os, const cppast::cpp_enum& enum_)
{
    std::cout << " # " << full_qualified_name(enum_) << " [attributes: "
                << sequence(enum_.attributes(), ", ", "\"", "\"") << "]\n";

    if(!cppast::has_attribute(enum_, ATTRIBUTES_IGNORE))
    {
        std::vector<std::string> values;

        cppast::visit(enum_, [&values, &os](const cppast::cpp_entity& entity, const cppast::visitor_info& info)
        {
            if(entity.kind() == cppast::cpp_enum_value::kind())
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
            if(info.is_new_entity() && info.access == cppast::cpp_public)
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
        {"98", cppast::cpp_standard::cpp_98},
        {"03", cppast::cpp_standard::cpp_03},
        {"11", cppast::cpp_standard::cpp_11},
        {"14", cppast::cpp_standard::cpp_14},
        {"1z", cppast::cpp_standard::cpp_1z},
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

bool reflect_file(const std::string& filepath, const std::string& cpp_standard, const std::vector<std::string>& include_dirs)
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
    config.set_flags(get_cpp_standard(cpp_standard));
    config.add_flag("-fPIC");

    std::cout << "parsing file " << filepath << " -std=c++"
        << cpp_standard << " ";

    for(const std::string& include_dir : include_dirs)
    {
        std::cout << "-I" << include_dir << " ";
        config.add_include_dir(include_dir);
    }

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
    if(argc > 2)
    {
        std::vector<std::string> include_dirs;
        include_dirs.reserve(argc - 3);

        for(int i = 3; i < argc; ++i)
        {
            include_dirs.push_back(argv[i]);
        }

        return !reflect_file(argv[1], argv[2], include_dirs);
    }
    else
    {
        std::cerr << "usage: tool <path to file> <c++ standard> [<include dirs>...]\n";
        return 1;
    }
}
