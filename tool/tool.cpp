#include <cassert>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/libclang_parser.hpp>
#include <cppast/parser.hpp>
#include <cppast/visitor.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <llvm/Support/CommandLine.h>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>

#include <tinyrefl/tool/serializers/registration_serializer.hpp>
#include <tinyrefl/tool/visitor.hpp>
#include <tinyrefl/types/string.hpp>

#include <cppfs/FileHandle.h>
#include <cppfs/FilePath.h>
#include <cppfs/fs.h>

#include <spdlog/spdlog.h>

namespace cl = llvm::cl;

bool is_outdated_file(const std::string& file)
{
    const auto input_file  = cppfs::fs::open(file);
    const auto output_file = cppfs::fs::open(file + ".tinyrefl");

    assert(input_file.exists());

    bool timestamp_outdated =
        !output_file.exists() ||
        input_file.modificationTime() > output_file.modificationTime();

    if(!timestamp_outdated)
    {
        auto        istream = output_file.createInputStream();
        std::string line;

        std::cout << "[info] checking generated code version...\n";

        if(std::getline(*istream, line) && // Header
           std::getline(*istream, line) && // Blank line
           std::getline(*istream, line))   // commit line
        {
            auto index = line.find_first_of(":");

            if(index != std::string::npos)
            {
                ++index; // Skip the space after the colon
                ++index; // Point to hash substring begin
                const std::string commit = line.substr(index);

                std::cout << "[info] tool commit:           "
                          << TINYREFL_GIT_COMMIT << "\n";
                std::cout << "[info] generated code commit: " << commit << "\n";
                return commit != TINYREFL_GIT_COMMIT;
            }
        }

        return true;
    }
    else
    {
        return true;
    }
}

static const std::string ATTRIBUTES_IGNORE = "tinyrefl::ignore";

void generate_string_definitions(
    std::ostream& os, const tinyrefl::tool::string_registry& string_registry)
{
    for(const auto& string : string_registry)
    {
        const auto hash = tinyrefl::string{string.c_str()}.hash();

        fmt::print(
            os,
            "#ifndef TINYREFL_STRING_{hash}_REGISTERED\n"
            "    #define TINYREFL_STRING_{hash}_REGISTERED\n"
            "    TINYREFL_REGISTER_STRING({string})\n"
            "#endif // TINYREFL_STRING_{hash}_REGISTERED\n",
            fmt::arg("hash", hash),
            fmt::arg("string", string));
    }
}

void generate_global_metadata_list(
    std::ostream&                    os,
    tinyrefl::tool::string_registry& string_registry,
    tinyrefl::tool::entity_registry& entity_registry)
{
    std::vector<std::string> entities;

    std::transform(
        entity_registry.begin(),
        entity_registry.end(),
        std::back_inserter(entities),
        [](const auto& entity_and_serialized_entity) {
            return entity_and_serialized_entity.second;
        });

    const auto entities_sequence =
        tinyrefl::tool::sequence_serializer{string_registry}.sequence(entities);

    os << "#ifndef TINYREFL_GENERATED_FILE_COUNT\n"
          "    #define TINYREFL_GENERATED_FILE_COUNT 1\n"
          "    #define TINYREFL_ENTITIES_0 "
       << entities_sequence
       << "\n"
          "    #ifdef TINYREFL_ENTITIES\n"
          "        #undef TINYREFL_ENTITIES\n"
          "    #endif // TINYREFL_ENTITIES\n"
          "    #define TINYREFL_ENTITIES TINYREFL_ENTITIES_0\n";

    static constexpr int TINYREFL_TOOL_MAX_GENERATED_FILES = 128;

    for(int i = 0; i <= TINYREFL_TOOL_MAX_GENERATED_FILES; ++i)
    {
        if(i < TINYREFL_TOOL_MAX_GENERATED_FILES)
        {
            fmt::print(
                os,
                "#elif TINYREFL_GENERATED_FILE_COUNT == {count}\n"
                "    #undef TINYREFL_GENERATED_FILE_COUNT\n"
                "    #define TINYREFL_GENERATED_FILE_COUNT {next_count}\n"
                "    #define TINYREFL_ENTITIES_{next} TINYREFL_SEQUENCE_CAT((TINYREFL_ENTITIES_{i}), ({entities}))\n"
                "    #undef TINYREFL_ENTITIES\n"
                "    #define TINYREFL_ENTITIES TINYREFL_ENTITIES_{next}\n",
                fmt::arg("i", i),
                fmt::arg("next", i + 1),
                fmt::arg("count", i + 1),
                fmt::arg("next_count", i + 2),
                fmt::arg("entities", entities_sequence));
        }
        else
        {
            fmt::print(
                os,
                "#else\n"
                "    #error Only up to {} tinyrefl generated code headers can be included in the same translation unit\n"
                "#endif // TINYREFL_GENERATED_FILE_COUNT\n\n",
                TINYREFL_TOOL_MAX_GENERATED_FILES);
        }
    }
}


