#ifndef TINYREFL_TOOL_MODEL_AST_JINJA2REFLECTION_HPP
#define TINYREFL_TOOL_MODEL_AST_JINJA2REFLECTION_HPP

#include <jinja2cpp/reflected_value.h>
#include <tinyrefl/tool/model/cppast/class.hpp>

namespace jinja2
{

template<>
struct TypeReflection<
    tinyrefl::tool::model::entity_ref<cppast::cpp_member_variable>>
    : public TypeReflected<
          tinyrefl::tool::model::entity_ref<cppast::cpp_member_variable>>
{
    static std::unordered_map<std::string, FieldAccessor> GetAccessors();
};

template<>
struct TypeReflection<
    tinyrefl::tool::model::entity_ref<cppast::cpp_member_function>>
    : public TypeReflected<
          tinyrefl::tool::model::entity_ref<cppast::cpp_member_function>>
{
    static std::unordered_map<std::string, FieldAccessor> GetAccessors();
};

template<>
struct TypeReflection<
    tinyrefl::tool::model::entity_ref<cppast::cpp_constructor>>
    : public TypeReflected<
          tinyrefl::tool::model::entity_ref<cppast::cpp_constructor>>
{
    static std::unordered_map<std::string, FieldAccessor> GetAccessors();
};

template<>
struct TypeReflection<tinyrefl::tool::model::entity_ref<cppast::cpp_class>>
    : public TypeReflected<tinyrefl::tool::model::entity_ref<cppast::cpp_class>>
{
    static std::unordered_map<std::string, FieldAccessor> GetAccessors();
};

template<>
struct TypeReflection<tinyrefl::tool::model::entity_ref<cppast::cpp_enum>>
    : public TypeReflected<tinyrefl::tool::model::entity_ref<cppast::cpp_enum>>
{
    static std::unordered_map<std::string, FieldAccessor> GetAccessors();
};
}

#endif // TINYREFL_TOOL_MODEL_AST_JINJA2REFLECTION_HPP
