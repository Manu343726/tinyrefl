#include <cppast/visitor.hpp>
#include <fmt/format.h>
#include <sstream>
#include <tinyrefl/tool/serializers/entity_serializer.hpp>
#include <tinyrefl/tool/visitor.hpp>

using namespace tinyrefl::tool;

entity_serializer::entity_serializer(
    tinyrefl::tool::string_registry& string_registry,
    tinyrefl::tool::entity_registry& entity_registry,
    cppast::cpp_entity_index&        index)
    : tinyrefl::tool::serializer{string_registry},
      _sequence_serializer{string_registry},
      _type_serializer{string_registry, index},
      _value_serializer{string_registry, index},
      _entity_names{string_registry},
      _entity_registry(entity_registry)
{
}

std::string entity_serializer::source_location(const cppast::cpp_entity& entity)
{
    return fmt::format(
        "TINYREFL_SOURCE_LOCATION(({}), ({}), ({}))",
        string_constant(_entity_names.full_name(entity_root(entity))),
        0,
        0);
}

std::string entity_serializer::function_argument_names(
    const cppast::cpp_function_base& function)
{
    return _sequence_serializer.sequence(
        function.parameters(),
        [this](const cppast::cpp_function_parameter& parameter) {
            return string_constant(parameter.name());
        });
}

std::string entity_serializer::function_signature(
    const cppast::cpp_function_base& function)
{
    return _sequence_serializer.sequence(
        function.parameters(),
        [this](const cppast::cpp_function_parameter& parameter) {
            return _type_serializer.full_type(parameter.type());
        });
}

std::string entity_serializer::parent_id(const cppast::cpp_entity& entity)
{
    switch(entity.parent().value().kind())
    {
    case cppast::cpp_entity_kind::class_t:
    case cppast::cpp_entity_kind::enum_t:
        return _type_serializer.type_reference(entity.parent().value());
    default:
        std::string parent_full_display_name;

        tinyrefl::tool::entity_cast(
            entity.parent().value(), [&](const auto& parent) {
                parent_full_display_name =
                    _entity_names.full_display_name(parent);
            });

        return string_constant(parent_full_display_name);
    }
}

const std::string&
    entity_serializer::serialize(const cppast::cpp_enum_value& value)
{
    if(_entity_registry.already_registered(value))
    {
        return _entity_registry[value];
    }
    else
    {
        return _entity_registry(
            value,
            fmt::format(
                "TINYREFL_ENUM_VALUE(({}), ({}), ({}), ({}), ({}), ({}), ({}))",
                string_constant(_entity_names.name(value)),
                string_constant(_entity_names.full_name(value)),
                parent_id(value),
                source_location(value),
                _type_serializer.type_reference(value.parent().value()),
                _value_serializer.value(value),
                serialize(value.attributes())));
    }
}

const std::string& entity_serializer::serialize(
    const cppast::cpp_enum&        enum_,
    tinyrefl::tool::enum_contents* enum_contents_out)
{
    auto body = [this, &enum_](tinyrefl::tool::enum_contents& enum_contents)
        -> const std::string& {
        tinyrefl::tool::visit_entity(
            enum_, [this, &enum_contents](const cppast::cpp_enum_value& value) {
                enum_contents.serialized_values.push_back(serialize(value));
            });

        if(_entity_registry.already_registered(enum_))
        {
            return _entity_registry[enum_];
        }
        else
        {
            return _entity_registry(
                enum_,
                fmt::format(
                    "TINYREFL_ENUM(({}), ({}), ({}), ({}), ({}), ({}), ({}))",
                    string_constant(_entity_names.name(enum_)),
                    string_constant(_entity_names.full_name(enum_)),
                    parent_id(enum_),
                    source_location(enum_),
                    _type_serializer.full_type(enum_),
                    _sequence_serializer.sequence(
                        enum_contents.serialized_values),
                    serialize(enum_.attributes())));
        }
    };

    if(enum_contents_out != nullptr)
    {
        return body(*enum_contents_out);
    }
    else
    {
        tinyrefl::tool::enum_contents contents;
        return body(contents);
    }
}

