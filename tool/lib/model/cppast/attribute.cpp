#include <tinyrefl/tool/model/cppast/attribute.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{
entity_refs<cppast::cpp_attribute> entity_attributes(
    const cppast::cpp_entity_index& index, const cppast::cpp_entity& entity)
{
    entity_refs<cppast::cpp_attribute> result;

    for(const auto& attribute : entity.attributes())
    {
        result.emplace_back(index, attribute);
    }

    return result;
}

std::string full_attribute(const cppast::cpp_attribute& attribute)
{
    if(attribute.arguments())
    {
        return attribute.name() + "(" +
               attribute.arguments().value().as_string() + ")";
    }
    else
    {
        return attribute.name();
    }
}

std::string attribute_name(const cppast::cpp_attribute& attribute)
{
    return attribute.name();
}

bool attribute_has_arguments(const cppast::cpp_attribute& attribute)
{
    return attribute.arguments().has_value();
}

entity_refs<cppast::cpp_token> tokenized_attribute_arguments(
    const cppast::cpp_entity_index& index,
    const cppast::cpp_attribute&    attribute)
{
    entity_refs<cppast::cpp_token> result;

    if(attribute_has_arguments(attribute))
    {
        const auto& arguments = attribute.arguments().value();

        for(const auto& token : arguments)
        {
            result.emplace_back(index, token);
        }
    }

    return result;
}

std::string attribute_joined_arguments(const cppast::cpp_attribute& attribute)
{
    if(attribute_has_arguments(attribute))
    {
        return attribute.arguments().value().as_string();
    }
    else
    {
        return "";
    }
}
} // namespace model
} // namespace tool
} // namespace tinyrefl
