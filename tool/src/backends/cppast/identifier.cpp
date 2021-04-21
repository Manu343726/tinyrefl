#include <cppast/cpp_class.hpp>
#include <cppfs/FilePath.h>
#include <fmt/format.h>
#include <tinyrefl/tool/backends/cppast/hierarchy.hpp>
#include <tinyrefl/tool/backends/cppast/identifier.hpp>
#include <tinyrefl/tool/backends/cppast/visitor.hpp>

using namespace tinyrefl::tool::cppast_backend;

std::string IdentifierReader::name(const cppast::cpp_entity& entity) const
{
    auto rename_command = cppast::has_attribute(entity, "tinyrefl::rename");

    if(not rename_command)
    {
        return entity.name();
    }

    const auto& arguments = rename_command.value().arguments();

    if(not arguments || arguments.value().empty())
    {
        reporter().error(entity)(
            "Expected argument for [[tinyrefl::rename(name)]] attribute");

        return "";
    }

    if(arguments.value().end() - arguments.value().begin() != 1)
    {
        reporter().error(entity)(
            "Expected one argument for [[tinyrefl::rename(name)]] attribute");

        return "";
    }

    if(arguments.value().front().kind != cppast::cpp_token_kind::string_literal)
    {
        reporter().error(entity)(
            "Expected string literal as argument for [[tinyrefl::rename(name)]] attribute");

        return "";
    }

    return arguments.value().front().spelling;
}

std::string IdentifierReader::fullName(const cppast::cpp_entity& entity) const
{
    const std::string name = this->name(entity);

    if(entity.kind() == cppast::cpp_entity_kind::base_class_t)
    {
        return cppast::to_string(
            static_cast<const cppast::cpp_base_class&>(entity).type());
    }
    else if(
        entity.parent().has_value() &&
        entity.parent().value().kind() != cppast::cpp_entity_kind::file_t)
    {
        return fmt::format("{}::{}", fullName(entity.parent().value()), name);
    }
    else
    {
        return name;
    }
}

std::string IdentifierReader::displayName(
    const cppast::cpp_entity& entity, const DisplayNameMode mode) const
{
    std::string result;

    if(entity.kind() == cppast::cpp_entity_kind::member_function_t ||
       entity.kind() == cppast::cpp_entity_kind::function_t ||
       entity.kind() == cppast::cpp_entity_kind::constructor_t)
    {
        std::string signature;

        tinyrefl::tool::cppast_backend::entity_cast(
            entity, [&signature](const cppast::cpp_function_base& entity) {
                signature = entity.signature();
            });

        result = entity.name() + signature;
    }
    else if(entity.kind() == cppast::cpp_entity_kind::function_t)
    {
        result = entity.name() +
                 static_cast<const cppast::cpp_function&>(entity).signature();
    }
    else
    {
        result = entity.name();
    }

    if(mode == DisplayNameMode::WithFile)
    {
        return fmt::format("{}::{}", name((entity)), result);
    }
    else
    {
        return result;
    }
}

std::string IdentifierReader::fullDisplayName(
    const cppast::cpp_entity& entity, const DisplayNameMode mode) const
{
    std::string       result;
    const std::string name = displayName(entity);

    if(entity.kind() == cppast::cpp_entity_kind::base_class_t)
    {
        result = cppast::to_string(
            static_cast<const cppast::cpp_base_class&>(entity).type());
    }
    else if(
        entity.parent().has_value() &&
        entity.parent().value().kind() != cppast::cpp_entity_kind::file_t)
    {
        result = fmt::format("{}::{}", fullName(entity.parent().value()), name);
    }
    else
    {
        result = name;
    }

    if(mode == DisplayNameMode::WithFile)
    {
        return fmt::format(
            "{}::{}",
            fullName(HierarchyReader(reporter()).root(entity)),
            result);
    }
    else
    {
        return result;
    }
}

std::string
    IdentifierReader::displayName(const cppast::cpp_namespace& namespace_) const
{
    return displayName(
        static_cast<const cppast::cpp_entity&>(namespace_),
        DisplayNameMode::WithFile);
}

std::string IdentifierReader::fullDisplayName(
    const cppast::cpp_namespace& namespace_) const
{
    return fullDisplayName(
        static_cast<const cppast::cpp_entity&>(namespace_),
        DisplayNameMode::WithFile);
}

std::string IdentifierReader::name(const cppast::cpp_file& file) const
{
    return cppfs::FilePath(file.name()).fileName();
}

std::string IdentifierReader::fullName(const cppast::cpp_file& file) const
{
    return cppfs::FilePath(file.name()).fullPath();
}

std::string IdentifierReader::displayName(const cppast::cpp_file& file) const
{
    return name(file);
}

std::string
    IdentifierReader::fullDisplayName(const cppast::cpp_file& file) const
{
    return fullName(file);
}


std::uint64_t
    IdentifierReader::parentUniqueId(const cppast::cpp_entity& entity) const
{
    const auto parent = HierarchyReader(reporter()).parent(entity);

    if(not parent.has_value())
    {
        return 0;
    }

    if(parent.value().kind() == cppast::cpp_entity_kind::file_t)
    {
        return globalNamespaceUniqueId();
    }

    std::uint64_t result = 0;

    tinyrefl::tool::cppast_backend::entity_cast(
        parent.value(), [&](const auto& parent) { result = uniqueId(parent); });

    return result;
}

std::string IdentifierReader::globalNamespaceName() const
{
    return "";
}

std::string IdentifierReader::globalNamespaceFullName() const
{
    return "";
}

std::string IdentifierReader::globalNamespaceDisplayName() const
{
    return "";
}

std::string IdentifierReader::globalNamespaceFullDisplayName() const
{
    return "";
}

std::uint64_t IdentifierReader::globalNamespaceUniqueId() const
{
    return tinyrefl::tool::uniqueId("");
}
