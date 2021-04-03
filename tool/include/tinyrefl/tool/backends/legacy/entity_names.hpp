#ifndef TINYREFL_TOOL_ENTITY_NAMES_HPP_INCLUDED
#define TINYREFL_TOOL_ENTITY_NAMES_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_namespace.hpp>

#include <string>

#include <tinyrefl/tool/backends/legacy/serializer.hpp>

namespace tinyrefl
{

namespace tool
{

class entity_names : tinyrefl::tool::serializer
{
public:
    using tinyrefl::tool::serializer::serializer;

    enum class display_name_includes_file : bool
    {
        YES = true,
        NO  = false
    };

    const std::string& name(const cppast::cpp_file& file);
    const std::string& full_name(const cppast::cpp_file& file);
    const std::string& name(const cppast::cpp_entity& entity);
    const std::string& full_name(const cppast::cpp_entity& entity);
    const std::string& display_name(
        const cppast::cpp_entity&        entity,
        const display_name_includes_file display_name_includes_file =
            display_name_includes_file::NO);
    const std::string& full_display_name(
        const cppast::cpp_entity&        entity,
        const display_name_includes_file display_name_includes_file =
            display_name_includes_file::NO);
    const std::string& display_name(const cppast::cpp_namespace& namespace_);
    const std::string&
        full_display_name(const cppast::cpp_namespace& namespace_);
};
} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_ENTITY_NAMES_HPP_INCLUDED
