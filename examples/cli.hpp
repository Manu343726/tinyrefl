#ifndef TINYREFL_EXAMPLES_CLI_HPP_INCLUDED
#define TINYREFL_EXAMPLES_CLI_HPP_INCLUDED

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <fmt/format.h>

namespace example_cli
{

struct [[cli::option("-I{directory}")]] IncludeDir
{
    std::string directory;
};

struct [[cli::option("-D{name}[={value}]")]] Define
{
    std::string                name;
    std::optional<std::string> value;
};

enum class [[cli::option("-std=c++{value}")]] Std{
    CPP_11 = 11, CPP_14 = 14, CPP_17 = 17, CPP_20 = 20};

using CompileOption = std::variant<IncludeDir, Define, Std>;

struct Compiler
{
    bool build(
        const std::string&                             file,
        const std::vector<example_cli::CompileOption>& compileOptions,
        const std::string&                             outputFile)
    {
        fmt::print("Building {}, output: {}\n", file, outputFile);
        return true;
    }
};

struct [[cli::parse("{name}/{version}")]] Package
{
    std::string name;
    int         version;
};

struct Dependencies
{
    bool fetch(const Package& package)
    {
        fmt::print(
            "Fetching package '{}' version {} ...\n",
            package.name,
            package.version);

        return true;
    }

    bool push(const Package& package)
    {
        fmt::print(
            "Uploading package '{}' version {}...\n",
            package.name,
            package.version);

        return true;
    }
};

struct [[cli::entrypoint]] Buildsystem
{
    Compiler     compiler;
    Dependencies deps;
};

} // namespace example_cli

#endif // TINYREFL_EXAMPLES_CLI_HPP_INCLUDED
