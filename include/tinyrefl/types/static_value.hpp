#ifndef TINYREFL_TYPES_STATIC_VALUE_HPP_INCLUDED
#define TINYREFL_TYPES_STATIC_VALUE_HPP_INCLUDED

#include <ctti/static_value.hpp>
#include <tinyrefl/utils/language_features.hpp>
#include <tinyrefl/utils/meta.hpp>
#include <type_traits>

#ifdef TINYREFL_HAS_CONSTEXPR_LAMBDAS
// See
// https://mpark.github.io/programming/2017/05/26/constexpr-function-parameters/

#define TINYREFL_CONSTANT(...)                                                 \
    union                                                                      \
    {                                                                          \
        static constexpr auto tinyrefl_constant_value()                        \
        {                                                                      \
            return __VA_ARGS__;                                                \
        }                                                                      \
    }

#define TINYREFL_CONSTANT_VALUE(...)                                           \
    [] {                                                                       \
        using R = TINYREFL_CONSTANT(__VA_ARGS__);                              \
        return R{};                                                            \
    }()
#endif // TINYREFL_HAS_CONSTEXPR_LAMBDAS

namespace tinyrefl
{

template<typename T, T Value>
using static_value = ctti::static_value<T, Value>;

#define TINYREFL_STATIC_VALUE(...)                                             \
    ::tinyrefl::static_value<decltype(__VA_ARGS__), (__VA_ARGS__)>

template<typename T, typename = void>
struct is_static_value : std::false_type
{
};

template<typename T>
struct is_static_value<T, tinyrefl::meta::void_t<decltype(T::value)>>
    : std::true_type
{
};
} // namespace tinyrefl

#endif // TINYREFL_TYPES_STATIC_VALUE_HPP_INCLUDED
