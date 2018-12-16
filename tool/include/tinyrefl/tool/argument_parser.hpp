#ifndef TINYREFL_TOOL_ARGUMENTPARSER_HPP
#define TINYREFL_TOOL_ARGUMENTPARSER_HPP

#include <string>
#include <vector>

namespace tinyrefl
{

namespace tool
{

enum cpp_standard
{
    std_cpp_98,
    std_cpp_03,
    std_cpp_11,
    std_cpp_14,
    std_cpp_17
};

struct compile_definition
{
    std::string macro;
    std::string value;

    compile_definition() = default;
    compile_definition(std::string macro, std::string value);

    static compile_definition parse(const std::string& input);
};


struct cpp_input_file
{
    std::string                     full_path;
    cpp_standard                    standard;
    std::vector<std::string>        include_dirs;
    std::vector<compile_definition> compile_definitions;
    std::vector<std::string>        warnings;
    std::vector<std::string>        extra_flags;
};

struct cpp_output_file
{
    std::string full_path;
    std::string template_dir;
};

struct config
{
    cpp_input_file  input;
    cpp_output_file output;
};

std::pair<std::string, std::string>
       parse_compile_definition(const std::string& define);
config parse_arguments(const int argc, const char** argv);

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_ARGUMENTPARSER_HPP
