#include <ostream>
#include <tinyrefl/tool/backends/cppast/type_visitor.hpp>
#include <tinyrefl/tool/backends/legacy/serializers/type_serializer.hpp>

namespace cppast
{
std::ostream& operator<<(std::ostream& os, const cpp_type_kind kind)
{
    switch(kind)
    {
    case cppast::cpp_type_kind::builtin_t:
        return os << "builtin";
    case cppast::cpp_type_kind::user_defined_t:
        return os << "user_defined";
    case cppast::cpp_type_kind::auto_t:
        return os << "auto";
    case cppast::cpp_type_kind::decltype_t:
        return os << "decltype";
    case cppast::cpp_type_kind::decltype_auto_t:
        return os << "decltype_auto";
    case cppast::cpp_type_kind::cv_qualified_t:
        return os << "cv_qualified";
    case cppast::cpp_type_kind::pointer_t:
        return os << "pointer";
    case cppast::cpp_type_kind::reference_t:
        return os << "reference";
    case cppast::cpp_type_kind::array_t:
        return os << "array";
    case cppast::cpp_type_kind::function_t:
        return os << "function";
    case cppast::cpp_type_kind::member_function_t:
        return os << "member_function";
    case cppast::cpp_type_kind::member_object_t:
        return os << "member_object";
    case cppast::cpp_type_kind::template_parameter_t:
        return os << "template_parameter";
    case cppast::cpp_type_kind::template_instantiation_t:
        return os << "template_instantiation";
    case cppast::cpp_type_kind::dependent_t:
        return os << "dependent";
    case cppast::cpp_type_kind::unexposed_t:
        return os << "unexposed";
    }

    return os;
}
} // namespace cppast

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

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

    tinyrefl::tool::cppast_backend::type_cast(
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

            spdlog::trace(
                "type_serializer::type(user defined type \"{}\")",
                type.entity().name());

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
        [&](const auto& type) {
            result = cppast::to_string(type);

            if(result.empty())
            {
                spdlog::warn(
                    "type_serializer::type(type (kind: {})): Result string empty",
                    type.kind());
            }
            else
            {
                spdlog::debug("type \"{}\"", cppast::to_string(type));
            }
        });

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

    const auto return_type = type(function.return_type());

    if(return_type.empty())
    {
        spdlog::error(
            "type_serializer::type(member function \"{}\"): Return type empty",
            _entity_names.full_display_name(function));
    }

    return string(decorator(fmt::format(
        "{}({}::*){}{}",
        return_type,
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

    tinyrefl::tool::cppast_backend::type_cast(
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
