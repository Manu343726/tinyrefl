#ifndef TINYREFL_TYPES_TYPE_HPP_INCLUDED
#define TINYREFL_TYPES_TYPE_HPP_INCLUDED

namespace tinyrefl
{

template<typename T>
struct Type
{
    constexpr Type() = default;
    using type       = T;
};

template<typename T>
using type_tag = Type<T>;

template<typename T>
constexpr type_tag<T> type_constant{};

} // namespace tinyrefl

#endif // TINYREFL_TYPES_TYPE_HPP_INCLUDED
