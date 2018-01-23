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


namespace tinyrefl
{

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

    if(entity.parent().has_value() && entity.parent().value().kind() != cppast::cpp_entity_kind::file_t)
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
    return fmt::format("tinyrefl::meta::list<{}>", sequence(args, ", "));
}

std::string enum_declaration(const std::string& name, const std::vector<std::string>& values)
{
    return fmt::format("enum class {} {{{}}};", name, sequence(values, ", "));
}

std::string member_pointer(const cppast::cpp_entity& member)
{
    return fmt::format("&{}", full_qualified_name(member));
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
    return fmt::format("tinyrefl::backend::member<CTTI_STATIC_VALUE({})>",
        member_pointer(member));
}

void generate_member(std::ostream& os, const cppast::cpp_entity& member)
{
    fmt::print(os, "TINYREFL_REFLECT_MEMBER({})\n", member_pointer(member));
}

void generate_class(std::ostream& os, const cppast::cpp_class& class_)
{
    std::vector<std::string> members;
    std::vector<std::string> base_classes;


    std::cout << " # " << full_qualified_name(class_) << " [attributes: "
                << sequence(class_.attributes(), ", ", "\"", "\"") << "]\n";


    cppast::visit(class_, [&](const cppast::cpp_entity& child, const cppast::visitor_info& info)
    {
        if(cppast::has_attribute(child, ATTRIBUTES_IGNORE))
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

    fmt::print(os, "TINYREFL_REFLECT_CLASS({}, {}, {})\n",
        full_qualified_name(class_),
        typelist(base_classes),
        typelist(members)
    );
}

void generate_enum(std::ostream& os, const cppast::cpp_enum& enum_)
{
    std::cout << " # " << full_qualified_name(enum_) << " [attributes: "
                << sequence(enum_.attributes(), ", ", "\"", "\"") << "]\n";

    if(!cppast::has_attribute(enum_, ATTRIBUTES_IGNORE))
    {
        std::vector<std::string> values;

        cppast::visit(enum_, [&values](const cppast::cpp_entity& entity, const cppast::visitor_info& info)
        {
            if(entity.kind() == cppast::cpp_enum_value::kind())
            {
                std::cout << "    - (enum value) " << full_qualified_name(entity) << "\n";
                values.push_back(fmt::format("CTTI_STATIC_VALUE({})", full_qualified_name(entity)));
            }
        });

        fmt::print(os, "TINYREFL_REFLECT_ENUM({}, {})\n",
            full_qualified_name(enum_),
            typelist(values)
        );
    }
}


void visit_ast_and_generate(const cppast::cpp_file& ast_root, const std::string& filepath)
{
    std::ofstream os{filepath + ".tinyrefl"};

    std::cout << "parsing file " << filepath << " ...\n";

    const auto include_guard = fmt::format("TINYREFL_GENERATED_FILE_{}_INCLUDED", std::hash<std::string>()(filepath));

    os << "#ifndef " << include_guard << "\n";
    os << "#define " << include_guard << "\n\n";
    os << "#include <tinyrefl/backend.hpp>\n\n";

    cppast::visit(ast_root,
        [](const cppast::cpp_entity& e) {
            return !cppast::is_templated(e) &&
                   cppast::is_definition(e) &&
                   !cppast::has_attribute(e, ATTRIBUTES_IGNORE);
        },
        [&os](const cppast::cpp_entity& e, const cppast::visitor_info& info) {
            if(info.is_new_entity() && info.access == cppast::cpp_public)
            {
                switch(e.kind())
                {
                case cppast::cpp_entity_kind::class_t:
                    generate_class(os, static_cast<const cppast::cpp_class&>(e)); break;
                case cppast::cpp_entity_kind::enum_t:
                    generate_enum(os, static_cast<const cppast::cpp_enum&>(e)); break;
                default:
                    break;
                }
            }
        });

    os << "\n#endif // " << include_guard << "\n";

    std::cout << "Done. Metadata saved in " << filepath << ".tinyrefl\n";
}

bool reflect_file(const std::string& filepath)
{
    using parser_t = cppast::simple_file_parser<cppast::libclang_parser>;

    cppast::cpp_entity_index index;
    parser_t parser{type_safe::ref(index)};
    parser_t::config config;
    config.add_include_dir(TINYREFL_INCLUDE_DIR);
    config.add_include_dir(CTTI_INCLUDE_DIR);
    config.add_include_dir(FMT_INCLUDE_DIR);
    config.add_include_dir(MASQUERADE_INCLUDE_DIR);
    config.set_flags(cppast::cpp_standard::cpp_14);

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

}
