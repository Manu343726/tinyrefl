#ifndef TINYREFL_BACKEND_BACKEND_HPP
#define TINYREFL_BACKEND_BACKEND_HPP

#include <tinyrefl/backend/register_entity_macros.hpp>

#include <tinyrefl/entities/attribute.hpp>
#include <tinyrefl/entities/class.hpp>
#include <tinyrefl/entities/constructor.hpp>
#include <tinyrefl/entities/enum.hpp>
#include <tinyrefl/entities/enum_value.hpp>
#include <tinyrefl/entities/file.hpp>
#include <tinyrefl/entities/free_function.hpp>
#include <tinyrefl/entities/member_function.hpp>
#include <tinyrefl/entities/member_variable.hpp>
#include <tinyrefl/entities/namespace.hpp>
#include <tinyrefl/entities/static_member_function.hpp>
#include <tinyrefl/entities/static_member_variable.hpp>
#include <tinyrefl/entities/variable.hpp>

#include <tinyrefl/types/static_value.hpp>
#include <tinyrefl/types/string.hpp>
#include <tinyrefl/utils/meta.hpp>
#include <tinyrefl/utils/preprocessor.hpp>



#define TINYREFL_API_CODEGEN_VERSION_MAJOR 0
#define TINYREFL_API_CODEGEN_VERSION_MINOR 5
#define TINYREFL_API_CODEGEN_VERSION_FIX 0

#define TINYREFL_DEFINE_STRINGS

#define TINYREFL_STRING(...)                                                   \
    ::tinyrefl::hash_constant<tinyrefl::hash(TINYREFL_PP_STR(__VA_ARGS__))>

#define TINYREFL_REGISTER_STRING(...)                                          \
    namespace tinyrefl                                                         \
    {                                                                          \
    template<>                                                                 \
    constexpr ::tinyrefl::string                                               \
        string_constant<TINYREFL_STRING(__VA_ARGS__)>()                        \
    {                                                                          \
        return TINYREFL_PP_STR(__VA_ARGS__);                                   \
    }                                                                          \
    } // namespace tinyrefl

#define TINYREFL_SEQUENCE(elems)                                               \
    ::tinyrefl::meta::list<TINYREFL_PP_UNWRAP elems>

#define TINYREFL_SEQUENCE_CAT(x, y)                                            \
    ::tinyrefl::meta::cat_t<TINYREFL_PP_UNWRAP x, TINYREFL_PP_UNWRAP y>

#define TINYREFL_TYPE(name, fullname) TINYREFL_PP_UNWRAP fullname

#define TINYREFL_VALUE(type, value)                                            \
    ::ctti::static_value<TINYREFL_PP_UNWRAP type, TINYREFL_PP_UNWRAP value>

#define TINYREFL_ATTRIBUTE(                                                    \
    name, full_name, namespace_, full_attribute, args, type, joined_args)      \
    ::tinyrefl::entities::attribute_metadata<                                  \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP namespace_,                                         \
        TINYREFL_PP_UNWRAP full_attribute,                                     \
        TINYREFL_PP_UNWRAP args>

#define TINYREFL_SOURCE_LOCATION(file, line, column)                           \
    ::tinyrefl::entities::source_location<                                     \
        TINYREFL_PP_UNWRAP file,                                               \
        TINYREFL_PP_UNWRAP line,                                               \
        TINYREFL_PP_UNWRAP column>

