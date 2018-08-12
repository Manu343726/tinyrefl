#ifndef TINYREFL_EXAMPLES_FUSION_HPP
#define TINYREFL_EXAMPLES_FUSION_HPP

#include <boost/fusion/adapted.hpp>
#include <type_traits>

#ifndef TINYREFL_PP_UNWRAP
#define TINYREFL_PP_UNWRAP(...) __VA_ARGS__
#endif // TINYREFL_PP_UNWRAP

namespace tinyrefl
{

namespace examples
{

namespace detail
{

template<typename MemberPtr>
struct member_type;

template<typename T, typename Class>
struct member_type<T Class::*>
{
    using type = T;
};

template<typename MemberPtr>
using member_type_t = typename member_type<MemberPtr>::type;

}

}

}

// Boost Fusion backend for tinyrefl metadata
#define TINYREFL_GODMODE(...) // No Gods here
#define TINYREFL_SEQUENCE(...) __VA_ARGS__
#define TINYREFL_STRING(...) __VA_ARGS__
#define TINYREFL_TYPE(name, fullname) fullname
#define TINYREFL_VALUE(type, value) // We don't care about values (pointers to members, enums, etc)
#define TINYREFL_CONSTRUCTOR(name, fullname, class_type, signature) // We don't care about ctors
#define TINYREFL_MEMBER_FUNCTION(name, fullname, parent_class_type, return_type, signature, pointer) // we don't care about member functions
#define TINYREFL_MEMBER_VARIABLE(name, fullname, parent_class_type, value_type, pointer) (value_type, name)
#define TINYREFL_REFLECT_MEMBER(member) // we don't care about per-member metadata
#define TINYREFL_ENUM_VALUE(name, fullname, type, value) // we don't care about enums
#define TINYREFL_REFLECT_ENUM_VALUE(value) // we don't care about enums
#define TINYREFL_REFLECT_ENUM(name, type, values) // we don't care about enums
#define TINYREFL_REFLECT_CLASS(classname, bases, constructors, member_functions, member_variables, classes, enums) \
    BOOST_FUSION_ADAPT_STRUCT(classname, TINYREFL_PP_UNWRAP member_variables);

#endif // TINYREFL_EXAMPLES_FUSION_HPP
