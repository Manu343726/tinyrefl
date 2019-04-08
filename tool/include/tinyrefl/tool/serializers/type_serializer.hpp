#ifndef TINYREFL_TOOL_SERIALIZERS_TYPE_SERIALIZER_HPP_INCLUDED
#define TINYREFL_TOOL_SERIALIZERS_TYPE_SERIALIZER_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_index.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_variable.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <tinyrefl/tool/entity_names.hpp>
#include <tinyrefl/tool/serializer.hpp>
#include <tinyrefl/tool/serializers/sequence_serializer.hpp>

#include <type_traits>

namespace tinyrefl
{

namespace tool
{

class type_serializer : public tinyrefl::tool::serializer
{
public:
    type_serializer(
        tinyrefl::tool::string_registry& string_registry,
        cppast::cpp_entity_index&        index);

    using type_decorator = std::function<std::string(const std::string&)>;
    static const type_decorator default_type_decorator;

    static std::string add_const(const std::string& type);
    static std::string add_pointer(const std::string& type);
    static std::string add_reference(const std::string& type);
    static std::string add_const_pointer(const std::string& type);
    static std::string add_const_reference(const std::string& type);

    std::string type(
        const cppast::cpp_type& type,
        const type_decorator&   decorator = default_type_decorator);
    std::string type(
        const cppast::cpp_entity& entity,
        const type_decorator&     decorator = default_type_decorator);
    std::string type(
        const cppast::cpp_function& function,
        const type_decorator&       decorator = default_type_decorator);
    std::string type(
        const cppast::cpp_variable& variable,
        const type_decorator&       decorator = default_type_decorator);
    std::string type(
        const cppast::cpp_member_function& function,
        const type_decorator&              decorator = default_type_decorator);
    std::string type(
        const cppast::cpp_member_variable& variable,
        const type_decorator&              decorator = default_type_decorator);
    std::string type(
        const cppast::cpp_enum_value& value,
        const type_decorator&         decorator = default_type_decorator);
    std::string type(
        const cppast::cpp_function_parameter& parameter,
        const type_decorator& decorator = default_type_decorator);

    std::string full_type(
        const cppast::cpp_type& type,
        const type_decorator&   decorator = default_type_decorator);
    std::string full_type(
        const cppast::cpp_entity& entity,
        const type_decorator&     decorator = default_type_decorator);
    std::string full_type(
        const cppast::cpp_function& function,
        const type_decorator&       decorator = default_type_decorator);
    std::string full_type(
        const cppast::cpp_variable& variable,
        const type_decorator&       decorator = default_type_decorator);
    std::string full_type(
        const cppast::cpp_member_function& function,
        const type_decorator&              decorator = default_type_decorator);
    std::string full_type(
        const cppast::cpp_member_variable& variable,
        const type_decorator&              decorator = default_type_decorator);
    std::string full_type(
        const cppast::cpp_enum_value& value,
        const type_decorator&         decorator = default_type_decorator);
    std::string full_type(
        const cppast::cpp_function_parameter& parameter,
        const type_decorator& decorator = default_type_decorator);

    template<typename Entity>
    std::string type_reference(
        const Entity&         entity,
        const type_decorator& decorator = default_type_decorator)
    {
        return fmt::format(
            "TINYREFL_TYPE(({}), ({}))",
            type(entity, decorator),
            full_type(entity, decorator));
    }

private:
    tinyrefl::tool::entity_names        _entity_names;
    tinyrefl::tool::sequence_serializer _sequence_serializer;
    cppast::cpp_entity_index&           _index;

    std::string function_signature(const cppast::cpp_function_base& entity);
};

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_SERIALIZERS_TYPE_SERIALIZER_HPP_INCLUDED
