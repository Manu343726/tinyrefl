#ifndef TINYREFL_TOOL_ENTITY_SERIALIZER_HPP_INCLUDED
#define TINYREFL_TOOL_ENTITY_SERIALIZER_HPP_INCLUDED

#include <tinyrefl/tool/attribute_registry.hpp>
#include <tinyrefl/tool/container_entity_contents.hpp>
#include <tinyrefl/tool/entity_names.hpp>
#include <tinyrefl/tool/entity_registry.hpp>
#include <tinyrefl/tool/serializer.hpp>
#include <tinyrefl/tool/serializers/sequence_serializer.hpp>
#include <tinyrefl/tool/serializers/type_serializer.hpp>
#include <tinyrefl/tool/serializers/value_serializer.hpp>

#include <cppast/cpp_class.hpp>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_variable.hpp>

namespace tinyrefl
{

namespace tool
{

class entity_serializer : public tinyrefl::tool::serializer
{
public:
    entity_serializer(
        tinyrefl::tool::string_registry&    string_registry,
        tinyrefl::tool::entity_registry&    entity_registry,
        tinyrefl::tool::attribute_registry& attribute_registry,
        cppast::cpp_entity_index&           index);

    const std::string& serialize(const cppast::cpp_enum_value& value);
    const std::string& serialize(
        const cppast::cpp_enum&        enum_,
        tinyrefl::tool::enum_contents* enum_contents_out = nullptr);
    const std::string& serialize(const cppast::cpp_function& function);
    const std::string& serialize(const cppast::cpp_variable& variable);
    const std::string& serialize(const cppast::cpp_constructor& constructor);
    const std::string& serialize(const cppast::cpp_member_function& function);
    const std::string& serialize(const cppast::cpp_member_variable& variable);
    const std::string& serialize(
        const cppast::cpp_class&        class_,
        tinyrefl::tool::class_contents* class_contents_out = nullptr);
    const std::string& serialize(
        const cppast::cpp_namespace&        namespace_,
        tinyrefl::tool::namespace_contents* namespace_contents_out = nullptr);
    const std::string& serialize(const cppast::cpp_file& file);
    std::string        serialize(const cppast::cpp_attribute& attribute);
    std::string        serialize(const cppast::cpp_attribute_list& attributes);

private:
    tinyrefl::tool::sequence_serializer _sequence_serializer;
    tinyrefl::tool::type_serializer     _type_serializer;
    tinyrefl::tool::value_serializer    _value_serializer;
    tinyrefl::tool::entity_names        _entity_names;
    tinyrefl::tool::entity_registry&    _entity_registry;
    tinyrefl::tool::attribute_registry& _attribute_registry;

    std::string
                function_argument_names(const cppast::cpp_function_base& function);
    std::string function_signature(const cppast::cpp_function_base& function);
    std::string parent_id(const cppast::cpp_entity& entity);
    std::string source_location(const cppast::cpp_entity& entity);
};

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_ENTITY_SERIALIZER_HPP_INCLUDED