void visit_ast_and_generate(
    cppast::cpp_entity_index& index,
    const cppast::cpp_file&   ast_root,
    const std::string&        filepath)
{
    std::ofstream os{filepath + ".tinyrefl"};

    const auto include_guard = fmt::format(
        "TINYREFL_GENERATED_FILE_{}_INCLUDED",
        std::hash<std::string>()(filepath));

    os << "// Code generated by tinyrefl (https://github.com/Manu343726/tinyrefl)\n"
       << "//\n"
       << "//   tinyrefl commit: " << TINYREFL_GIT_COMMIT << "\n"
       << "//   tinyrefl branch: " << TINYREFL_GIT_BRANCH << "\n"
       << "//   tinyrefl version: " << TINYREFL_VERSION << "\n"
       << "//   tinyrefl version major: " << TINYREFL_VERSION_MAJOR << "\n"
       << "//   tinyrefl version minor: " << TINYREFL_VERSION_MINOR << "\n"
       << "//   tinyrefl version fix: " << TINYREFL_VERSION_FIX << "\n\n"
       << "#ifndef " << include_guard << "\n"
       << "#define " << include_guard << "\n\n"
       << "#define TINYREFL_TOOL_CODEGEN_VERSION_MAJOR "
       << TINYREFL_VERSION_MAJOR << "\n"
       << "#define TINYREFL_TOOL_CODEGEN_VERSION_MINOR "
       << TINYREFL_VERSION_MINOR << "\n"
       << "#define TINYREFL_TOOL_CODEGEN_VERSION_FIX " << TINYREFL_VERSION_FIX
       << "\n"
       << "#define TINYREFL_TOOL_CODEGEN_VERSION \"" << TINYREFL_VERSION
       << "\"\n\n"
       <<
#include "metadata_header.hpp"
       << std::endl;

    tinyrefl::tool::string_registry         string_registry;
    tinyrefl::tool::entity_registry         entity_registry;
    tinyrefl::tool::registration_serializer registration_serializer{
        string_registry, entity_registry, index};
    std::vector<std::string> serialization_queue;

    entity_registry.set_callback([&](const auto&        entity,
                                     const std::string& serialized_entity) {
        if(static_cast<const cppast::cpp_entity*>(&entity) !=
           static_cast<const cppast::cpp_entity*>(&ast_root))
        {
            const auto full_display_name =
                tinyrefl::tool::entity_names{string_registry}.full_display_name(
                    entity);
            const auto entity_id = tinyrefl::hash(full_display_name);

            fmt::print(
                "[debug] {} \"{}\" (id: {}) found and serialized\n",
                cppast::to_string(entity.kind()),
                full_display_name,
                entity_id);

            serialization_queue.push_back(
                registration_serializer.serialize_registration(entity));
        }
    });

    const auto file_registration =
        registration_serializer.serialize_registration(ast_root);

    generate_string_definitions(os, string_registry);
    os << file_registration << "\n";
    generate_global_metadata_list(os, string_registry, entity_registry);

    for(auto it = serialization_queue.rbegin();
        it != serialization_queue.rend();
        ++it)
    {
        os << *it << "\n";
    }

    os << "\n#undef TINYREFL_TOOL_CODEGEN_VERSION_MAJOR\n"
          "#undef TINYREFL_TOOL_CODEGEN_VERSION_MINOR\n"
          "#undef TINYREFL_TOOL_CODEGEN_VERSION_FIX\n"
          "#undef TINYREFL_TOOL_CODEGEN_VERSION\n";

    os << "\n#endif // " << include_guard << "\n";

    std::cout << "Done. Metadata saved in " << filepath << ".tinyrefl\n";
}

