#ifndef TINYREFL_TOOL_MODEL_CPPAST_CLASS_HPP
#define TINYREFL_TOOL_MODEL_CPPAST_CLASS_HPP

#include <cppast/cpp_class.hpp>
#include <cppast/cpp_entity_index.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <tinyrefl/tool/model/cppast/common.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

entity_refs<cppast::cpp_class> base_classes(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_);
entity_refs<cppast::cpp_member_variable> member_variables(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_);
entity_refs<cppast::cpp_member_function> member_functions(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_);
entity_refs<cppast::cpp_constructor> constructors(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_);
entity_refs<cppast::cpp_class> member_classes(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_);
entity_refs<cppast::cpp_enum> member_enums(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_);

std::string type_string(const cppast::cpp_member_function& function);
std::string type_string(const cppast::cpp_member_variable& variable);
}
}
}

#endif // TINYREFL_TOOL_MODEL_CPPAST_CLASS_HPP
