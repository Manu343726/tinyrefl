#ifndef TINYREFL_EXAMPLES_HANA_HPP
#define TINYREFL_EXAMPLES_HANA_HPP

#include <boost/hana.hpp>

#ifndef TINYREFL_PP_UNWRAP
#define TINYREFL_PP_UNWRAP(...) __VA_ARGS__
#endif // TINYREFL_PP_UNWRAP

// Boost Hana backend for tinyrefl metadata
#define TINYREFL_GODMODE(...) // No Gods here
#define TINYREFL_SEQUENCE(...) __VA_ARGS__
#define TINYREFL_STRING(...) __VA_ARGS__
#define TINYREFL_TYPE(name, fullname) fullname
#define TINYREFL_VALUE(value) // we don't care about values
#define TINYREFL_CONSTRUCTOR(name, fullname, class_type, signature) // We don't care about ctors
#define TINYREFL_MEMBER_FUNCTION(name, fullname, parent_class_type, return_type, signature, pointer) // we don't care about member functions
#define TINYREFL_MEMBER_VARIABLE(name, fullname, parent_class_type, value_type, pointer) name // Just it's name
#define TINYREFL_REFLECT_MEMBER(member) // we don't care about per-member metadata
#define TINYREFL_ENUM_VALUE(name, fullname, type, value) // we don't care about enums
#define TINYREFL_REFLECT_ENUM_VALUE(value) // we don't care about enums
#define TINYREFL_REFLECT_ENUM(name, type, values) // we don't care about enums
#define TINYREFL_REFLECT_CLASS(classname, bases, constructors, member_functions, member_variables, classes, enums) \
    BOOST_HANA_ADAPT_STRUCT(classname, TINYREFL_PP_UNWRAP member_variables);

#endif // TINYREFL_EXAMPLES_HANA_HPP
