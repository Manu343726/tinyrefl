#include <iostream>
#include <llvm/Support/CommandLine.h>
#include <stdexcept>
#include <tinyrefl/tool/argument_parser.hpp>
#include <tinyrefl/tool/detail/logger.hpp>

namespace cl = llvm::cl;

namespace tinyrefl
{

namespace tool
{

compile_definition compile_definition::parse(const std::string& input)
{
    std::size_t equal_sign = input.find_first_of('=');

    if(equal_sign != std::string::npos)
    {
        return {input.substr(0, equal_sign), input.substr(equal_sign + 1)};
    }
    else
    {
        return {input, ""};
    }
}

compile_definition::compile_definition(std::string macro, std::string value)
    : macro{std::move(macro)}, value{std::move(value)}
{
}

static constexpr const char* version_string =
    "tinyrefl-tool v" TINYREFL_VERSION "\n\n"
    "tinyrefl commit: " TINYREFL_GIT_COMMIT "\n"
    "tinyrefl branch: " TINYREFL_GIT_BRANCH "\n"
    "tinyrefl version: " TINYREFL_VERSION "\n"
    "tinyrefl version major: " TINYREFL_VERSION_MAJOR_STRING "\n"
    "tinyrefl version minor: " TINYREFL_VERSION_MINOR_STRING "\n"
    "tinyrefl version fix:   " TINYREFL_VERSION_FIX_STRING "\n\n"
    "Compiled with LLVM  version: " TINYREFL_LLVM_VERSION "\n"
    "Compiled with cppast version: " CPPAST_VERSION_STRING "\n\n"
    "This tool is part of tinyrefl, a C++ static reflection system\n"
    "See https://gitlab.com/Manu343726/tinyrefl for docs and issues\n";

config parse_arguments(const int argc, const char** argv)
{
    argagg::parser parser{{
        {"help", {"-h", "--help"}, "displays help information", 0},
        {"version", {"--version"}, "displays versio0n information", 0},
        {"includes", {"-I"}, "include directories (can be repeated)", 1},
        {"defines", {"-D"}, "compile definitions (can be repeated)", 1},
        {"warnings", {"-W"}, "warnings (can be repeated)", 1},
    }};

    struct options
    {
        cl::opt<std::string> filename{
            cl::Positional, cl::desc("<input header>"), cl::Required};
        cl::opt<std::string> template_{
            "t",
            cl::desc("Template root directory"),
            cl::value_desc("<directory>"),
            cl::init(TINYREFL_TOOL_TEMPLATES_DIR
                     "/" TINYREFL_TOOL_DEFAULT_TEMPLATE)};
        cl::opt<std::string> output_file{
            "o", cl::desc("Output file"), cl::value_desc("<file path>")};
        cl::list<std::string> includes{"I",
                                       cl::Prefix,
                                       cl::ValueOptional,
                                       cl::desc("Include directories"),
                                       cl::value_desc("<include dir>")};
        cl::list<std::string> definitions{
            "D",
            cl::Prefix,
            cl::ValueOptional,
            cl::desc("Compile definitions"),
            cl::value_desc("<MACRO> or <MACRO>=<VALUE>")};
        cl::list<std::string> warnings{"W",
                                       cl::Prefix,
                                       cl::ValueOptional,
                                       cl::desc("Warning"),
                                       cl::value_desc("<warning>")};
        cl::opt<cpp_standard> stdversion{
            "std",
            cl::desc("C++ standard"),
            cl::value_desc("<C++ standard>"),
            cl::values(
                clEnumValN(
                    cpp_standard::std_cpp_98, "c++98", "C++ 1998 standard"),
                clEnumValN(
                    cpp_standard::std_cpp_03, "c++03", "C++ 2003 standard"),
                clEnumValN(
                    cpp_standard::std_cpp_11, "c++11", "C++ 2011 standard"),
                clEnumValN(
                    cpp_standard::std_cpp_14, "c++14", "C++ 2014 standard"),
                clEnumValN(
                    cpp_standard::std_cpp_17, "c++17", "C++ 2017 standard"))};
        cl::list<std::string> custom_flags{cl::Sink,
                                           cl::desc("Custom compiler flags"),
                                           cl::value_desc("<more flags...>")};
    };

    // We use an static variable to avoid registering options more
    // than once, which is not supported by llvm command line parser
    static options options;

#if TINYREFL_LLVM_VERSION_MAJOR >= 6
    cl::SetVersionPrinter([](llvm::raw_ostream& out) { print_version(out); });
#else
    cl::SetVersionPrinter(+[] { print_version(std::cout); });
#endif // TINYREFL_LLVM_VERSION_MAJOR

    detail::log().info("parsing command line args...");
    constexpr bool IgnoreParsingErrors = true;

    if(cl::ParseCommandLineOptions(
           argc, argv, "Tinyrefl codegen tool", IgnoreParsingErrors))
    {
        config result;
        result.input.full_path    = options.filename;
        result.input.standard     = options.stdversion;
        result.input.include_dirs = options.includes;

        for(const auto& define : options.definitions)
        {
            result.input.compile_definitions.push_back(
                compile_definition::parse(define));
        }

        result.input.warnings    = options.warnings;
        result.input.extra_flags = options.custom_flags;

        if(!options.output_file.empty())
        {
            result.output.full_path = options.output_file;
        }
        else
        {
            result.output.full_path = result.input.full_path + ".tinyrefl";
        }

        result.output.template_dir = options.template_;

        return result;
    }
    else
    {
        detail::log().error("Error parsing command line arguments");
        throw std::invalid_argument{"Error parsing command line arguments"};
    }
}
} // namespace tool

} // namespace tinyrefl
