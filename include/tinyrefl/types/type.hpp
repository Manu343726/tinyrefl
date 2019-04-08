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

} // namespace tinyrefl

#endif // TINYREFL_TYPES_TYPE_HPP_INCLUDED
