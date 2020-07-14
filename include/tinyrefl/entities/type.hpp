#ifndef TINYREFL_ENTITIES_TYPE_HPP_INCLUDED
#define TINYREFL_ENTITIES_TYPE_HPP_INCLUDED

#include <ctti/detailed_nameof.hpp>
#include <tinyrefl/backend/metadata.hpp>
#include <type_traits>

namespace tinyrefl
{

namespace entities
{

template<typename T>
struct type_entity
{
    constexpr type_entity() = default;

    constexpr type_entity<std::decay_t<T>> decayed() const
    {
        return {};
    }

    constexpr tinyrefl::string name() const
    {
        return get_name(tinyrefl::meta::bool_<tinyrefl::has_metadata<T>()>{});
    }

    constexpr tinyrefl::string full_name() const
    {
        return get_full_name(
            tinyrefl::meta::bool_<tinyrefl::has_metadata<T>()>{});
    }

private:
    static constexpr tinyrefl::string get_name(tinyrefl::meta::true_)
    {
        return tinyrefl::metadata<T>().name();
    }

    static constexpr tinyrefl::string get_full_name(tinyrefl::meta::true_)
    {
        return tinyrefl::metadata<T>().full_name();
    }

    static constexpr tinyrefl::string get_name(tinyrefl::meta::false_)
    {
        return ctti::detailed_nameof<T>().name();
    }

    static constexpr tinyrefl::string get_full_name(tinyrefl::meta::false_)
    {
        return ctti::detailed_nameof<T>().full_name();
    }
};

template<typename Lhs, typename Rhs>
constexpr bool
    operator==(const type_entity<Lhs>& lhs, const type_entity<Rhs>& rhs)
{
    return std::is_same<Lhs, Rhs>::value;
}

template<typename Lhs, typename Rhs>
constexpr bool
    operator!=(const type_entity<Lhs>& lhs, const type_entity<Rhs>& rhs)
{
    return !(lhs == rhs);
}

namespace impl
{

template<
    typename T,
    bool IsType = std::is_base_of_v<type_entity<T>, tinyrefl::metadata<T>>>
struct type_base : public tinyrefl::metadata<T>, public type_entity<T>
{
    using type_entity<T>::name;
    using type_entity<T>::full_name;
};

template<typename T>
struct type_base<T, true> : public tinyrefl::metadata<T>
{
};

} // namespace impl

template<typename T>
struct type : public impl::type_base<T>
{
};

} // namespace entities

} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_TYPE_HPP_INCLUDED
