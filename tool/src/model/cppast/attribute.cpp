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
        return attribute.name() + attribute.arguments().value().as_string();
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

std::vector<std::string>
    tokenized_attribute_arguments(const cppast::cpp_attribute& attribute)
{
    std::vector<std::string> result;

    if(attribute.arguments())
    {
        const auto& arguments = attribute.arguments().value();

        DEBUG_ASSERT(
            arguments.front().spelling == "(" &&
                arguments.back().spelling == ")",
            tinyrefl::tool::detail::assert_handler{});

        const auto begin = ++arguments.begin();
        const auto end   = --arguments.end();

        for(auto it = begin; it != end; ++it)
        {
            result.push_back(it->spelling);
        }
    }

    return result;
}

std::string attribute_joined_arguments(const cppast::cpp_attribute& attribute)
{
    if(attribute.arguments())
    {
        std::string joined_arguments =
            attribute.arguments().value().as_string();

        DEBUG_ASSERT(
            (joined_arguments.size() >= 2) &&
                (joined_arguments.front() == '(') &&
                (joined_arguments.back() == ')'),
            tinyrefl::tool::detail::assert_handler{});

        return joined_arguments.substr(1, joined_arguments.size() - 1);
    }
    else
    {
        return "";
    }
}
}
}
}
