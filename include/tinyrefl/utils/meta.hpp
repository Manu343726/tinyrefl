#ifndef TINYREFL_UTILS_META_HPP_INCLUDED
#define TINYREFL_UTILS_META_HPP_INCLUDED

#include <ctti/detail/meta.hpp>
#include <tinyrefl/types/type.hpp>
#include <tinyrefl/utils/language_features.hpp>
#include <tuple>

namespace tinyrefl
{

namespace meta
{

using namespace ctti::meta;

namespace impl
{

template<typename Seq, typename T>
struct index_of
{
    static constexpr int value =
        index_of<tinyrefl::meta::tail_t<Seq>, T>::value >= 0
            ? index_of<tinyrefl::meta::tail_t<Seq>, T>::value + 1
            : -1;
};

template<template<typename...> class Seq, typename... Tail, typename T>
struct index_of<Seq<T, Tail...>, T>
{
    static constexpr int value = 0;
};

template<template<typename...> class Seq, typename T>
struct index_of<Seq<>, T>
{
    static constexpr int value = -1;
};

} // namespace impl

template<typename T, typename Seq>
using index_of = impl::index_of<Seq, T>;

template<typename T, typename... Ts>
using pack_index_of = index_of<T, tinyrefl::meta::list<Ts...>>;

template<typename T, typename Seq>
using contains = tinyrefl::meta::bool_<(index_of<T, Seq>::value >= 0)>;

template<typename T, typename... Ts>
using pack_contains = contains<T, tinyrefl::meta::list<Ts...>>;

template<template<typename...> class Zipper, typename... Sequences>
struct zip
{
    using type = tinyrefl::meta::prepend<
        Zipper<tinyrefl::meta::head_t<Sequences>...>,
        tinyrefl::meta::type_t<
            zip<Zipper, tinyrefl::meta::tail_t<Sequences>...>>>;
};

template<
    template<typename...>
    class Zipper,
    template<typename...>
    class... Sequences,
    typename... Tails>
struct zip<Zipper, Sequences<Tails>...>
{
    using type = tinyrefl::meta::list<Zipper<Tails...>>;
};

template<template<typename...> class Zipper, typename SeqOfSeqs>
using zip_t = tinyrefl::meta::type_t<zip<Zipper, SeqOfSeqs>>;

template<template<typename...> class Zipper, typename... PackOfSeqs>
using pack_zip = zip<Zipper, tinyrefl::meta::list<PackOfSeqs...>>;

template<template<typename...> class Zipper, typename... PackOfSeqs>
using pack_zip_t = tinyrefl::meta::type_t<pack_zip<Zipper, PackOfSeqs...>>;

template<typename Pair>
struct inverted_pair;

template<template<typename, typename> class Pair, typename Lhs, typename Rhs>
struct inverted_pair<Pair<Lhs, Rhs>>
{
    using type = Pair<Rhs, Lhs>;
};

template<typename Map>
using inverted_map =
    tinyrefl::meta::fmap_t<tinyrefl::meta::defer<inverted_pair>, Map>;

template<typename... Pairs>
struct slow_map;

template<typename... Keys, typename... Values>
struct slow_map<tinyrefl::meta::pair<Keys, Values>...>
{
    using keys   = tinyrefl::meta::list<Keys...>;
    using values = tinyrefl::meta::list<Values...>;

    struct key_not_found
    {
    };

