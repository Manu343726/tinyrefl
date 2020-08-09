#include <cppast/cpp_class.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>

#include <cppfs/FilePath.h>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <stdexcept>

#include <tinyrefl/tool/entity_names.hpp>
#include <tinyrefl/tool/visitor.hpp>

using namespace tinyrefl::tool;

const std::string& entity_names::name(const cppast::cpp_file& file)
{
    return string(cppfs::FilePath{file.name()}.fileName());
}

const std::string& entity_names::full_name(const cppast::cpp_file& file)
{
    return string(cppfs::FilePath{file.name()}.fullPath());
}

const std::string& entity_names::name(const cppast::cpp_entity& entity)
{
    auto rename_command = cppast::has_attribute(entity, "tinyrefl::rename");

    if(rename_command)
    {
        const auto arguments = rename_command.value().arguments();

        if(!arguments || arguments.value().empty())
        {
            throw std::runtime_error{fmt::format(
                "[error] expected argument for [[tinyrefl::rename(name)]] attribute")};
        }
        else if(arguments.value().end() - arguments.value().begin() != 1)
        {
            throw std::runtime_error{fmt::format(
                "[error] expected one argument for [[tinyrefl::rename(name)]] attribute")};
        }
        else if(
            arguments.value().front().kind !=
            cppast::cpp_token_kind::string_literal)
        {
            throw std::runtime_error{fmt::format(
                "[error] expected string literal as argument for [[tinyrefl::rename(name)]] attribute")};
        }
        else
        {
            return string(arguments.value().front().spelling);
        }
    }
    else
    {
        return string(entity.name());
    }
}

const std::string& entity_names::full_name(const cppast::cpp_entity& entity)
{
    std::string name = this->name(entity);

    if(entity.kind() == cppast::cpp_entity_kind::base_class_t)
    {
        return string(cppast::to_string(
            static_cast<const cppast::cpp_base_class&>(entity).type()));
    }
    else if(
        entity.parent().has_value() &&
        entity.parent().value().kind() != cppast::cpp_entity_kind::file_t)
    {
        return string(
            fmt::format("{}::{}", full_name(entity.parent().value()), name));
    }
    else
    {
        return string(fmt::format("{}", name));
    }
}

const std::string& entity_names::display_name(
    const cppast::cpp_entity&        entity,
    const display_name_includes_file display_name_includes_file)
{
    std::string result;

    if(entity.kind() == cppast::cpp_entity_kind::member_function_t ||
       entity.kind() == cppast::cpp_entity_kind::function_t ||
       entity.kind() == cppast::cpp_entity_kind::constructor_t)
    {
        std::string signature;

        tinyrefl::tool::entity_cast(
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

    if(display_name_includes_file ==
       entity_names::display_name_includes_file::YES)
    {
        return string(fmt::format("{}::{}", name(entity_root(entity)), result));
    }
    else
    {
        return string(result);
    }
}

const std::string& entity_names::full_display_name(
    const cppast::cpp_entity&        entity,
    const display_name_includes_file display_name_includes_file)
{
    std::string result;
    std::string name = display_name(entity);

    if(entity.kind() == cppast::cpp_entity_kind::base_class_t)
    {
        result = cppast::to_string(
            static_cast<const cppast::cpp_base_class&>(entity).type());
    }
    else if(
        entity.parent().has_value() &&
        entity.parent().value().kind() != cppast::cpp_entity_kind::file_t)
    {
        result =
            fmt::format("{}::{}", full_name(entity.parent().value()), name);
    }
    else
    {
        result = name;
    }

    if(display_name_includes_file ==
       entity_names::display_name_includes_file::YES)
    {
        return string(
            fmt::format("{}::{}", full_name(entity_root(entity)), result));
    }
    else
    {
        return string(result);
    }
}

const std::string&
    entity_names::display_name(const cppast::cpp_namespace& namespace_)
{
    return display_name(
        static_cast<const cppast::cpp_entity&>(namespace_),
        display_name_includes_file::YES);
}

const std::string&
    entity_names::full_display_name(const cppast::cpp_namespace& namespace_)
{
    return full_display_name(
        static_cast<const cppast::cpp_entity&>(namespace_),
        display_name_includes_file::YES);
}
