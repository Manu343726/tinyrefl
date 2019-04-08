#ifndef TINYREFL_OVERLOADED_FUNCTION_HPP
#define TINYREFL_OVERLOADED_FUNCTION_HPP

#include <tinyrefl/utils/meta.hpp>

namespace tinyrefl
{

namespace detail
{

template<typename Functions>
struct overloaded_function;

template<typename Head, typename Second, typename... Tail>
struct overloaded_function<tinyrefl::meta::list<Head, Second, Tail...>>
    : public Head,
      public overloaded_function<tinyrefl::meta::list<Second, Tail...>>
{
    using Head::operator();
    using overloaded_function<tinyrefl::meta::list<Second, Tail...>>::
        operator();

    constexpr overloaded_function(Head head, Second second, Tail... tail)
        : Head{head},
          overloaded_function<tinyrefl::meta::list<Second, Tail...>>{second,
                                                                     tail...}
    {
    }
};

template<typename Function, typename Args, typename = void>
struct is_invokable : public std::false_type
{
};

template<typename Function, typename... Args>
struct is_invokable<
    Function,
    tinyrefl::meta::list<Args...>,
    tinyrefl::meta::void_t<typename std::result_of<Function(Args...)>::type>>
    : std::true_type
{
};

template<typename Head>
struct overloaded_function<tinyrefl::meta::list<Head>> : public Head
{
    constexpr overloaded_function(Head head) : Head{head} {}

    using Head::operator();
};
} // namespace detail

template<typename... Functions>
using overloaded_function_t =
    tinyrefl::detail::overloaded_function<tinyrefl::meta::list<Functions...>>;

template<typename... Functions>
constexpr auto overloaded_function(Functions... functions)
{
    return overloaded_function_t<Functions...>{functions...};
}

struct function_fallback_result
{
    constexpr function_fallback_result() = default;
};

namespace detail
{

template<typename Function>
struct function_fallback
{
    template<typename... Args>
    constexpr auto operator()(Args&&... args) const -> std::enable_if_t<
        !tinyrefl::detail::is_invokable<
            Function,
            tinyrefl::meta::list<decltype(std::forward<Args>(args))...>>::value,
        tinyrefl::function_fallback_result>
    {
        return {};
    }
};
} // namespace detail

template<typename... Functions>
constexpr auto overloaded_function_default(Functions... functions)
{
    using overloaded_t = overloaded_function_t<Functions...>;
    return overloaded_function(
        functions..., tinyrefl::detail::function_fallback<overloaded_t>{});
}
} // namespace tinyrefl

#endif // TINYREFL_OVERLOADED_FUNCTION_HPP
