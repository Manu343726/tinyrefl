#include <catch.hpp>
#include <cppast/libclang_parser.hpp>
#include <cppast/visitor.hpp>
#include <jinja2cpp/template.h>
#include <tinyrefl/tool/model/cppast/jinja2reflection.hpp>

using namespace tinyrefl::tool::model;

TEST_CASE("tool::cppast-entities-jinja2-reflection")
{
    using parser_t = cppast::simple_file_parser<cppast::libclang_parser>;
    cppast::cpp_entity_index index;
    parser_t                 parser{type_safe::ref(index)};
    parser_t::config         config;
    config.set_flags(cppast::cpp_standard::cpp_11);

    auto generate_class = [&index](const cppast::cpp_class& class_) {


        jinja2::Template tmpl;
        tmpl.Load(R"(
{{class.fullname}}:
{% for variable in class.variables %}
 - {{variable.name}} ({{variable.fullname}}). Type: {{variable.type}}
{% endfor %}
)");

        const auto result = tmpl.RenderAsString(jinja2::ValuesMap{
            {"class", jinja2::Reflect(make_entity_ref(index, class_))}});

        const std::string expected_string = R"(
Class:
 - i (Class::i). Type: int Class::*
)";
        CHECK(result == expected_string);
    };

    auto file =
        parser.parse(TINYREFL_TOOL_SOURCE_DIR "/tests/class.hpp", config);

    REQUIRE(file.has_value());

    cppast::visit(
        file.value(),
        [=](const cppast::cpp_entity&   entity,
            const cppast::visitor_info& info) {
            if(entity.kind() == cppast::cpp_entity_kind::class_t &&
               entity.name() == "Class")
            {
                generate_class(static_cast<const cppast::cpp_class&>(entity));
            }
        });
}
