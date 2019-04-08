#ifndef TINYREFL_TYPES_STATIC_VALUE_HPP_INCLUDED
#define TINYREFL_TYPES_STATIC_VALUE_HPP_INCLUDED

#include <ctti/static_value.hpp>
#include <type_traits>

namespace tinyrefl
{

template<typename T, T Value>
using static_value = ctti::static_value<T, Value>;

#define TINYREFL_STATIC_VALUE(...) \
    ::tinyrefl::static_value<decltype(__VA_ARGS__), (__VA_ARGS__)>

template<typename T, typename = void>
struct is_static_value : std::false_type
{
};

template<typename T>
struct is_static_value<T, std::void_t<decltype(T::value)>> : std::true_type
{
};
} // namespace tinyrefl

#endif // TINYREFL_TYPES_STATIC_VALUE_HPP_INCLUDED
