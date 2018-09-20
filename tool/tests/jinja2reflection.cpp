#include <catch.hpp>
#include <jinja2cpp/template.h>
#include <tinyrefl/tool/jinja2reflection.hpp>
#include <tinyrefl/tool/model/model.hpp>
#include <tinyrefl/tool/model/model.hpp.tinyrefl>

TEST_CASE("tool::jinja2-reflection")
{
    jinja2::Template tmpl;
    tmpl.Load(R"(
{{enum.full_name}}:
{% for value in enum.values %}
 - {{value.name}} ({{value.full_name}}): {{value.value}}
{% endfor %}
)");

    tinyrefl::tool::model::enum_ enum_{
        "my_namespace::MyClass::Enum",
        "Enum",
        std::vector<tinyrefl::tool::model::enum_value>{
            tinyrefl::tool::model::enum_value{
                "my_namespace::MyClass::Enum::A", "A", 0},
            tinyrefl::tool::model::enum_value{
                "my_namespace::MyClass::Enum::B", "B", 1},
            tinyrefl::tool::model::enum_value{
                "my_namespace::MyClass::Enum::C", "C", 2},
            tinyrefl::tool::model::enum_value{
                "my_namespace::MyClass::Enum::D", "D", 42}}};

    const auto result = tmpl.RenderAsString(
        jinja2::ValuesMap{{"enum", jinja2::Reflect(enum_)}});

    const std::string expected_string = R"(
my_namespace::MyClass::Enum:
 - A (my_namespace::MyClass::Enum::A): 0
 - B (my_namespace::MyClass::Enum::B): 1
 - C (my_namespace::MyClass::Enum::C): 2
 - D (my_namespace::MyClass::Enum::D): 42
)";
    CHECK(result == expected_string);
}
