#include <boost/algorithm/string/replace.hpp>
#include <catch.hpp>
#include <cppast/libclang_parser.hpp>
#include <cppast/visitor.hpp>
#include <jinja2cpp/template.h>
#include <tinyrefl/tool/model/cppast/jinja2reflection.hpp>

using namespace tinyrefl::tool::model;

using parser_t = cppast::simple_file_parser<cppast::libclang_parser>;

cppast::cpp_entity_index& get_index()
{
    static cppast::cpp_entity_index index;
    return index;
}

class TestFileParser
{
private:
    cppast::cpp_entity_index                        _index;
    parser_t                                        _parser;
    type_safe::optional_ref<const cppast::cpp_file> _file;

public:
    TestFileParser() : _parser{type_safe::ref(_index)}
    {
        parser_t::config config;
        config.set_flags(cppast::cpp_standard::cpp_11);
        config.add_include_dir(TINYREFL_INCLUDE_DIR);

        _file =
            _parser.parse(TINYREFL_TOOL_SOURCE_DIR "/tests/class.hpp", config);

        if(!_file.has_value())
        {
            throw std::runtime_error{"Error parsing file, cannot start test"};
        }
    }

    const cppast::cpp_file& file() const
    {
        return _file.value();
    }

    const cppast::cpp_entity_index& index() const
    {
        return _index;
    }
};

const TestFileParser& get_context()
{
    static TestFileParser testFileParser;
    return testFileParser;
}

template<typename T>
std::string lexical_cast(const T& value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

enum class TemplateMode
{
    SIMPLE,
    EXTEND_MODEL
};

void test_impl(const std::string& template_, const std::string& expected_render)
{
    jinja2::ValuesMap variables{
        {"file",
         jinja2::Reflect(
             make_entity_ref(get_context().index(), get_context().file()))}};

    jinja2::Template tmpl;
    auto             result = tmpl.Load(template_);

    if(!result.has_value())
    {
        FAIL(lexical_cast(result.error()));
    }

    const auto render = tmpl.RenderAsString(variables);

    INFO("With template: \"" + template_ + "\"");
    CHECK(render == expected_render);
}

void test(
    const std::string& template_body,
    const std::string& expected_render,
    const TemplateMode template_mode = TemplateMode::EXTEND_MODEL)
{
    switch(template_mode)
    {
    case TemplateMode::SIMPLE:
    {
        test_impl(template_body, expected_render);
        break;
    }
    case TemplateMode::EXTEND_MODEL:
    {
        // clang-format off
        test_impl(R"(
{% extends ')" TINYREFL_TOOL_SOURCE_DIR R"(/templates/macro_based_codegen/model.jinja2' %}
{% block body %})" +
        template_body + R"(
{% endblock %})", expected_render);
        // clang-format on
    }
    }
}

void simple_test(
    const std::string& template_, const std::string& expected_render)
{
    test(template_, expected_render, TemplateMode::SIMPLE);
}

TEST_CASE("templates::macro_based_codegen::model")
{
    simple_test(
        "{{file.absolute_path}}", TINYREFL_TOOL_SOURCE_DIR "/tests/class.hpp");

    simple_test("{{file.filename}}", "class.hpp");

    simple_test("{{file.classes[0].name}}", "Base");

    simple_test(
        "{{file.classes|map('name')|join(',')}}", "Base,Class,Tag,AnotherTag");

    test(
        "{{include_guard(file)}}",
        "TINYREFL_TOOL_GENERATED_" +
            boost::algorithm::replace_all_copy(
                std::string{TINYREFL_TOOL_SOURCE_DIR "/tests/class.hpp"},
                "/",
                "_") +
            "_INCLUDED");
}
