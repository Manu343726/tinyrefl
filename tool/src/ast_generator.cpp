#include <boost/filesystem/path.hpp>
#include <cppast/libclang_parser.hpp>
#include <jinja2cpp/template.h>
#include <jinja2cpp/template_env.h>
#include <tinyrefl/tool/ast_generator.hpp>

namespace tinyrefl
{

namespace tool
{

file_generation_result generate_to_ostream(
    const std::string& input_file,
    const std::string& template_content,
    const std::string& template_environment_root,
    std::ostream&      output)
{
}

string_generation_result generate_from_string_template_as_string(
    const std::string& input_file,
    const std::string& template_,
    const std::string& template_environment_root)
{
}

string_generation_result generate_from_file_template_as_string(
    const std::string& input_file,
    const std::string& template_,
    const std::string& template_environment_root)
{
}

file_generation_result generate_from_string_template_as_string(
    const std::string& input_file,
    const std::string& template_,
    const std::string& output_file,
    const std::string& template_environment_root)
{
}

file_generation_result generate_from_file_template_as_file(
    const std::string& input_file,
    const std::string& template_,
    const std::string& output_file,
    const std::string& template_environment_root)
{
}

} // namespace tool
} // namespace tinyrefl
