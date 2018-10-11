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

struct cpp_input_file
{
    std::string                                      full_path;
    cpp_standard                                     standard;
    std::vector<std::string>                         include_dirs;
    std::vector<std::pair<std::string, std::string>> compile_definitions;
    std::vector<std::string>                         warnings;
    std::vector<std::string>                         extra_flags;
};



} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_ARGUMENTPARSER_HPP
