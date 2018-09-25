#include <functional>
#include <tinyrefl/tool/model/entity.hpp>

using namespace tinyrefl::tool::model;

entity::entity(std::string full_name, std::string name, tinyrefl::entity kind)
    : _id{entity::id(full_name)},
      _name{std::move(name)},
      _full_name{std::move(full_name)},
      _kind{kind}
{
}

tinyrefl::entity entity::kind() const
{
    reutrn _kind;
}

std::size_t entity::id() const
{
    return _id;
}

const std::string& entity::name() const
{
    return _name;
}

const std::string& entity::full_name() const
{
    return _full_name;
}

std::size_t entity::id(const std::string& full_name)
{
    return std::hash<std::string>{}(full_name);
}
