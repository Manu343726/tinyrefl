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

    auto file =
        parser.parse(TINYREFL_TOOL_SOURCE_DIR "/tests/class.hpp", config);
    REQUIRE(file.has_value());
    jinja2::ValuesMap variables{
        {"file", jinja2::Reflect(make_entity_ref(index, file.value()))}};

    jinja2::Template tmpl;
    tmpl.Load(R"(
{% for class in file.classes %}
{{class.fullname}}:
 base classes:
{% for base in class.bases %}
  {{base.fullname}} ({{base.access}})
{% endfor %}
 member classes:
{% for class in class.classes %}
  {{class.fullname}} ({{class.access}})
{% endfor %}
 member enums:
{% for enum in class.enums %}
  {{enum.fullname}} ({{enum.access}})
{% for value in enum.values %}
   {{value.name}}:
    - fullname: {{value.fullname}}
    - qualified name: {{value.qualifiedname}}
    - value: {{value.value}}
{% endfor %}
{% endfor %}
 member variables:
{% for variable in class.variables %}
  {{variable.name}}:
   - access: {{variable.access}}
   - full name: {{variable.fullname}}
   - pointer: {{variable.pointer}}
   - type: {{variable.type}}
   - class: {{variable.class.fullname}}
{% endfor %}
 member functions:
{% for function in class.functions %}
  {{function.name}}:
   - access: {{function.access}}
   - full name: {{function.fullname}}
   - signature: {{function.signature}}
   - pointer: {{function.pointer}}
   - type: {{function.type}}
   - class: {{function.class.fullname}}
{% endfor %}
{% endfor %}
)");

    const auto        result          = tmpl.RenderAsString(variables);
    const std::string expected_string = R"(
my::Base:
 base classes:
 member classes:
 member enums:
 member variables:
  str:
   - access: public
   - full name: my::Base::str
   - pointer: &my::Base::str
   - type: std::string my::Base::*
   - class: my::Base
 member functions:
my::Class:
 base classes:
  my::Base (public)
 member classes:
  my::Class::Tag (private)
  my::Class::AnotherTag (private)
 member enums:
  my::Class::Enum (private)
   A:
    - fullname: my::Class::Enum::A
    - qualified name: Enum::A
    - value: 0
   B:
    - fullname: my::Class::Enum::B
    - qualified name: Enum::B
    - value: 0
   C:
    - fullname: my::Class::Enum::C
    - qualified name: Enum::C
    - value: 0
 member variables:
  i:
   - access: private
   - full name: my::Class::i
   - pointer: &my::Class::i
   - type: int my::Class::*
   - class: my::Class
 member functions:
  get_i:
   - access: private
   - full name: my::Class::get_i
   - signature: () const
   - pointer: &my::Class::get_i
   - type: int(my::Class::*)() const
   - class: my::Class
  get_i:
   - access: private
   - full name: my::Class::get_i
   - signature: ()
   - pointer: &my::Class::get_i
   - type: int&(my::Class::*)()
   - class: my::Class
my::Class::Tag:
 base classes:
 member classes:
 member enums:
 member variables:
 member functions:
my::Class::AnotherTag:
 base classes:
 member classes:
 member enums:
 member variables:
 member functions:
)";
    CHECK(result == expected_string);
}
