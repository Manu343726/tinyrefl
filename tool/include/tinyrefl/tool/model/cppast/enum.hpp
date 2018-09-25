#ifndef TINYREFL_TOOL_MODEL_AST_ENUM_HPP
#define TINYREFL_TOOL_MODEL_AST_ENUM_HPP

#include <cppast/cpp_enum.hpp>
#include <cstddef>
#include <string>
#include <tinyrefl/tool/model/cppast/common.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{
entity_refs<cppast::cpp_enum_value> enum_values(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_);
std::int64_t enum_raw_value(const cppast::cpp_enum_value& value);
std::string enum_value_name(const cppast::cpp_enum_value& value);
std::string enum_value_qualified_name(const cppast::cpp_enum_value& value);
std::string enum_value_full_qualified_name(const cppast::cpp_enum_value& value);
std::vector<std::string> enum_names(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_);
std::vector<std::string> enum_qualified_names(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_);
std::vector<std::string> enum_full_qualified_names(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_);
std::size_t enum_count(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_);
std::vector<std::int64_t> enum_raw_values(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_);
}
}
}

#endif // TINYREFL_TOOL_MODEL_AST_ENUM_HPP