    template<typename Key>
    using at_key = std::conditional_t<
        tinyrefl::meta::contains<Key, keys>::value,
        tinyrefl::meta::
            get_t<tinyrefl::meta::index_of<Key, keys>::value, values>,
        key_not_found>;
};

template<typename... Ts>
constexpr std::tuple<Ts...> make_tuple(tinyrefl::meta::list<Ts...>)
{
    return std::make_tuple(Ts{}...);
}

namespace impl
{
template<typename... Ts, typename Visitor, std::size_t... Indices>
constexpr void foreach(
    const std::tuple<Ts...>& tuple,
    Visitor && visitor,
    std::index_sequence<Indices...>)
{
    (void)std::array<int, sizeof...(Ts)>{
        {(visitor(std::get<Indices>(tuple)), 0)...}};
}

template<typename... Ts, typename Visitor, std::size_t... Indices>
constexpr void foreach(
    std::tuple<Ts...>& tuple,
    Visitor && visitor,
    std::index_sequence<Indices...>)
{
    (void)std::array<int, sizeof...(Ts)>{
        {(visitor(std::get<Indices>(tuple)), 0)...}};
}

template<
    typename... Ts,
    typename Visitor,
    std::size_t... Indices,
    std::size_t Offset>
constexpr void indexed_foreach(
    const std::tuple<Ts...>& tuple,
    Visitor&&                visitor,
    std::index_sequence<Indices...>,
    tinyrefl::meta::size_t<Offset>)
{
    (void)std::array<int, sizeof...(Ts)>{{(
        visitor(
            std::get<Indices + Offset>(tuple),
            tinyrefl::type_constant<tinyrefl::meta::size_t<Indices + Offset>>),
        0)...}};
}

template<
    typename... Ts,
    typename Visitor,
    std::size_t... Indices,
    std::size_t Offset>
constexpr void indexed_foreach(
    std::tuple<Ts...>& tuple,
    Visitor&&          visitor,
    std::index_sequence<Indices...>,
    tinyrefl::meta::size_t<Offset>)
{
    (void)std::array<int, sizeof...(Ts)>{{(
        visitor(
            std::get<Indices + Offset>(tuple),
            tinyrefl::type_constant<tinyrefl::meta::size_t<Indices + Offset>>),
        0)...}};
}
} // namespace impl

template<typename... Ts, typename Visitor>
constexpr void foreach(const std::tuple<Ts...>& tuple, Visitor && visitor)
{
    ::tinyrefl::meta::impl::foreach(
        tuple,
        std::forward<Visitor>(visitor),
        std::index_sequence_for<Ts...>{});
}

template<typename... Ts, typename Visitor>
constexpr void foreach(std::tuple<Ts...>& tuple, Visitor && visitor)
{
    ::tinyrefl::meta::impl::foreach(
        tuple,
        std::forward<Visitor>(visitor),
        std::index_sequence_for<Ts...>{});
}

template<typename... Ts, typename Visitor>
constexpr void
    indexed_foreach(const std::tuple<Ts...>& tuple, Visitor&& visitor)
{
    ::tinyrefl::meta::impl::indexed_foreach(
        tuple,
        std::forward<Visitor>(visitor),
        std::index_sequence_for<Ts...>{},
        tinyrefl::meta::size_t<0>{});
}

template<typename... Ts, typename Visitor>
constexpr void indexed_foreach(std::tuple<Ts...>& tuple, Visitor&& visitor)
{
    ::tinyrefl::meta::impl::indexed_foreach(
        tuple,
        std::forward<Visitor>(visitor),
        std::index_sequence_for<Ts...>{},
        tinyrefl::meta::size_t<0>{});
}

template<typename... Ts, typename Visitor, std::size_t Begin, std::size_t End>
constexpr void indexed_foreach(
    const std::tuple<Ts...>& tuple,
    const std::pair<tinyrefl::meta::size_t<Begin>, tinyrefl::meta::size_t<End>>,
    Visitor&& visitor)
{
    static_assert(End - Begin <= sizeof...(Ts), "Range out of bounds");

    ::tinyrefl::meta::impl::indexed_foreach(
        tuple,
        std::forward<Visitor>(visitor),
        std::make_index_sequence<End - Begin>{},
        tinyrefl::meta::size_t<Begin>{});
}

template<typename... Ts, typename Visitor, std::size_t Begin, std::size_t End>
constexpr void indexed_foreach(
    std::tuple<Ts...>& tuple,
    const std::pair<tinyrefl::meta::size_t<Begin>, tinyrefl::meta::size_t<End>>,
    Visitor&& visitor)
{
    static_assert(End - Begin <= sizeof...(Ts), "Range out of bounds");

    ::tinyrefl::meta::impl::indexed_foreach(
        tuple,
        std::forward<Visitor>(visitor),
        std::make_index_sequence<End - Begin>{},
        tinyrefl::meta::size_t<Begin>{});
}

template<typename T>
struct type_to_value
{
    static constexpr T value = T{};
};

template<typename T>
constexpr T type_to_value<T>::value;

template<typename T, typename = void>
struct is_constexpr_default_constructible : public std::false_type
{
};

template<typename T>
struct is_constexpr_default_constructible<
    T,
    tinyrefl::meta::void_t<int[(T{}, 42)]>> : std::true_type
{
};

namespace impl
{

template<typename Seq, typename Out>
struct remove_duplicates;

template<
    typename Head,
    typename... Tail,
    typename... Out,
    template<typename...>
    class Seq>
struct remove_duplicates<Seq<Head, Tail...>, Seq<Out...>>
{
    using type = typename remove_duplicates<
        Seq<Tail...>,
        std::conditional_t<
            tinyrefl::meta::pack_contains<Head, Out...>::value,
            Seq<Out...>,
            Seq<Out..., Head>>>::type;
};

template<template<typename...> class Seq, typename... Out>
struct remove_duplicates<Seq<>, Seq<Out...>>
{
    using type = Seq<Out...>;
};

template<typename... Ts, typename Function, std::size_t... Indices>
constexpr auto tuple_map(
    const std::tuple<Ts...>& tuple,
    Function                 function,
    tinyrefl::meta::index_sequence<Indices...>)
{
    return std::forward_as_tuple(function(std::get<Indices>(tuple))...);
}

constexpr std::tuple<> tuple_cat()
{
    return {};
}

template<typename... Ts>
constexpr decltype(auto) tuple_cat(const std::tuple<Ts...>& tuple)
{
    return tuple;
}

template<typename... Head, typename... Tail>
constexpr auto tuple_cat(const std::tuple<Head...>& head, const Tail&&... tail)
{
    return std::tuple_cat(head, tuple_cat(tail...));
}

template<typename... Ts, std::size_t... Indices>
constexpr auto tuple_flat(
    const std::tuple<Ts...>& tuple, tinyrefl::meta::index_sequence<Indices...>)
{
    return tuple_cat(std::get<Indices>(tuple)...);
}

template<typename Function>
struct tuple_item_filter
{
    template<typename Item>
    constexpr auto operator()(const Item& item)
        -> std::enable_if_t<Function()(Item{}), std::tuple<Item>>
    {
        return {item};
    }

