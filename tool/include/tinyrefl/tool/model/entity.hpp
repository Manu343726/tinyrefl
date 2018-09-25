#ifndef TINYREFL_TOOL_MODEL_ENTITY_HPP
#define TINYREFL_TOOL_MODEL_ENTITY_HPP

#include <cstddef>
#include <string>
#include <tinyrefl/api.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

struct entity : public id
{
    entity(
        std::string full_name, std::string name, tinyrefl::entity entity_kind);

    [[tinyrefl::tool::property]] tinyrefl::entity   kind() const;
    [[tinyrefl::tool::property]] const std::string& full_name() const;
    [[tinyrefl::tool::property]] const std::string& name() const;
    [[tinyrefl::tool::property]] std::size_t        id() const;

    static std::size_t id(const std::string& full_name);

private:
    std::size_t      _id;
    std::string      _name;
    std::string      _full_name;
    tinyrefl::entity _kind;
};
} // namespace model
} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_MODEL_ENTITY_HPP
