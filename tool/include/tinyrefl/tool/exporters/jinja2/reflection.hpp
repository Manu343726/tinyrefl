#ifndef TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_HPP_INCLUDED
#define TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_HPP_INCLUDED

#include <google/protobuf/repeated_field.h>
#include <jinja2cpp/reflected_value.h>
#include <model/ast.pb.h>

namespace jinja2
{

#define TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(...)          \
                                                                               \
    template<>                                                                 \
    struct TypeReflection<__VA_ARGS__> : public TypeReflected<__VA_ARGS__>     \
    {                                                                          \
        static std::unordered_map<std::string, FieldAccessor>& GetAccessors(); \
    }

TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::SourceLocation);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::SourceRange);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Identifier);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Declaration);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Type);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Attribute);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Parameter);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Function);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::MemberFunction);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Constructor);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Variable);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::MemberVariable);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::EnumValue);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Enum);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::ClassMember);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Class);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::NamespaceMember);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::Namespace);
TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION(
    tinyrefl::tool::model::File);

#undef TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_TYPEREFLECTION

namespace detail
{

template<typename T>
struct Reflector<google::protobuf::RepeatedPtrField<T>>
{
    static auto Create(google::protobuf::RepeatedPtrField<T> val)
    {
        return ContainerReflector::CreateFromValue(std::move(val));
    }
    template<typename U>
    static auto CreateFromPtr(U&& val)
    {
        return ContainerReflector::CreateFromPtr(std::forward<U>(val));
    }
};

} // namespace detail

} // namespace jinja2

#endif // TINYREFL_TOOL_EXPORTERS_JINJA2_REFLECTION_HPP_INCLUDED
