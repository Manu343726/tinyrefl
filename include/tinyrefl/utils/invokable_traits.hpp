#ifndef TINYREFL_UTILS_INVOKABLE_TRAITS_HPP
#define TINYREFL_UTILS_INVOKABLE_TRAITS_HPP

#include <tinyrefl/utils/meta.hpp>
#include <type_traits>

namespace tinyrefl
{

namespace invokable_traits
{

enum class invokable_kind
{
    FREE_FUNCTION,
    NON_CONST_MEMBER_FUNCTION,
    CONST_MEMBER_FUNCTION,
    MEMBER_VARIABLE,
    VARIABLE
};

template<typename T>
struct traits
{
    static constexpr tinyrefl::invokable_traits::invokable_kind kind =
        tinyrefl::invokable_traits::invokable_kind::VARIABLE;

    using signature         = T();
    using return_type       = T;
    using class_type        = void;
    using arguments         = tinyrefl::meta::list<>;
    using decayed_arguments = tinyrefl::meta::list<>;
};

template<typename R, typename Class, typename... Args>
struct traits<R (Class::*)(Args...)>
{
    static constexpr tinyrefl::invokable_traits::invokable_kind kind =
        tinyrefl::invokable_traits::invokable_kind::NON_CONST_MEMBER_FUNCTION;

    using signature         = R(Args...);
    using return_type       = R;
    using class_type        = Class;
    using arguments         = tinyrefl::meta::list<Args...>;
    using decayed_arguments = tinyrefl::meta::list<std::decay_t<Args>...>;
};

template<typename R, typename Class, typename... Args>
struct traits<R (Class::*)(Args...) const>
{
    static constexpr tinyrefl::invokable_traits::invokable_kind kind =
        tinyrefl::invokable_traits::invokable_kind::CONST_MEMBER_FUNCTION;

    using signature         = R(Args...);
    using return_type       = R;
    using class_type        = Class;
    using arguments         = tinyrefl::meta::list<Args...>;
    using decayed_arguments = tinyrefl::meta::list<std::decay_t<Args>...>;
};

template<typename R, typename... Args>
struct traits<R(Args...)>
{
    static constexpr tinyrefl::invokable_traits::invokable_kind kind =
        tinyrefl::invokable_traits::invokable_kind::FREE_FUNCTION;

    using signature         = R(Args...);
    using return_type       = R;
    using class_type        = void;
    using arguments         = tinyrefl::meta::list<Args...>;
    using decayed_arguments = tinyrefl::meta::list<std::decay_t<Args>...>;
};

template<typename R, typename... Args>
struct traits<R (*)(Args...)> : public traits<R(Args...)>
{
};

template<typename T, typename Class>
struct traits<T Class::*>
{
    static constexpr tinyrefl::invokable_traits::invokable_kind kind =
        tinyrefl::invokable_traits::invokable_kind::MEMBER_VARIABLE;

    using signature         = T();
    using return_type       = T;
    using class_type        = Class;
    using arguments         = tinyrefl::meta::list<>;
    using decayed_arguments = tinyrefl::meta::list<>;
};

template<typename T, template<typename, T> class StaticValue, T Value>
struct traits<StaticValue<T, Value>> : public traits<T>
{
};

template<typename Function>
constexpr tinyrefl::invokable_traits::invokable_kind kind =
    traits<Function>::kind;

template<typename Function>
constexpr bool is_const =
    tinyrefl::invokable_traits::kind<Function> ==
    tinyrefl::invokable_traits::invokable_kind::CONST_MEMBER_FUNCTION;

template<typename Function>
using return_type = typename traits<Function>::return_type;

template<typename Function>
using class_type = typename traits<Function>::class_type;

template<typename Function>
using arguments = typename traits<Function>::arguments;

template<typename Function>
using decayed_arguments = typename traits<Function>::decayed_arguments;

template<typename Function>
using signature = typename traits<Function>::signature;

template<typename Function>
constexpr bool requires_object =
    kind<Function> == invokable_kind::CONST_MEMBER_FUNCTION ||
    kind<Function> == invokable_kind::NON_CONST_MEMBER_FUNCTION ||
    kind<Function> == invokable_kind::MEMBER_VARIABLE;
} // namespace invokable_traits

} // namespace tinyrefl

#endif // TINYREFL_UTILS_INVOKABLE_TRAITS_HPP