    template<typename Item>
    constexpr auto operator()(const Item& item)
        -> std::enable_if_t<not Function()(Item{}), std::tuple<>>
    {
        return {};
    }
};

struct tuple_filter_t
{
    constexpr tuple_filter_t() = default;

    template<typename Item, typename Function>
    constexpr auto operator()(const Item& item, Function function) const
    {
        constexpr bool result = function(Item{});
        return call(item, tinyrefl::meta::bool_<result>{});
    }

private:
    template<typename Item>
    constexpr std::tuple<Item>
        call(const Item& item, tinyrefl::meta::true_) const
    {
        return {item};
    }

    template<typename Item>
    constexpr std::tuple<> call(const Item& item, tinyrefl::meta::false_) const
    {
        return {};
    }
};

template<typename... Items, typename Function, std::size_t... Is>
constexpr auto tuple_filter(
    const std::tuple<Items...>& tuple,
    Function                    function,
    std::index_sequence<Is...>)
{
    return std::make_tuple(tuple_filter_t{}(std::get<Is>(tuple), function)...);
}

template<typename Function>
struct map
{
    constexpr map(Function function) : _function{std::move(function)} {}

    template<typename T>
    constexpr decltype(auto) operator()(const T& value) const
    {
        return _function(value);
    }

private:
    Function _function;
};

template<typename Predicate>
struct all_of
{
    constexpr all_of(Predicate predicate) : _predicate{std::move(predicate)} {}

    template<typename... Ts>
    constexpr bool operator()(const std::tuple<Ts...>& tuple) const
    {
        return invoke(tuple, std::index_sequence_for<Ts...>{});
    }

private:
    template<typename... Ts, std::size_t... Is>
    constexpr bool
        invoke(const std::tuple<Ts...>& tuple, std::index_sequence<Is...>) const
    {
        for(const bool value :
            {static_cast<bool>(_predicate(std::get<Is>(tuple)))...})
        {
            if(not value)
            {
                return false;
            }
        }

        return true;
    }