cppast::cpp_standard get_cpp_standard(const std::string& cpp_standard)
{
    static const std::unordered_map<std::string, cppast::cpp_standard> map{
        {"c++98", cppast::cpp_standard::cpp_98},
        {"c++03", cppast::cpp_standard::cpp_03},
        {"c++11", cppast::cpp_standard::cpp_11},
        {"c++14", cppast::cpp_standard::cpp_14},
        {"c++1z", cppast::cpp_standard::cpp_1z},
    };

    auto it = map.find(cpp_standard);

    if(it != map.end())
    {
        return it->second;
    }
    else
    {
        return cppast::cpp_standard::cpp_latest;
    }
}

struct compile_definition
{
    std::string macro;
    std::string value;

    compile_definition() = default;
    compile_definition(std::string macro, std::string value);
    compile_definition(const std::string& input);
};

compile_definition parse_compile_definition(const std::string& input)
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

compile_definition::compile_definition(const std::string& input)
    : compile_definition{parse_compile_definition(input)}
{
}

std::istream&
    operator>>(std::istream& is, compile_definition& compile_definition)
{
    std::string input;
    is >> input;
    compile_definition = parse_compile_definition(input);
    return is;
}

bool reflect_file(
    std::string                     filepath,
    const cppast::cpp_standard      cpp_standard,
    cl::list<std::string>&          include_dirs,
    cl::list<std::string>&          definitions,
    cl::list<std::string>&          warnings,
    const std::vector<std::string>& custom_flags,
    const std::string&              clang_binary,
    bool                            force_parsing)
{
    using parser_t = cppast::simple_file_parser<cppast::libclang_parser>;

    if(!is_outdated_file(filepath) && !force_parsing)
    {
        std::cout << "file " << filepath
                  << " metadata is up to date, skipping\n";
        return true;
    }

    cppast::cpp_entity_index index;
    parser_t                 parser{type_safe::ref(index)};
    parser_t::config         config;
    config.set_flags(cpp_standard);

    if(!clang_binary.empty())
    {
        if(!config.set_clang_binary(clang_binary))
        {
            std::cerr
                << "error configuring cppast libclang parser: Clang binary \""
                << clang_binary << "\" not found\n";
            std::exit(1);
        }
    }

    std::cout << "parsing file " << filepath << " "
              << cppast::to_string(cpp_standard) << " ";

    // Add definitions to identify that the translation unit
    // is being parsed by tinyrefl-tool
    definitions.addValue("TINYREFL_TOOL_RUNNING");
    definitions.addValue("__tinyrefl__");

    for(const auto& definition : definitions)
    {
        compile_definition def{definition};
        std::cout << "-D" << def.macro << "=" << def.value << " ";

        if(def.macro.empty())
        {
            std::cout << "(empty, ignored) ";
        }
        else
        {
            config.define_macro(def.macro, def.value);
        }
    }

    for(const std::string& include_dir : include_dirs)
    {
        std::cout << "-I" << include_dir << " ";

        if(include_dir.empty())
        {
            std::cout << "(empty, ignored) ";
        }
        else
        {
            config.add_include_dir(include_dir);
        }
    }

    for(const std::string& warning : warnings)
    {
        auto warning_flag = "-W" + warning;
        std::cout << warning_flag << " ";
        config.add_flag(warning_flag);
    }

    for(const auto& flag : custom_flags)
    {
        std::cout << flag << " ";
        config.add_flag(flag);
    }


    // Tell libclang to ignore unknown arguments
    std::cout << " -Qunused-arguments -Wno-unknown-warning-option";
    config.add_flag("-Qunused-arguments");
    config.add_flag("-Wno-unknown-warning-option");
    std::cout << " ...\n";

    cppfs::FilePath fs_filepath{filepath};

    if(!fs_filepath.isAbsolute())
    {
        filepath = fs_filepath.fullPath();
    }

    try
    {
        auto file = parser.parse(filepath, config);

        if(file.has_value())
        {
            visit_ast_and_generate(index, file.value(), filepath);
            return true;
        }
        else
        {
            std::cerr << "error parsing input file\n";
        }
    }
    catch(const cppast::libclang_error& error)
    {
        std::cerr << "[error] " << error.what() << "\n";
    }

    return false;
}

