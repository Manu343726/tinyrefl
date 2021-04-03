#ifndef TINYREFL_TOOL_CONTAINER_ENTITY_CONTENTS_HPP_INCLUDED
#define TINYREFL_TOOL_CONTAINER_ENTITY_CONTENTS_HPP_INCLUDED

#include <string>
#include <vector>

namespace tinyrefl
{

namespace tool
{

struct enum_contents
{
    std::vector<std::string> serialized_values;
};

struct class_contents
{
    std::vector<std::string> serialized_constructors;
    std::vector<std::string> serialized_member_functions;
    std::vector<std::string> serialized_member_variables;
    std::vector<std::string> serialized_static_member_functions;
    std::vector<std::string> serialized_static_member_variables;
    std::vector<std::string> serialized_member_classes;
    std::vector<std::string> serialized_member_enums;
};

struct namespace_contents
{
    std::vector<std::string> serialized_functions;
    std::vector<std::string> serialized_variables;
    std::vector<std::string> serialized_classes;
    std::vector<std::string> serialized_enums;
    std::vector<std::string> serialized_namespaces;
};

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_CONTAINER_ENTITY_CONTENTS_HPP_INCLUDED
