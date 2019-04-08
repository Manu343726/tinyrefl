#ifndef TINYREFL_ENTITIES_POINTER_HPP_INCLUDED
#define TINYREFL_ENTITIES_POINTER_HPP_INCLUDED

namespace tinyrefl
{

namespace entities
{
template<typename Pointer>
struct pointer
{
    using pointer_static_value          = Pointer;
    using pointer_type                  = decltype(Pointer::value);
    static constexpr pointer_type value = Pointer::value;

    constexpr pointer_type get() const
    {
        return Pointer::value;
    }
};

} // namespace entities
} // namespace tinyrefl
#endif // TINYREFL_ENTITIES_POINTER_HPP_INCLUDED
