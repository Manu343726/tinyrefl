#ifndef TINYREFL_TOOL_MODEL_CPPAST_FILE_HPP
#define TINYREFL_TOOL_MODEL_CPPAST_FILE_HPP

#include <cppast/cpp_class.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_file.hpp>
#include <tinyrefl/tool/model/cppast/common.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{
entity_refs<cppast::cpp_class> all_namespace_level_classes(
    const cppast::cpp_entity_index& index, const cppast::cpp_file& file);
entity_refs<cppast::cpp_enum> all_namespace_level_enums(
    const cppast::cpp_entity_index& index, const cppast::cpp_file& file);
entity_refs<cppast::cpp_class> all_classes(
    const cppast::cpp_entity_index& index, const cppast::cpp_file& file);
entity_refs<cppast::cpp_enum> all_enums(
    const cppast::cpp_entity_index& index, const cppast::cpp_file& file);
}
}
}

#endif // TINYREFL_TOOL_MODEL_CPPAST_FILE_HPP
