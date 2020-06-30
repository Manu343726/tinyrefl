#ifndef TINYREFL_EXAMPLES_HANA_HPP
#define TINYREFL_EXAMPLES_HANA_HPP

#include <boost/hana.hpp>

#ifndef TINYREFL_PP_UNWRAP
#define TINYREFL_PP_UNWRAP(...) __VA_ARGS__
#endif // TINYREFL_PP_UNWRAP

#define TINYREFL_API_CODEGEN_VERSION_MAJOR 0
#define TINYREFL_API_CODEGEN_VERSION_MINOR 4
#define TINYREFL_API_CODEGEN_VERSION_FIX 1

// Boost Hana backend for tinyrefl metadata
#define TINYREFL_GODMODE(...) // No Gods here
#define TINYREFL_SEQUENCE(items) TINYREFL_PP_UNWRAP items
#define TINYREFL_STRING(...) __VA_ARGS__
#define TINYREFL_TYPE(name, fullname) TINYREFL_PP_UNWRAP fullname
#define TINYREFL_VALUE(type, value) // we don't care about values
#define TINYREFL_ATTRIBUTE(                                                    \
    name, namespace_, full_attribute, args) // We don't care about attributes
#define TINYREFL_CONSTRUCTOR(                                                  \
    name,                                                                      \
    fullname,                                                                  \
    class_type,                                                                \
    signature,                                                                 \
    attributes) // We don't care about ctors
#define TINYREFL_MEMBER_FUNCTION(                                              \
    name,                                                                      \
    fullname,                                                                  \
    display_name,                                                              \
    full_display_name,                                                         \
    parent_class_type,                                                         \
    return_type,                                                               \
    signature,                                                                 \
    arg_names,                                                                 \
    pointer,                                                                   \
    attributes) // we don't care about member functions
#define TINYREFL_MEMBER_VARIABLE(                                              \
    name, fullname, parent_class_type, value_type, pointer, attributes)        \
    TINYREFL_PP_UNWRAP name // Just it's name
#define TINYREFL_ENUM_VALUE(                                                   \
    name, fullname, type, value, attributes) // we don't care about enums
#define TINYREFL_REFLECT_MEMBER(                                               \
    member) // We don't care about metadata definitions of members
#define TINYREFL_REFLECT_ENUM_VALUE(value) // we don't care about enums
#define TINYREFL_REFLECT_ENUM(                                                 \
    name, type, values, attributes) // we don't care about enums
#define TINYREFL_REFLECT_CLASS(                                                \
    classname,                                                                 \
    classtype,                                                                 \
    bases,                                                                     \
    constructors,                                                              \
    member_functions,                                                          \
    member_variables,                                                          \
    classes,                                                                   \
    enums,                                                                     \
    attributes)                                                                \
    BOOST_HANA_ADAPT_STRUCT(                                                   \
        TINYREFL_PP_UNWRAP classname, TINYREFL_PP_UNWRAP member_variables);

#endif // TINYREFL_EXAMPLES_HANA_HPP
