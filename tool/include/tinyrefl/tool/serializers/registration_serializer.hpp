#ifndef TINYREFL_TOOL_SERIALIZERS_REGISTRATION_SERIALIZER_HPP_INCLUDED
#define TINYREFL_TOOL_SERIALIZERS_REGISTRATION_SERIALIZER_HPP_INCLUDED

#include <tinyrefl/tool/serializer.hpp>
#include <tinyrefl/tool/serializers/entity_serializer.hpp>

#include <cppast/cpp_class.hpp>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_variable.hpp>

namespace tinyrefl
{
namespace tool
{
class registration_serializer : public tinyrefl::tool::serializer
{
public:
    registration_serializer(
        tinyrefl::tool::string_registry&    string_registry,
        tinyrefl::tool::entity_registry&    entity_registry,
        tinyrefl::tool::attribute_registry& attribute_registry,
        cppast::cpp_entity_index&           index);

    std::string serialize_registration(const cppast::cpp_file& file);
    std::string serialize_registration(
        const cppast::cpp_namespace&        namespace_,
        tinyrefl::tool::namespace_contents* namespace_contents = nullptr);
    std::string serialize_registration(const cppast::cpp_function& function);
    std::string serialize_registration(const cppast::cpp_variable& variable);
    std::string serialize_registration(
        const cppast::cpp_class&        class_,
        tinyrefl::tool::class_contents* class_contents = nullptr);
    std::string
        serialize_registration(const cppast::cpp_member_function& function);
    std::string
        serialize_registration(const cppast::cpp_member_variable& variable);
    std::string
                serialize_registration(const cppast::cpp_constructor& constructor);
    std::string serialize_registration(
        const cppast::cpp_enum&        enum_,
        tinyrefl::tool::enum_contents* enum_contents = nullptr);
    std::string serialize_registration(const cppast::cpp_enum_value& value);
    std::string serialize_registration(const cppast::cpp_entity& entity);
    std::string serialize_registration(const std::string& string_constant);

private:
    tinyrefl::tool::entity_serializer _entity_serializer;

    std::string serialize_registration(
        const char* registration_function, std::string serialzied_entity);
};
} // namespace tool
} // namespace tinyrefl

#endif // TINYREFL_TOOL_SERIALIZERS_REGISTRATION_SERIALIZER_HPP_INCLUDED
