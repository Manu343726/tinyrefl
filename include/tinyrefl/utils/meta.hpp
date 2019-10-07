#ifndef TINYREFL_UTILS_META_HPP_INCLUDED
#define TINYREFL_UTILS_META_HPP_INCLUDED

#include <ctti/detail/meta.hpp>
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
    template<typename...> class Zipper,
    template<typename...> class... Sequences,
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
    template<typename...> class Seq>
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
