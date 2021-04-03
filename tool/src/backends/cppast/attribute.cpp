#include <algorithm>
#include <cstdint>
#include <fmt/format.h>
#include <iterator>
#include <tinyrefl/tool/backends/cppast/attribute.hpp>
#include <tinyrefl/tool/unique_id.hpp>

using namespace tinyrefl::tool::cppast_backend;

std::string AttributeReader::name(const cppast::cpp_attribute& attribute) const
{
    return attribute.name();
}

std::string
    AttributeReader::fullName(const cppast::cpp_attribute& attribute) const
{
    if(not attribute.scope().has_value())
    {
        return name(attribute);
    }

    return fmt::format("{}::{}", attribute.scope().value(), name(attribute));
}

std::string
    AttributeReader::fullAttribute(const cppast::cpp_attribute& attribute) const
{
    if(not attribute.arguments().has_value())
    {
        return fullName(attribute);
    }

    return fmt::format(
        "{}({})", fullName(attribute), joinedArguments(attribute));
}

std::string
    AttributeReader::namespace_(const cppast::cpp_attribute& attribute) const
{
    return attribute.scope().value_or("");
}

std::vector<std::string>
    AttributeReader::arguments(const cppast::cpp_attribute& attribute) const
{
    std::vector<std::string> result;

    if(attribute.arguments().has_value())
    {
        std::transform(
            attribute.arguments().value().begin(),
            attribute.arguments().value().end(),
            std::back_inserter(result),
            [](const cppast::cpp_token& token) { return token.spelling; });
    }

    return result;
}

std::string AttributeReader::joinedArguments(
    const cppast::cpp_attribute& attribute) const
{
    if(not attribute.arguments().has_value())
    {
        return "";
    }

    return attribute.arguments().value().as_string();
}

std::uint64_t
    AttributeReader::uniqueId(const cppast::cpp_attribute& attribute) const
{
    return tinyrefl::tool::uniqueId(fullAttribute(attribute));
}
