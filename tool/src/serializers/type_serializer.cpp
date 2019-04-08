#include <tinyrefl/tool/serializers/type_serializer.hpp>
#include <tinyrefl/tool/type_visitor.hpp>

using namespace tinyrefl::tool;

const type_serializer::type_decorator type_serializer::default_type_decorator =
    [](const std::string& type) { return type; };

std::string type_serializer::function_signature(
    const cppast::cpp_function_base& function)
{
    return fmt::format(
        "({})",
        _sequence_serializer.raw_sequence(
            function.parameters(),
            [this](const cppast::cpp_function_parameter& parameter) {
                return full_type(parameter);
            }));
}

std::string type_serializer::add_const(const std::string& type)
{
    return fmt::format("const {}", type);
}

std::string type_serializer::add_pointer(const std::string& type)
{
    return fmt::format("{}*", type);
}

std::string type_serializer::add_reference(const std::string& type)
{
    return fmt::format("{}&", type);
}

std::string type_serializer::add_const_pointer(const std::string& type)
{
    return fmt::format("const {}*", type);
}

std::string type_serializer::add_const_reference(const std::string& type)
{
    return fmt::format("const {}&", type);
}

type_serializer::type_serializer(
    tinyrefl::tool::string_registry& string_registry,
    cppast::cpp_entity_index&        index)
    : tinyrefl::tool::serializer{string_registry},
      _entity_names{string_registry},
      _sequence_serializer{string_registry},
      _index(index)
{
}

const char* cv_qualifier(const cppast::cpp_cv qualifier)
{
    switch(qualifier)
    {
    case cppast::cpp_cv::cpp_cv_const:
        return "const";
    case cppast::cpp_cv::cpp_cv_const_volatile:
        return "const volatile";
    case cppast::cpp_cv::cpp_cv_volatile:
        return "volatile";
    case cppast::cpp_cv::cpp_cv_none:
        return "";
    }

    return "";
}

std::string type_serializer::type(
    const cppast::cpp_type& type, const type_decorator& decorator)
{
    std::string result;

    tinyrefl::tool::type_cast(
        type,
        [&](const cppast::cpp_reference_type& type) {
            result = fmt::format("{}&", this->type(type.referee()));
        },
        [&](const cppast::cpp_cv_qualified_type& type) {
            result = fmt::format(
                "{} {}",
                this->type(type.type()),
                cv_qualifier(type.cv_qualifier()));
        },
        [&](const cppast::cpp_user_defined_type& type) {
            const auto& referenced_entities = type.entity().get(_index);

            if(!referenced_entities.empty())
            {
                result = _entity_names.full_display_name(
                    referenced_entities[0].get());
            }
            else
            {
                result = type.entity().name();
            }
        },
        [&](const auto& type) { result = cppast::to_string(type); });

    return string(decorator(result));
}

std::string type_serializer::type(
    const cppast::cpp_entity& entity, const type_decorator& decorator)
{
    return string(decorator(entity.name()));
}

std::string type_serializer::type(
    const cppast::cpp_function& function, const type_decorator& decorator)
{
    return string(decorator(fmt::format(
        "{}(*){}",
        type(function.return_type()),
        function_signature(function))));
}

std::string type_serializer::type(
    const cppast::cpp_variable& variable, const type_decorator& decorator)
{
    return type(variable.type(), decorator);
}

std::string type_serializer::type(
    const cppast::cpp_member_function& function,
    const type_decorator&              decorator)
{
    const char* qualifier = nullptr;

    switch(function.cv_qualifier())
    {
    case cppast::cpp_cv::cpp_cv_const:
        qualifier = "const";
        break;
    case cppast::cpp_cv::cpp_cv_volatile:
        qualifier = "volatile";
        break;
    case cppast::cpp_cv::cpp_cv_const_volatile:
        qualifier = "const volatile";
        break;
    case cppast::cpp_cv::cpp_cv_none:
        qualifier = "";
        break;
    }

    return string(decorator(fmt::format(
        "{}({}::*){}{}",
        type(function.return_type()),
        _entity_names.full_name(function.parent().value()),
        function_signature(function),
        qualifier)));
}

std::string type_serializer::type(
    const cppast::cpp_member_variable& variable,
    const type_decorator&              decorator)
{
    return string(decorator(fmt::format(
        "{} {}::*",
        type(variable.type()),
        _entity_names.full_name(variable.parent().value()))));
}

std::string type_serializer::type(
    const cppast::cpp_function_parameter& parameter,
    const type_decorator&                 decorator)
{
    return type(parameter.type(), decorator);
}

std::string type_serializer::type(
    const cppast::cpp_enum_value& value, const type_decorator& decorator)
{
    return type(value.parent().value(), decorator);
}

std::string type_serializer::full_type(
    const cppast::cpp_type& type, const type_decorator& decorator)
{
    std::string result;

    tinyrefl::tool::type_cast(
        type,
        [&](const cppast::cpp_reference_type& type) {
            result = fmt::format("{}&", this->full_type(type.referee()));
        },
        [&](const cppast::cpp_cv_qualified_type& type) {
            result = fmt::format(
                "{} {}",
                this->full_type(type.type()),
                cv_qualifier(type.cv_qualifier()));
        },
        [&](const cppast::cpp_user_defined_type& type) {
            const auto& referenced_entities = type.entity().get(_index);

            if(!referenced_entities.empty())
            {
                result = _entity_names.full_display_name(
                    referenced_entities[0].get());
            }
            else
            {
                result = type.entity().name();
            }
        },
        [&](const auto& type) { result = cppast::to_string(type); });

    return string(decorator(result));
}

std::string type_serializer::full_type(
    const cppast::cpp_entity& entity, const type_decorator& decorator)
{
    return string(decorator(_entity_names.full_name(entity)));
}

std::string type_serializer::full_type(
    const cppast::cpp_function& function, const type_decorator& decorator)
{
    return type(function, decorator);
}

std::string type_serializer::full_type(
    const cppast::cpp_variable& variable, const type_decorator& decorator)
{
    return type(variable, decorator);
}

std::string type_serializer::full_type(
    const cppast::cpp_member_function& function,
    const type_decorator&              decorator)
{
    return type(function, decorator);
}

std::string type_serializer::full_type(
    const cppast::cpp_member_variable& variable,
    const type_decorator&              decorator)
{
    return type(variable, decorator);
}

std::string type_serializer::full_type(
    const cppast::cpp_enum_value& value, const type_decorator& decorator)
{
    return full_type(value.parent().value(), decorator);
}

std::string type_serializer::full_type(
    const cppast::cpp_function_parameter& parameter,
    const type_decorator&                 decorator)
{
    return full_type(parameter.type(), decorator);
}
