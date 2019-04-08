#ifndef TINYREFL_TYPES_ARRAY_HPP_INCLUDED
#define TINYREFL_TYPES_ARRAY_HPP_INCLUDED

#include <tinyrefl/types/static_value.hpp>
#include <tinyrefl/utils/algorithm.hpp>
#include <tinyrefl/utils/meta.hpp>

namespace tinyrefl
{

template<typename T, std::size_t N>
struct array
{
    template<typename... Items>
    constexpr array(Items... items) : _items{std::move(items)...}
    {
    }
    constexpr array()             = default;
    constexpr array(const array&) = default;
    constexpr array(array&&)      = default;

    using const_iterator = const T*;

    constexpr const_iterator begin() const
    {
        return &_items[0];
    }

    constexpr const_iterator end() const
    {
        return &_items[N];
    }

    constexpr std::size_t size() const
    {
        return N;
    }

    constexpr const T& operator[](const std::size_t index) const
    {
        return _items[index];
    }

public:
    std::array<T, N> _items;
};

template<typename T>
struct array<T, 0>
{
    constexpr array()             = default;
    constexpr array(const array&) = default;
    constexpr array(array&&)      = default;

    struct dummy_iterator
    {
        constexpr dummy_iterator& operator++()
        {
            return *this;
        }

        constexpr dummy_iterator& operator--()
        {
            return *this;
        }

        constexpr dummy_iterator& operator++(int)
        {
            return *this;
        }

        constexpr dummy_iterator& operator--(int)
        {
            return *this;
        }

        constexpr const T& operator*() const
        {
            return *static_cast<const T*>(nullptr);
        }

        constexpr const T* operator->() const
        {
            return nullptr;
        }

        constexpr operator const T*() const
        {
            return nullptr;
        }

        constexpr bool operator==(dummy_iterator) const
        {
            return true;
        }

        constexpr bool operator!=(dummy_iterator) const
        {
            return true;
        }

        constexpr bool operator>=(dummy_iterator) const
        {
            return true;
        }

        constexpr bool operator<=(dummy_iterator) const
        {
            return true;
        }

        constexpr bool operator>(dummy_iterator) const
        {
            return false;
        }

        constexpr bool operator<(dummy_iterator) const
        {
            return false;
        }
    };

    constexpr dummy_iterator begin() const
    {
        return {};
    }

    constexpr dummy_iterator end() const
    {
        return {};
    }

    constexpr std::size_t size() const
    {
        return 0;
    }

    constexpr const T& operator[](const std::size_t index) const
    {
        return *static_cast<const T*>(nullptr);
    }
};

template<
    typename List,
    typename ItemType  = void,
    typename Transform = tinyrefl::meta::defer<tinyrefl::meta::identity>>
struct typelist_to_array;

namespace impl
{

template<typename Transform, typename Value>
using type_transformed_value_type =
    decltype(tinyrefl::meta::apply_t<Transform, Value>::value);

template<typename Transform, typename Value>
using value_transformed_value_type = decltype(
    Transform::transform(tinyrefl::meta::apply_t<Transform, Value>::value));

template<typename Transform, typename Value>
using constexpr_default_constructed_value_transformed_value_type =
    decltype(std::array<int, (Value{}, 1)>{}, Transform::transform(Value{}));

template<typename Transform, typename ValueType, typename = void>
struct has_value_transform : std::false_type
{
};

template<typename Transform, typename ValueType>
struct has_value_transform<
    Transform,
    ValueType,
    std::void_t<decltype(Transform::transform(std::declval<ValueType>()))>>
    : std::true_type
{
};

template<
    typename Transform,
    typename Value,
    bool IsStaticValue = tinyrefl::is_static_value<Value>::value>
struct has_value_transform_accepting_static_value : std::false_type
{
};

template<typename Transform, typename Value>
struct has_value_transform_accepting_static_value<Transform, Value, true>
    : has_value_transform<Transform, decltype(Value::value)>
{
};

template<
    typename Transform,
    typename Value,
    bool IsStaticValue = tinyrefl::is_static_value<Value>::value,
    bool HasValueTransformAcceptingStaticValue =
        has_value_transform_accepting_static_value<Transform, Value>::value,
    bool HasValueTransformAcceptingConstexprInstance =
        has_value_transform<Transform, Value>::value>
struct apply_transform
{
    using value_type =
        decltype(tinyrefl::meta::apply_t<Transform, Value>::value);

    static constexpr const value_type& value()
    {
        return tinyrefl::meta::apply_t<Transform, Value>::value;
    }
};

template<
    typename Transform,
    typename Value,
    bool HasValueTransformAcceptingConstexprInstance>
struct apply_transform<
    Transform,
    Value,
    true, // Value is a static_value,
    true, // Transform has transform() method accepting Value::value
    HasValueTransformAcceptingConstexprInstance>
{
    using value_type = decltype(Transform::template transform(Value::value));

    static constexpr const value_type& value()
    {
        return Transform::template transform(Value::value);
    }
};

template<typename Transform, typename Value>
struct apply_transform<Transform, Value, false, false, false>
{
    using value_type = tinyrefl::meta::apply_t<Transform, Value>;

    static constexpr value_type value()
    {
        return value_type{};
    }
};

template<typename Transform, typename Value>
struct apply_transform<Transform, Value, false, false, true>
{
    using value_type =
        decltype(Transform::template transform(std::declval<Value>()));

    static constexpr value_type value()
    {
        return Transform::template transform(Value{});
    }
};

} // namespace impl

template<
    typename Value,
    typename... Values,
    typename ItemType,
    typename Transform>
struct typelist_to_array<
    tinyrefl::meta::list<Value, Values...>,
    ItemType,
    Transform>
{
    using value_type = std::remove_reference_t<decltype(
        tinyrefl::impl::apply_transform<Transform, Value>::value())>;

    static constexpr tinyrefl::array<value_type, sizeof...(Values) + 1> value{
        tinyrefl::impl::apply_transform<Transform, Value>::value(),
        tinyrefl::impl::apply_transform<Transform, Values>::value()...};
};

template<
    typename Value,
    typename... Values,
    typename ItemType,
    typename Transform>
constexpr tinyrefl::array<
    typename typelist_to_array<
        tinyrefl::meta::list<Value, Values...>,
        ItemType,
        Transform>::value_type,
    sizeof...(Values) + 1>
    typelist_to_array<
        tinyrefl::meta::list<Value, Values...>,
        ItemType,
        Transform>::value;

template<typename ItemType, typename Transform>
struct typelist_to_array<tinyrefl::meta::list<>, ItemType, Transform>
{
    static constexpr tinyrefl::array<ItemType, 0> value = {};
};

template<typename ItemType, typename Transform>
constexpr tinyrefl::array<ItemType, 0>
    typelist_to_array<tinyrefl::meta::list<>, ItemType, Transform>::value;

template<typename Transform>
struct typelist_to_array<tinyrefl::meta::list<>, void, Transform>
{
    static constexpr tinyrefl::array<int, 0> value = tinyrefl::array<int, 0>{};
};

template<typename Transform>
constexpr tinyrefl::array<int, 0>
    typelist_to_array<tinyrefl::meta::list<>, void, Transform>::value;
} // namespace tinyrefl

#endif // TINYREFL_TYPES_ARRAY_HPP_INCLUDED