const std::string&
    entity_serializer::serialize(const cppast::cpp_function& function)
{
    if(_entity_registry.already_registered(function))
    {
        return _entity_registry[function];
    }
    else
    {
        return _entity_registry(
            function,
            fmt::format(
                "{}(({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}))",
                ((function.parent().value().kind() ==
                  cppast::cpp_entity_kind::class_t)
                     ? "TINYREFL_STATIC_MEMBER_FUNCTION"
                     : "TINYREFL_FUNCTION"),
                string_constant(_entity_names.name(function)),
                string_constant(_entity_names.full_name(function)),
                parent_id(function),
                source_location(function),
                string_constant(_entity_names.display_name(function)),
                string_constant(_entity_names.full_display_name(function)),
                _type_serializer.type_reference(function.return_type()),
                function_signature(function),
                function_argument_names(function),
                _value_serializer.pointer(function),
                serialize(function.attributes())));
    }
}

const std::string&
    entity_serializer::serialize(const cppast::cpp_variable& variable)
{
    if(_entity_registry.already_registered(variable))
    {
        return _entity_registry[variable];
    }
    else
    {
        return _entity_registry(
            variable,
            fmt::format(
                "{}(({}), ({}), ({}), ({}), ({}), ({}), ({}))",
                ((variable.parent().value().kind() ==
                  cppast::cpp_entity_kind::class_t)
                     ? "TINYREFL_STATIC_MEMBER_VARIABLE"
                     : "TINYREFL_VARIABLE"),
                string_constant(_entity_names.name(variable)),
                string_constant(_entity_names.full_name(variable)),
                parent_id(variable),
                source_location(variable),
                _type_serializer.type_reference(variable.type()),
                _value_serializer.pointer(variable),
                serialize(variable.attributes())));
    }
}

const std::string&
    entity_serializer::serialize(const cppast::cpp_constructor& constructor)
{
    if(_entity_registry.already_registered(constructor))
    {
        return _entity_registry[constructor];
    }
    else
    {
        return _entity_registry(
            constructor,
            fmt::format(
                "TINYREFL_CONSTRUCTOR(({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}))",
                string_constant(_entity_names.name(constructor)),
                string_constant(_entity_names.full_name(constructor)),
                parent_id(constructor),
                source_location(constructor),
                string_constant(_entity_names.display_name(constructor)),
                string_constant(_entity_names.full_display_name(constructor)),
                _type_serializer.type_reference(constructor.parent().value()),
                function_signature(constructor),
                function_argument_names(constructor),
                serialize(constructor.attributes())));
    }
}

const std::string&
    entity_serializer::serialize(const cppast::cpp_member_function& function)
{
    if(_entity_registry.already_registered(function))
    {
        return _entity_registry[function];
    }
    else
    {
        return _entity_registry(
            function,
            fmt::format(
                "TINYREFL_MEMBER_FUNCTION(({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}))",
                string_constant(_entity_names.name(function)),
                string_constant(_entity_names.full_name(function)),
                parent_id(function),
                source_location(function),
                string_constant(_entity_names.display_name(function)),
                string_constant(_entity_names.full_display_name(function)),
                _type_serializer.type_reference(function.return_type()),
                function_signature(function),
                function_argument_names(function),
                _value_serializer.pointer(function),
                serialize(function.attributes())));
    }
}

const std::string&
    entity_serializer::serialize(const cppast::cpp_member_variable& variable)
{
    if(_entity_registry.already_registered(variable))
    {
        return _entity_registry[variable];
    }
    else
    {
        return _entity_registry(
            variable,
            fmt::format(
                "TINYREFL_MEMBER_VARIABLE(({}), ({}), ({}), ({}), ({}), ({}), ({}))",
                string_constant(_entity_names.name(variable)),
                string_constant(_entity_names.full_name(variable)),
                parent_id(variable),
                source_location(variable),
                _type_serializer.type_reference(variable.type()),
                _value_serializer.pointer(variable),
                serialize(variable.attributes())));
    }
}