    Predicate _predicate;
};

template<typename Predicate>
struct any_of
{
    constexpr any_of(Predicate predicate) : _predicate{std::move(predicate)} {}

    template<typename... Ts>
    constexpr bool operator()(const std::tuple<Ts...>& tuple) const
    {
        return invoke(tuple, std::index_sequence_for<Ts...>{});
    }

private:
    template<typename... Ts, std::size_t... Is>
    constexpr bool
        invoke(const std::tuple<Ts...>& tuple, std::index_sequence<Is...>) const
    {
        for(const bool value :
            {static_cast<bool>(_predicate(std::get<Is>(tuple)))...})
        {
            if(value)
            {
                return true;
            }
        }

        return false;
    }

    Predicate _predicate;
};

struct bool_cast
{
    constexpr bool_cast() = default;

    template<typename T>
    constexpr bool operator()(const T& value) const
    {
        return static_cast<bool>(value);
    }
};

template<typename... Predicates>
struct all
{
    constexpr all(std::tuple<Predicates...> predicates)
        : _predicates{std::move(predicates)}
    {
    }

    template<typename T>
    constexpr bool operator()(const T& value) const
    {
        return invoke(value, std::index_sequence_for<Predicates...>{});
    }

private:
    std::tuple<Predicates...> _predicates;

    template<typename T, std::size_t... Is>
    constexpr bool invoke(const T& value, std::index_sequence<Is...>) const
    {
        for(const bool result :
            {static_cast<bool>((std::get<Is>(_predicates)(value)))...})
        {
            if(not result)
            {
                return false;
            }
        }

        return true;
    }
};

template<typename... Predicates>
struct any
{
    constexpr any(std::tuple<Predicates...> predicates)
        : _predicates{std::move(predicates)}
    {
    }

    template<typename T>
    constexpr bool operator()(const T& value) const
    {
        return invoke(value, std::index_sequence_for<Predicates...>{});
    }

private:
    std::tuple<Predicates...> _predicates;

    template<typename T, std::size_t... Is>
    constexpr bool invoke(const T& value, std::index_sequence<Is...>) const
    {
        for(const bool result :
            {static_cast<bool>((std::get<Is>(_predicates)(value)))...})
        {
            if(result)
            {
                return true;
            }
        }

        return false;
    }
};

template<typename Lhs, typename Rhs>
struct combine_t
{
    constexpr combine_t(Lhs lhs, Rhs rhs)
        : _lhs{std::move(lhs)}, _rhs{std::move(rhs)}
    {
    }

