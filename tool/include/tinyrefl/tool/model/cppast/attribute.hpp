#ifndef TINYREFL_TOOL_MODEL_CPPAST_ATTRIBUTE_HPP
#define TINYREFL_TOOL_MODEL_CPPAST_ATTRIBUTE_HPP

#include <cppast/cpp_attribute.hpp>
#include <cppast/cpp_entity.hpp>
#include <tinyrefl/tool/model/cppast/common.hpp>
#include <tinyrefl/tool/model/cppast/token.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{
entity_refs<cppast::cpp_attribute> entity_attributes(
    const cppast::cpp_entity_index& index, const cppast::cpp_entity& entity);
std::string full_attribute(const cppast::cpp_attribute& attribute);
std::string attribute_name(const cppast::cpp_attribute& attribute);
entity_refs<cppast::cpp_token> tokenized_attribute_arguments(
    const cppast::cpp_entity_index& index,
    const cppast::cpp_attribute&    attribute);
bool        attribute_has_arguments(const cppast::cpp_attribute& attribute);
std::string attribute_joined_arguments(const cppast::cpp_attribute& attribute);
} // namespace model
} // namespace tool
} // namespace tinyrefl

#endif // TINYREFL_TOOL_MODEL_CPPAST_ATTRIBUTE_HPP