const std::string& entity_serializer::serialize(
    const cppast::cpp_class&        class_,
    tinyrefl::tool::class_contents* class_contents_out)
{
    auto body = [this, &class_](tinyrefl::tool::class_contents& class_contents)
        -> const std::string& {
        if(_entity_registry.already_registered(class_))
        {
            return _entity_registry[class_];
        }
        else
        {
            tinyrefl::tool::visit_entity(
                class_,
                [this,
                 &class_contents](const cppast::cpp_constructor& constructor) {
                    class_contents.serialized_constructors.push_back(
                        serialize(constructor));
                },
                [this,
                 &class_contents](const cppast::cpp_member_function& function) {
                    class_contents.serialized_member_functions.push_back(
                        serialize(function));
                },
                [this,
                 &class_contents](const cppast::cpp_member_variable& variable) {
                    class_contents.serialized_member_variables.push_back(
                        serialize(variable));
                },
                [this, &class_contents](const cppast::cpp_function& function) {
                    class_contents.serialized_static_member_functions.push_back(
                        serialize(function));
                },
                [this, &class_contents](const cppast::cpp_variable& variable) {
                    class_contents.serialized_static_member_variables.push_back(
                        serialize(variable));
                },
                [this, &class_contents](const cppast::cpp_class& class_) {
                    class_contents.serialized_member_classes.push_back(
                        serialize(class_));
                },
                [this, &class_contents](const cppast::cpp_enum& enum_) {
                    class_contents.serialized_member_enums.push_back(
                        serialize(enum_));
                });

            return _entity_registry(
                class_,
                fmt::format(
                    "TINYREFL_CLASS(({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}))",
                    string_constant(_entity_names.name(class_)),
                    string_constant(_entity_names.full_name(class_)),
                    parent_id(class_),
                    source_location(class_),
                    _type_serializer.full_type(class_),
                    _sequence_serializer.sequence(
                        class_.bases(),
                        [this](const cppast::cpp_base_class& base_class) {
                            return _type_serializer.full_type(base_class);
                        }),
                    _sequence_serializer.sequence(
                        class_contents.serialized_constructors),
                    _sequence_serializer.sequence(
                        class_contents.serialized_member_functions),
                    _sequence_serializer.sequence(
                        class_contents.serialized_member_variables),
                    _sequence_serializer.sequence(
                        class_contents.serialized_static_member_functions),
                    _sequence_serializer.sequence(
                        class_contents.serialized_static_member_variables),
                    _sequence_serializer.sequence(
                        class_contents.serialized_member_classes),
                    _sequence_serializer.sequence(
                        class_contents.serialized_member_enums),
                    serialize(class_.attributes())));
        }
    };

    if(class_contents_out != nullptr)
    {
        return body(*class_contents_out);
    }
    else
    {
        tinyrefl::tool::class_contents contents;
        return body(contents);
    }
}

