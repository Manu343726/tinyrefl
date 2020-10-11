#include <tinyrefl/tool/serializers/registration_serializer.hpp>
#include <tinyrefl/tool/visitor.hpp>
#include <tinyrefl/types/string.hpp>

using namespace tinyrefl::tool;

registration_serializer::registration_serializer(
    tinyrefl::tool::string_registry&    string_registry,
    tinyrefl::tool::entity_registry&    entity_registry,
    tinyrefl::tool::attribute_registry& attribute_registry,
    cppast::cpp_entity_index&           index)
    : tinyrefl::tool::serializer{string_registry},
      _entity_serializer{
          string_registry, entity_registry, attribute_registry, index}
{
}

std::string registration_serializer::serialize_registration(
    const char* registration_function, std::string serialized_entity)
{
    return fmt::format("{}({})", registration_function, serialized_entity);
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_file& file)
{
    return serialize_registration(
        "TINYREFL_REGISTER_FILE", _entity_serializer.serialize(file));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_namespace&        namespace_,
    tinyrefl::tool::namespace_contents* namespace_contents)
{
    return serialize_registration(
        "TINYREFL_REGISTER_NAMESPACE",
        _entity_serializer.serialize(namespace_, namespace_contents));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_function& function)
{
    return serialize_registration(
        (function.parent().value().kind() == cppast::cpp_entity_kind::class_t
             ? "TINYREFL_REGISTER_STATIC_MEMBER_FUNCTION"
             : "TINYREFL_REGISTER_FUNCTION"),
        _entity_serializer.serialize(function));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_variable& variable)
{
    return serialize_registration(
        (variable.parent().value().kind() == cppast::cpp_entity_kind::class_t
             ? "TINYREFL_REGISTER_STATIC_MEMBER_VARIABLE"
             : "TINYREFL_REGISTER_VARIABLE"),
        _entity_serializer.serialize(variable));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_class&        class_,
    tinyrefl::tool::class_contents* class_contents)
{
    return serialize_registration(
        "TINYREFL_REGISTER_CLASS",
        _entity_serializer.serialize(class_, class_contents));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_member_function& function)
{
    return serialize_registration(
        "TINYREFL_REGISTER_MEMBER_FUNCTION",
        _entity_serializer.serialize(function));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_member_variable& variable)
{
    return serialize_registration(
        "TINYREFL_REGISTER_MEMBER_VARIABLE",
        _entity_serializer.serialize(variable));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_constructor& constructor)
{
    return serialize_registration(
        "TINYREFL_REGISTER_CONSTRUCTOR",
        _entity_serializer.serialize(constructor));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_enum& enum_, tinyrefl::tool::enum_contents* enum_contents)
{
    return serialize_registration(
        "TINYREFL_REGISTER_ENUM",
        _entity_serializer.serialize(enum_, enum_contents));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_enum_value& value)
{
    return serialize_registration(
        "TINYREFL_REGISTER_ENUM_VALUE", _entity_serializer.serialize(value));
}

std::string registration_serializer::serialize_registration(
    const cppast::cpp_entity& entity)
{
    std::string result;

    tinyrefl::tool::entity_cast(entity, [&result, this](const auto& entity) {
        result = this->serialize_registration(entity);
    });

    return result;
}

std::string registration_serializer::serialize_registration(
    const std::string& string_constant)
{
    const auto hash = tinyrefl::string{string_constant.c_str()}.hash();

    return fmt::format(
        "#ifndef TINYREFL_STRING_{hash}_REGISTERED\n"
        "#define TINYREFL_STRING_{hash}_REGISTERED\n"
        "#define TINYREFL_REGISTER_STRING({string})\n"
        "#endif // TINYREFL_STRING_{hash}_REGISTERED",
        fmt::arg("hash", hash),
        fmt::arg("string", string_constant));
}
