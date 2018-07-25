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
#define TINYREFL_VALUE(value) value
#define TINYREFL_MEMBER(name, fullname, type, pointer) (::tinyrefl::examples::detail::member_type_t<decltype(pointer)>, name)
#define TINYREFL_REFLECT_MEMBER(member) // we don't care about per-member metadata
#define TINYREFL_ENUM_VALUE(name, fullname, type, value) // we don't care about enums
#define TINYREFL_REFLECT_ENUM_VALUE(value) // we don't care about enums
#define TINYREFL_REFLECT_ENUM(name, type, values) // we don't care about enums
#define TINYREFL_REFLECT_CLASS(classname, bases, members, classes, enums) \
    BOOST_FUSION_ADAPT_STRUCT(classname, TINYREFL_PP_UNWRAP members);

#endif // TINYREFL_EXAMPLES_FUSION_HPP
