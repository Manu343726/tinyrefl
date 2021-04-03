#include <cppast/cpp_class.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppfs/FilePath.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <stdexcept>
#include <tinyrefl/tool/backends/cppast/identifier.hpp>
#include <tinyrefl/tool/backends/cppast/visitor.hpp>
#include <tinyrefl/tool/backends/legacy/entity_names.hpp>

using namespace tinyrefl::tool;

const std::string& entity_names::name(const cppast::cpp_file& file)
{
    return string(
        tinyrefl::tool::cppast_backend::IdentifierReader().name(file));
}

const std::string& entity_names::full_name(const cppast::cpp_file& file)
{
    return string(
        tinyrefl::tool::cppast_backend::IdentifierReader().fullName(file));
}

const std::string& entity_names::name(const cppast::cpp_entity& entity)
{
    return string(
        tinyrefl::tool::cppast_backend::IdentifierReader().name(entity));
}

const std::string& entity_names::full_name(const cppast::cpp_entity& entity)
{
    return string(
        tinyrefl::tool::cppast_backend::IdentifierReader().fullName(entity));
}

const std::string& entity_names::display_name(
    const cppast::cpp_entity&        entity,
    const display_name_includes_file display_name_includes_file)
{
    const auto mode =
        (display_name_includes_file == display_name_includes_file::YES
             ? tinyrefl::tool::cppast_backend::IdentifierReader::
                   DisplayNameMode::WithFile
             : tinyrefl::tool::cppast_backend::IdentifierReader::
                   DisplayNameMode::WithoutFile);

    return string(
        tinyrefl::tool::cppast_backend::IdentifierReader().displayName(
            entity, mode));
}

const std::string& entity_names::full_display_name(
    const cppast::cpp_entity&        entity,
    const display_name_includes_file display_name_includes_file)
{
    const auto mode =
        (display_name_includes_file == display_name_includes_file::YES
             ? tinyrefl::tool::cppast_backend::IdentifierReader::
                   DisplayNameMode::WithFile
             : tinyrefl::tool::cppast_backend::IdentifierReader::
                   DisplayNameMode::WithoutFile);

    return string(
        tinyrefl::tool::cppast_backend::IdentifierReader().fullDisplayName(
            entity, mode));
}

const std::string&
    entity_names::display_name(const cppast::cpp_namespace& namespace_)
{
    return string(
        tinyrefl::tool::cppast_backend::IdentifierReader().displayName(
            namespace_));
}

const std::string&
    entity_names::full_display_name(const cppast::cpp_namespace& namespace_)
{
    return string(
        tinyrefl::tool::cppast_backend::IdentifierReader().fullDisplayName(
            namespace_));
}
