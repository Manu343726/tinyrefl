#include <boost/algorithm/string/replace.hpp>
#include <catch.hpp>
#include <cppast/libclang_parser.hpp>
#include <cppast/visitor.hpp>
#include <jinja2cpp/filesystem_handler.h>
#include <jinja2cpp/template.h>
#include <jinja2cpp/template_env.h>
#include <tinyrefl/tool/model/cppast/jinja2reflection.hpp>

using namespace tinyrefl::tool::model;

using parser_t = cppast::simple_file_parser<cppast::libclang_parser>;

class TestFileParser
{
private:
    cppast::cpp_entity_index                        _index;
    parser_t                                        _parser;
    type_safe::optional_ref<const cppast::cpp_file> _file;
    type_safe::optional<tinyrefl::tool::model::entity_ref<cppast::cpp_file>>
                      _file_ref;
    jinja2::ValuesMap _variables;

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

        _file_ref = type_safe::make_optional(
            tinyrefl::tool::model::make_entity_ref(_index, _file.value()));
        DEBUG_ASSERT(
            _file_ref.has_value(), tinyrefl::tool::detail::assert_handler{});

        _variables["file"] = jinja2::Reflect(_file_ref.value());
    }

    const cppast::cpp_file& file() const
    {
        return _file.value();
    }

    const cppast::cpp_entity_index& index() const
    {
        return _index;
    }

    const jinja2::ValuesMap& variables() const
    {
        return _variables;
    }
};

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
    TestFileParser      context;
    jinja2::TemplateEnv tmplEnv;
    const std::string   rootFolder =
        TINYREFL_TOOL_SOURCE_DIR "/templates/macro_based_codegen";
    jinja2::RealFileSystem fs;
    fs.SetRootFolder(rootFolder);
    tmplEnv.AddFilesystemHandler("", fs);
    jinja2::Template tmpl{&tmplEnv};
    auto             result = tmpl.Load(template_);

    if(!result.has_value())
    {
        FAIL(lexical_cast(result.error()));
    }

    const auto render = tmpl.RenderAsString(context.variables());
    INFO("root: \"" + rootFolder + "\"");
    INFO("With template: \n" + template_);
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
        test_impl(R"({% extends "model.jinja2" %}{% block body %})" + template_body + R"({% endblock body %})", expected_render);
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
    SECTION("file.absolute_path")
    {
        simple_test(
            "{{file.absolute_path}}",
            TINYREFL_TOOL_SOURCE_DIR "/tests/class.hpp");
    }

    SECTION("file.filename")
    {
        simple_test("{{file.filename}}", "class.hpp");
    }

    SECTION("file.classes.length")
    {
        simple_test("{{file.classes|length}}", "4");
    }

    SECTION("file.classes[0].name")
    {
        simple_test("{{file.classes[0].name}}", "Base");
    }

    SECTION("file.classes[0].fullname")
    {
        simple_test("{{file.classes[0].fullname}}", "my::Base");
    }

    SECTION("file.classes names")
    {
        simple_test(
            "{{file.classes|map(attribute='name')|join(',')}}",
            "Base,Class,Tag,AnotherTag");
    }

    SECTION("macro defined in same template")
    {
        simple_test(
            "{% macro string(str) %}{{str}}{% endmacro %}{{string('foo')}}",
            "foo");
    }

    SECTION("macro defined in parent template")
    {
        test("{{tinyrefl_string('foo')}}", "TINYREFL_STRING(foo)");
    }

    SECTION("file include guard")
    {
        test(
            "{{include_guard(file)}}",
            "TINYREFL_TOOL_GENERATED_" +
                boost::algorithm::replace_all_copy(
                    std::string{TINYREFL_TOOL_SOURCE_DIR "/tests/class.hpp"},
                    "/",
                    "_") +
                "_INCLUDED");
    }
}