template<typename Stream>
void print_version(Stream& out)
{
    out << "tinyrefl-tool v" << TINYREFL_VERSION << "\n\n"
        << "tinyrefl commit: " << TINYREFL_GIT_COMMIT << "\n"
        << "tinyrefl branch: " << TINYREFL_GIT_BRANCH << "\n"
        << "tinyrefl version: " << TINYREFL_VERSION << "\n"
        << "tinyrefl version major: " << TINYREFL_VERSION_MAJOR_STRING << "\n"
        << "tinyrefl version minor: " << TINYREFL_VERSION_MINOR_STRING << "\n"
        << "tinyrefl version fix:   " << TINYREFL_VERSION_FIX_STRING << "\n\n"
        << "Compiled with LLVM  version: " << TINYREFL_LLVM_VERSION << "\n"
        << "This tool is part of tinyrefl, a C++ static reflection system\n"
        << "See https://gitlab.com/Manu343726/tinyrefl for docs and issues\n";
}

int main(int argc, char** argv)
{
    cl::opt<std::string> filename{
        cl::Positional, cl::desc("<input header>"), cl::Required};
    cl::list<std::string> includes{
        "I", cl::Prefix, cl::ValueOptional, cl::desc("Include directories")};
    cl::list<std::string> definitions{
        "D", cl::Prefix, cl::ValueOptional, cl::desc("Compile definitions")};
    cl::list<std::string> warnings{
        "W", cl::Prefix, cl::ValueOptional, cl::desc("Warnings")};
    cl::opt<cppast::cpp_standard> stdversion{
        "std",
        cl::desc("C++ standard"),
        cl::values(
            clEnumValN(
                cppast::cpp_standard::cpp_98, "c++98", "C++ 1998 standard"),
            clEnumValN(
                cppast::cpp_standard::cpp_03, "c++03", "C++ 2003 standard"),
            clEnumValN(
                cppast::cpp_standard::cpp_11, "c++11", "C++ 2011 standard"),
            clEnumValN(
                cppast::cpp_standard::cpp_14, "c++14", "C++ 2014 standard"),
            clEnumValN(
                cppast::cpp_standard::cpp_1z, "c++17", "C++ 2017 standard"))};
    cl::list<std::string> custom_flags{cl::Sink,
                                       cl::desc("Custom compiler flags")};
    cl::opt<std::string>  clang_binary{
        "clang-binary",
        cl::ValueOptional,
        cl::desc(
            "clang++ binary. If not given, tinyrefl-tool will search in your PATH")};

    cl::opt<bool> force_parsing{
        "force",
        cl::desc(
            "Force parsing and metadata generation even if the existing metadata is up to date")};
    cl::opt<spdlog::level::level_enum> log_level{
        "log-level",
        cl::desc("Log level"),
        cl::ValueOptional,
        cl::values(
            clEnumValN(spdlog::level::trace, "TRACE", "Trace logging"),
            clEnumValN(spdlog::level::debug, "DEBUG", "Debug logging"),
            clEnumValN(spdlog::level::info, "INFO", "Info logging"),
            clEnumValN(spdlog::level::warn, "WARNING", "Warning logging"),
            clEnumValN(spdlog::level::err, "ERROR", "Error logging"),
            clEnumValN(
                spdlog::level::critical, "CRITICAL", "Critical logging"))};

#if TINYREFL_LLVM_VERSION_MAJOR >= 6
    cl::SetVersionPrinter([](llvm::raw_ostream& out) { print_version(out); });
#else
    cl::SetVersionPrinter(+[] { print_version(std::cout); });
#endif // TINYREFL_LLVM_VERSION_MAJOR

    if(cl::ParseCommandLineOptions(argc, argv, "Tinyrefl codegen tool"))
    {
        if(log_level)
        {
            spdlog::set_level(log_level);
        }

        if(reflect_file(
               filename,
               stdversion,
               includes,
               definitions,
               warnings,
               custom_flags,
               clang_binary,
               force_parsing))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 2;
    }
}
