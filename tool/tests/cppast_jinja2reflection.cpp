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
    config.add_include_dir(TINYREFL_INCLUDE_DIR);

    auto file =
        parser.parse(TINYREFL_TOOL_SOURCE_DIR "/tests/class.hpp", config);
    REQUIRE(file.has_value());
    jinja2::ValuesMap variables{
        {"file", jinja2::Reflect(make_entity_ref(index, file.value()))}};

    jinja2::Template tmpl;
    tmpl.Load(R"(
{% for class in file.classes %}
{{class.fullname}}:
 comment: {{class.comment | trim}}
 attributes:
{% for attribute in class.attributes %}
  {{attribute.full}}
   - name: {{attribute.name}}
   - full attribute: {{attribute.full}}
   - has arguments: {{attribute.has_arguments}}
{% if attribute.has_arguments %}
   - arguments: {{attribute.arguments}}
   - tokenized arguments: {{attribute.tokenized_arguments | map(attribute='spelling') | join(' ')}}
{% endif %}
{% endfor %}
 base classes:
{% for base in class.bases %}
  {{base.fullname}} ({{base.access}}) [[attributes: {{base.attributes | map(attribute='full') | join(', ')}}]]
{% endfor %}
 member classes:
{% for class in class.classes %}
  {{class.fullname}} ({{class.access}}) [[attributes: {{class.attributes | map(attribute='full') | join(', ')}}]]
{% endfor %}
 member enums:
{% for enum in class.enums %}
  {{enum.fullname}} ({{enum.access}}) [[attributes: {{enum.attributes | map(attribute='full') | join(', ')}}]]
   comment: {{enum.comment | trim}}
{% for value in enum.values %}
   {{value.name}} [[attributes: {{value.attributes | map(attribute='full') | join(', ')}}]]:
    - comment: {{value.comment | trim}}
    - fullname: {{value.fullname}}
    - qualified name: {{value.qualifiedname}}
    - value: {{value.value}}
{% endfor %}
{% endfor %}
 user defined constructors:
{% for constructor in class.constructors %}
  {{constructor.name}} [[attributes: {{constructor.attributes | map(attribute='full') | join(', ')}}]]:
   - comment: {{constructor.comment | trim}}
   - full name: {{constructor.fullname}}
   - signature: {{constructor.signature}}
   - class: {{constructor.class.fullname}}
{% endfor %}
 member variables:
{% for variable in class.variables %}
  {{variable.name}} [[attributes: {{variable.attributes | map(attribute='full') | join(', ')}}]]:
   - comment: {{variable.comment | trim}}
   - access: {{variable.access}}
   - full name: {{variable.fullname}}
   - pointer: {{variable.pointer}}
   - type: {{variable.type}}
   - class: {{variable.class.fullname}}
{% endfor %}
 member functions:
{% for function in class.functions %}
  {{function.name}} [[attributes: {{function.attributes | map(attribute='full') | join(', ')}}]]:
   - comment: {{function.comment | trim}}
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
 comment: 
 attributes:
  Name("Base",type="class")
   - name: Name
   - full attribute: Name("Base",type="class")
   - has arguments: true
   - arguments: "Base",type="class"
   - tokenized arguments: "Base" , type = "class"
 base classes:
 member classes:
 member enums:
 user defined constructors:
 member variables:
  str [[attributes: ]]:
   - comment: 
   - access: public
   - full name: my::Base::str
   - pointer: &my::Base::str
   - type: std::string my::Base::*
   - class: my::Base
 member functions:
my::Class:
 comment: A class
 attributes:
  Class
   - name: Class
   - full attribute: Class
   - has arguments: false
 base classes:
  my::Base (public) [[attributes: Name("Base",type="class")]]
 member classes:
  my::Class::Tag (private) [[attributes: Tag]]
  my::Class::AnotherTag (private) [[attributes: ]]
 member enums:
  my::Class::Enum (private) [[attributes: Enum]]
   comment: An enum
   A [[attributes: ]]:
    - comment: 
    - fullname: my::Class::Enum::A
    - qualified name: Enum::A
    - value: 0
   B [[attributes: ]]:
    - comment: An enum value
    - fullname: my::Class::Enum::B
    - qualified name: Enum::B
    - value: 0
   C [[attributes: C]]:
    - comment: 
    - fullname: my::Class::Enum::C
    - qualified name: Enum::C
    - value: 0
 user defined constructors:
  Class [[attributes: ctor("int: i")]]:
   - comment: A constructor
   - full name: my::Class::Class
   - signature: (int)
   - class: my::Class
 member variables:
  i [[attributes: property("i")]]:
   - comment: A member variable
   - access: private
   - full name: my::Class::i
   - pointer: &my::Class::i
   - type: int my::Class::*
   - class: my::Class
 member functions:
  get_i [[attributes: property_getter("i")]]:
   - comment: A getter member function
   - access: private
   - full name: my::Class::get_i
   - signature: () const
   - pointer: &my::Class::get_i
   - type: int(my::Class::*)() const
   - class: my::Class
  get_i [[attributes: property_setter("i")]]:
   - comment: A setter member function
   - access: private
   - full name: my::Class::get_i
   - signature: ()
   - pointer: &my::Class::get_i
   - type: int&(my::Class::*)()
   - class: my::Class
my::Class::Tag:
 comment: 
 attributes:
  Tag
   - name: Tag
   - full attribute: Tag
   - has arguments: false
 base classes:
 member classes:
 member enums:
 user defined constructors:
 member variables:
 member functions:
my::Class::AnotherTag:
 comment: 
 attributes:
 base classes:
 member classes:
 member enums:
 user defined constructors:
 member variables:
 member functions:
)";
    CHECK(result == expected_string);
}
