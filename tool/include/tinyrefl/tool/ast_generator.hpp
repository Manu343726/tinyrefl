#ifndef TINYREFL_TOOL_ASTGENERATOR_HPP
#define TINYREFL_TOOL_ASTGENERATOR_HPP

#include <nonstd/expected.hpp>
#include <string>

namespace tinyrefl
{

namespace tool
{

enum class generation_error_kind
{
    filesystem_error,
    cpp_parse_error,
    template_parse_error,
    template_processing_error
};

struct generation_error
{
    generation_error_kind kind;
    std::string           message;

    std::string full_error() const;
};

struct InputFile
{
    std::string path;

    std::vector<std::string> include_dirs;
    std::vector<std::string> extra_flags;
};

using string_generation_result =
    nonstd::expected<std::string, generation_error>;
using file_generation_result = nonstd::expected<void, generation_error>;

string_generation_result generate_from_string_template_as_string(
    const std::string& input_file,
    const std::string& template_,
    const std::string& template_environment_root = "");

string_generation_result generate_from_file_template_as_string(
    const std::string& input_file,
    const std::string& template_,
    const std::string& template_environment_root = "");

file_generation_result generate_from_string_template_as_string(
    const std::string& input_file,
    const std::string& template_,
    const std::string& output_file,
    const std::string& template_environment_root = "");

file_generation_result generate_from_file_template_as_file(
    const std::string& input_file,
    const std::string& template_,
    const std::string& output_file,
    const std::string& template_environment_root = "");

} // namespace tool
} // namespace tinyrefl

#endif // TINYREFL_TOOL_ASTGENERATOR_HPP