#define TINYREFL_CONSTRUCTOR(                                                  \
    name,                                                                      \
    full_name,                                                                 \
    parent,                                                                    \
    source_location,                                                           \
    display_name,                                                              \
    full_display_name,                                                         \
    class_type,                                                                \
    parameter_types,                                                           \
    parameter_names,                                                           \
    attributes)                                                                \
    ::tinyrefl::entities::constructor<                                         \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP display_name,                                       \
        TINYREFL_PP_UNWRAP full_display_name,                                  \
        TINYREFL_PP_UNWRAP class_type,                                         \
        TINYREFL_PP_UNWRAP parameter_types,                                    \
        TINYREFL_PP_UNWRAP parameter_names,                                    \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_MEMBER_FUNCTION(                                              \
    name,                                                                      \
    full_name,                                                                 \
    parent,                                                                    \
    source_location,                                                           \
    display_name,                                                              \
    full_display_name,                                                         \
    return_type,                                                               \
    parameter_types,                                                           \
    parameter_names,                                                           \
    pointer,                                                                   \
    attributes)                                                                \
    ::tinyrefl::entities::member_function<                                     \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP display_name,                                       \
        TINYREFL_PP_UNWRAP full_display_name,                                  \
        TINYREFL_PP_UNWRAP return_type,                                        \
        TINYREFL_PP_UNWRAP parameter_types,                                    \
        TINYREFL_PP_UNWRAP parameter_names,                                    \
        TINYREFL_PP_UNWRAP pointer,                                            \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_MEMBER_VARIABLE(                                              \
    name, full_name, parent, source_location, value_type, pointer, attributes) \
    ::tinyrefl::entities::member_variable<                                     \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP pointer,                                            \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_STATIC_MEMBER_FUNCTION(                                       \
    name,                                                                      \
    full_name,                                                                 \
    parent,                                                                    \
    source_location,                                                           \
    display_name,                                                              \
    full_display_name,                                                         \
    return_type,                                                               \
    parameter_types,                                                           \
    parameter_names,                                                           \
    pointer,                                                                   \
    attributes)                                                                \
    ::tinyrefl::entities::static_member_function<                              \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP display_name,                                       \
        TINYREFL_PP_UNWRAP full_display_name,                                  \
        TINYREFL_PP_UNWRAP return_type,                                        \
        TINYREFL_PP_UNWRAP parameter_types,                                    \
        TINYREFL_PP_UNWRAP parameter_names,                                    \
        TINYREFL_PP_UNWRAP pointer,                                            \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_STATIC_MEMBER_VARIABLE(                                       \
    name, full_name, parent, source_location, value_type, pointer, attributes) \
    ::tinyrefl::entities::static_member_variable<                              \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP pointer,                                            \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_FUNCTION(                                                     \
    name,                                                                      \
    full_name,                                                                 \
    parent,                                                                    \
    source_location,                                                           \
    display_name,                                                              \
    full_display_name,                                                         \
    return_type,                                                               \
    parameter_types,                                                           \
    parameter_names,                                                           \
    pointer,                                                                   \
    attributes)                                                                \
    ::tinyrefl::entities::free_function<                                       \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP display_name,                                       \
        TINYREFL_PP_UNWRAP full_display_name,                                  \
        TINYREFL_PP_UNWRAP return_type,                                        \
        TINYREFL_PP_UNWRAP parameter_types,                                    \
        TINYREFL_PP_UNWRAP parameter_names,                                    \
        TINYREFL_PP_UNWRAP pointer,                                            \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_VARIABLE(                                                     \
    name, full_name, parent, source_location, value_type, pointer, attributes) \
    ::tinyrefl::entities::variable<                                            \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP pointer,                                            \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_ENUM_VALUE(                                                   \
    name, full_name, parent, source_location, enum_type, value, attributes)    \
    ::tinyrefl::entities::enum_value_metadata<                                 \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP enum_type,                                          \
        TINYREFL_PP_UNWRAP value,                                              \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_ENUM(                                                         \
    name, full_name, parent, source_location, enum_type, values, attributes)   \
    ::tinyrefl::entities::enum_<                                               \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP enum_type,                                          \
        TINYREFL_PP_UNWRAP values,                                             \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_CLASS(                                                        \
    name,                                                                      \
    full_name,                                                                 \
    parent,                                                                    \
    source_location,                                                           \
    class_type,                                                                \
    base_classes,                                                              \
    constructors,                                                              \
    member_functions,                                                          \
    member_variables,                                                          \
    static_member_functions,                                                   \
    static_member_variables,                                                   \
    member_classes,                                                            \
    member_enums,                                                              \
    attributes)                                                                \
    ::tinyrefl::entities::class_<                                              \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP class_type,                                         \
        TINYREFL_PP_UNWRAP base_classes,                                       \
        TINYREFL_PP_UNWRAP constructors,                                       \
        TINYREFL_PP_UNWRAP member_functions,                                   \
        TINYREFL_PP_UNWRAP member_variables,                                   \
        TINYREFL_PP_UNWRAP static_member_functions,                            \
        TINYREFL_PP_UNWRAP static_member_variables,                            \
        TINYREFL_PP_UNWRAP member_classes,                                     \
        TINYREFL_PP_UNWRAP member_enums,                                       \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_NAMESPACE(                                                    \
    name,                                                                      \
    full_name,                                                                 \
    parent,                                                                    \
    source_location,                                                           \
    display_name,                                                              \
    full_display_name,                                                         \
    functions,                                                                 \
    variables,                                                                 \
    classes,                                                                   \
    enums,                                                                     \
    namespaces,                                                                \
    attributes)                                                                \
    ::tinyrefl::entities::namespace_<                                          \
        TINYREFL_PP_UNWRAP name,                                               \
        TINYREFL_PP_UNWRAP full_name,                                          \
        TINYREFL_PP_UNWRAP parent,                                             \
        TINYREFL_PP_UNWRAP source_location,                                    \
        TINYREFL_PP_UNWRAP display_name,                                       \
        TINYREFL_PP_UNWRAP full_display_name,                                  \
        TINYREFL_PP_UNWRAP functions,                                          \
        TINYREFL_PP_UNWRAP variables,                                          \
        TINYREFL_PP_UNWRAP classes,                                            \
        TINYREFL_PP_UNWRAP enums,                                              \
        TINYREFL_PP_UNWRAP namespaces,                                         \
        TINYREFL_PP_UNWRAP attributes>