    template<typename... Args>
    constexpr decltype(auto) operator()(Args&&... args) const
    {
        const auto inner_result = _lhs(std::forward<Args>(args)...);
        return _rhs(inner_result);
    }

private:
    Lhs _lhs;
    Rhs _rhs;
};

template<typename Lhs, typename Rhs>
constexpr combine_t<std::decay_t<Lhs>, std::decay_t<Rhs>>
    combine(Lhs&& lhs, Rhs&& rhs)
{
    return {std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)};
}

template<typename Function>
constexpr std::decay_t<Function> combine(Function&& function)
{
    return std::forward<Function>(function);
}

template<typename... Ts, std::size_t... Indices>
constexpr auto typelist_to_tuple(
    tinyrefl::meta::list<Ts...>, tinyrefl::meta::index_sequence<Indices...>)
{
    return std::make_tuple(
        tinyrefl::type_tag<tinyrefl::meta::pack_get_t<Indices, Ts...>>{}...);
}
} // namespace impl

template<typename Seq>
using remove_duplicates = tinyrefl::meta::impl::
    remove_duplicates<Seq, tinyrefl::meta::functor_t<Seq>>;

template<typename Seq>
using remove_duplicates_t = typename remove_duplicates<Seq>::type;

template<typename... Ts>
using pack_remove_duplicates = remove_duplicates<tinyrefl::meta::list<Ts...>>;

template<typename... Ts>
using pack_remove_duplicates_t = typename pack_remove_duplicates<Ts...>::type;

template<typename... Ts>
constexpr std::size_t tuple_size(const std::tuple<Ts...>&)
{
    return sizeof...(Ts);
}

namespace hof
{

template<typename Function>
constexpr auto map(Function&& function)
{
    return impl::map<std::decay_t<Function>>{std::forward<Function>(function)};
}

template<typename Predicate>
constexpr impl::all_of<std::decay_t<Predicate>> all_of(Predicate&& predicate)
{
    return {std::forward<Predicate>(predicate)};
}

template<typename Predicate>
constexpr impl::any_of<std::decay_t<Predicate>> any_of(Predicate&& predicate)
{
    return {std::forward<Predicate>(predicate)};
}

template<typename... Predicates>
constexpr auto all(std::tuple<Predicates...> predicates)
{
    return impl::all<Predicates...>{std::move(predicates)};
}

template<typename... Predicates>
constexpr auto any(std::tuple<Predicates...> predicates)
{
    return impl::any<Predicates...>{std::move(predicates)};
}
} // namespace hof

template<typename... Ts, typename Function>
constexpr auto tuple_map(const std::tuple<Ts...>& tuple, Function function)
{
    return impl::tuple_map(
        tuple, function, tinyrefl::meta::make_index_sequence_for<Ts...>());
}

template<typename... Tuples>
constexpr auto tuple_cat(const Tuples&... tuples)
{
    return impl::tuple_cat(tuples...);
}

template<typename... Ts>
constexpr auto tuple_flat(const std::tuple<Ts...>& tuple)
{
    return impl::tuple_flat(
        tuple, tinyrefl::meta::make_index_sequence_for<Ts...>());
}

template<typename Function, typename... Ts>
constexpr auto tuple_flatmap(const std::tuple<Ts...>& tuple, Function function)
{
    return tuple_flat(tuple_map(tuple, function));
}

template<typename Function, typename... Items>
constexpr auto
    tuple_filter(const std::tuple<Items...>& tuple, Function function)
{
    return tuple_flat(impl::tuple_filter(
        tuple, function, std::index_sequence_for<Items...>{}));
}

template<typename... Ts, typename Predicate>
constexpr bool
    all_of(const std::tuple<Ts...>& tuple, const Predicate& predicate)
{
    return impl::all_of<Predicate>{predicate}(tuple);
}

template<typename... Ts, typename Predicate>
constexpr bool
    any_of(const std::tuple<Ts...>& tuple, const Predicate& predicate)
{
    return impl::any_of<Predicate>{predicate}(tuple);
}

template<typename... Ts>
constexpr bool all_of(const std::tuple<Ts...>& tuple)
{
    return all_of(tuple, impl::bool_cast{});
}

template<typename... Ts>
constexpr bool any_of(const std::tuple<Ts...>& tuple)
{
    return any_of(tuple, impl::bool_cast{});
}

template<typename Function>
constexpr std::decay_t<Function> combine(Function&& function)
{
    return std::forward<Function>(function);
}

template<typename Head, typename... Tail>
constexpr auto combine(Head&& head, Tail&&... tail)
{
    return impl::combine(
        std::forward<Head>(head), impl::combine(std::forward<Tail>(tail)...));
}

template<typename... Ts>
constexpr auto typelist_to_tuple(tinyrefl::meta::list<Ts...>)
{
    return impl::typelist_to_tuple(
        tinyrefl::meta::list<Ts...>{},
        tinyrefl::meta::make_index_sequence_for<Ts...>{});
}

namespace impl
{

template<template<typename...> class Op, typename Types, typename = void>
struct detector : std::false_type
{
};

template<template<typename...> class Op, typename... Types>
struct detector<
    Op,
    tinyrefl::meta::list<Types...>,
    tinyrefl::meta::void_t<Op<Types...>>> : std::true_type
{
};

} // namespace impl

template<template<typename...> class Op, typename... Types>
using is_detected =
    tinyrefl::meta::impl::detector<Op, tinyrefl::meta::list<Types...>>;

template<template<typename...> class Op, typename... Types>
constexpr bool is_detected_v = is_detected<Op, Types...>::value;

} // namespace meta

} // namespace tinyrefl

#endif // TINYREFL_UTILS_META_HPP_INCLUDED