const std::string& entity_serializer::serialize(
    const cppast::cpp_namespace&        namespace_,
    tinyrefl::tool::namespace_contents* namespace_contents_out)
{
    auto body = [this, &namespace_](
                    tinyrefl::tool::namespace_contents& namespace_contents)
        -> const std::string& {
        if(_entity_registry.already_registered(namespace_))
        {
            return _entity_registry[namespace_];
        }
        else
        {
            tinyrefl::tool::visit_entity(
                namespace_,
                [this,
                 &namespace_contents](const cppast::cpp_function& function) {
                    namespace_contents.serialized_functions.push_back(
                        serialize(function));
                },
                [this,
                 &namespace_contents](const cppast::cpp_variable& variable) {
                    namespace_contents.serialized_variables.push_back(
                        serialize(variable));
                },
                [this, &namespace_contents](const cppast::cpp_class& class_) {
                    namespace_contents.serialized_classes.push_back(
                        serialize(class_));
                },
                [this, &namespace_contents](const cppast::cpp_enum& enum_) {
                    namespace_contents.serialized_enums.push_back(
                        serialize(enum_));
                },
                [this,
                 &namespace_contents](const cppast::cpp_namespace& namespace_) {
                    namespace_contents.serialized_namespaces.push_back(
                        serialize(namespace_));
                });
            return _entity_registry(
                namespace_,

                fmt::format(
                    "TINYREFL_NAMESPACE(({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}), ({}))",
                    string_constant(_entity_names.name(namespace_)),
                    string_constant(_entity_names.full_name(namespace_)),
                    parent_id(namespace_),
                    source_location(namespace_),
                    string_constant(_entity_names.display_name(namespace_)),
                    string_constant(
                        _entity_names.full_display_name(namespace_)),
                    _sequence_serializer.sequence(
                        namespace_contents.serialized_functions),
                    _sequence_serializer.sequence(
                        namespace_contents.serialized_variables),
                    _sequence_serializer.sequence(
                        namespace_contents.serialized_classes),
                    _sequence_serializer.sequence(
                        namespace_contents.serialized_enums),
                    _sequence_serializer.sequence(
                        namespace_contents.serialized_namespaces),
                    serialize(namespace_.attributes())));
        }
    };

    if(namespace_contents_out != nullptr)
    {
        return body(*namespace_contents_out);
    }
    else
    {
        tinyrefl::tool::namespace_contents contents;
        return body(contents);
    }
}

const std::string& entity_serializer::serialize(const cppast::cpp_file& file)
{
    if(_entity_registry.already_registered(file))
    {
        return _entity_registry[file];
    }
    else
    {
        std::vector<std::string> namespaces;

        tinyrefl::tool::visit_entity(
            file, [this, &namespaces](const cppast::cpp_namespace& namespace_) {
                fmt::print(
                    "[debug] namespace \"{}\" from file \"{}\" (node: {})\n",
                    _entity_names.full_name(namespace_),
                    _entity_names.full_name(namespace_.parent().value()),
                    reinterpret_cast<const void*>(&namespace_));

                namespaces.push_back(serialize(namespace_));
            });

        fmt::print(
            "[debug] file \"{}\" contains {} namespaces\n",
            _entity_names.full_name(file),
            namespaces.size());

        return _entity_registry(
            file,
            fmt::format(
                "TINYREFL_FILE(({}), ({}), ({}))",
                string_constant(_entity_names.name(file)),
                string_constant(_entity_names.full_name(file)),
                _sequence_serializer.sequence(namespaces)));
    }
}

std::string entity_serializer::serialize(const cppast::cpp_attribute& attribute)
{
    std::vector<std::string> arguments;
    std::ostringstream       full_attribute;
    std::ostringstream       full_name;
    std::string              namespace_;

    if(attribute.scope().has_value())
    {
        namespace_ = attribute.scope().value();
        full_attribute << namespace_ << "::" << attribute.name();
        full_name << namespace_ << "::" << attribute.name();
    }
    else
    {
        full_attribute << attribute.name();
        full_name << attribute.name();
    }

    if(attribute.arguments().has_value())
    {
        const auto& attribute_arguments = attribute.arguments().value();
        full_attribute << "(" << attribute_arguments.as_string() << ")";

        for(const auto& argument : attribute_arguments)
        {
            arguments.push_back(string_constant(argument.spelling));
        }
    }

    return fmt::format(
        "TINYREFL_ATTRIBUTE(({}), ({}), ({}), ({}), ({}))",
        string_constant(attribute.name()),
        string_constant(full_name.str()),
        string_constant(namespace_),
        string_constant(full_attribute.str()),
        _sequence_serializer.sequence(arguments));
}

std::string
    entity_serializer::serialize(const cppast::cpp_attribute_list& attributes)
{
    return _sequence_serializer.sequence(
        attributes, [this](const cppast::cpp_attribute& attribute) {
            return serialize(attribute);
        });
}