#define TINYREFL_FILE(filename, full_filename, namespaces)                     \
    ::tinyrefl::entities::file<                                                \
        TINYREFL_PP_UNWRAP filename,                                           \
        TINYREFL_PP_UNWRAP full_filename,                                      \
        TINYREFL_PP_UNWRAP namespaces>

#define TINYREFL_REGISTER_ATTRIBUTE(full_attribute, type, joined_args)         \
    namespace tinyrefl                                                         \
    {                                                                          \
    namespace backend                                                          \
    {                                                                          \
    template<>                                                                 \
    constexpr TINYREFL_PP_UNWRAP type attribute_instance<TINYREFL_STRING(      \
        TINYREFL_PP_UNWRAP full_attribute)>{TINYREFL_PP_UNWRAP joined_args};   \
    } /* namespace backend */                                                  \
    } // namespace tinyrefl

#define TINYREFL_MERGE_NAMESPACES(a, b)                                        \
    ::tinyrefl::entities::                                                     \
        merge_namespaces<TINYREFL_PP_UNWRAP a, TINYREFL_PP_UNWRAP b>

#define TINYREFL_REGISTER_CONSTRUCTOR(constructor)                             \
    TINYREFL_REGISTER_ENTITY_BY_FULL_DISPLAY_NAME(constructor)

#define TINYREFL_REGISTER_MEMBER_FUNCTION(function)                            \
    TINYREFL_REGISTER_ENTITY_BY_POINTER((function))

#define TINYREFL_REGISTER_MEMBER_VARIABLE(variable)                            \
    TINYREFL_REGISTER_ENTITY_BY_POINTER((variable))

#define TINYREFL_REGISTER_STATIC_MEMBER_FUNCTION(function)                     \
    TINYREFL_REGISTER_ENTITY_BY_POINTER((function))

#define TINYREFL_REGISTER_STATIC_MEMBER_VARIABLE(variable)                     \
    TINYREFL_REGISTER_ENTITY_BY_POINTER((variable))

#define TINYREFL_REGISTER_FUNCTION(function)                                   \
    TINYREFL_REGISTER_ENTITY_BY_POINTER((function))

#define TINYREFL_REGISTER_VARIABLE(variable)                                   \
    TINYREFL_REGISTER_ENTITY_BY_POINTER((variable))

#define TINYREFL_REGISTER_ENUM_VALUE(value)                                    \
    TINYREFL_REGISTER_ENTITY_BY_TYPE(value_static_value, value)

#define TINYREFL_REGISTER_ENUM(enum_)                                          \
    TINYREFL_REGISTER_ENTITY_BY_TYPE(enum_type, enum_)

#define TINYREFL_REGISTER_CLASS(class_)                                        \
    TINYREFL_REGISTER_ENTITY_BY_TYPE(class_type, class_)

#define TINYREFL_REGISTER_NAMESPACE(namespace_)                                \
    TINYREFL_REGISTER_ENTITY_BY_FULL_DISPLAY_NAME(namespace_)

#define TINYREFL_REGISTER_FILE(file)                                           \
    TINYREFL_REGISTER_ENTITY_BY_FULL_DISPLAY_NAME(file)

namespace tinyrefl
{

// Declaration of the tinyrefl::ignore attribute
struct ignore
{
};

} // namespace tinyrefl

#endif // TINYREFL_BACKEND_BACKEND_HPP
