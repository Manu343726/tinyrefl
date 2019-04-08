#ifndef TINYREFL_UTILS_SWITCH_HPP
#define TINYREFL_UTILS_SWITCH_HPP

#include <tinyrefl/utils/meta.hpp>
#include <tinyrefl/utils/overloaded_function.hpp>

namespace tinyrefl
{

namespace impl
{

template<typename Value, template<typename...> class Seq, typename... Keys>
constexpr auto switch_(Value value, Seq<Keys...>)
{
    return [value](auto... callbacks) TINYREFL_CONSTEXPR_LAMBDA {
        return [value](auto callbacks) TINYREFL_CONSTEXPR_LAMBDA {
            bool missed = true;
            [](...) TINYREFL_CONSTEXPR_LAMBDA {
            }(std::array<int, sizeof...(Keys)>{
                {((value == Keys::value ? (missed = false, callbacks(Keys{}))
                                        : (void)1),
                  0)...}});

            return missed;
        }(tinyrefl::overloaded_function_default(callbacks...));
    };
}
} // namespace impl

template<typename Sequence, typename Value>
constexpr auto switch_(Value value)
{
    return tinyrefl::impl::switch_(value, Sequence{});
}
} // namespace tinyrefl

#endif // TINYRREFL_UTILS_SWITCH
