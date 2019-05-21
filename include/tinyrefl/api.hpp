#ifndef TINYREFL_API_HPP
#define TINYREFL_API_HPP

#include <algorithm>
#include <cassert>
#include <sstream>

#include <tinyrefl/backend/backend.hpp>
#include <tinyrefl/backend/metadata.hpp>
#include <tinyrefl/object_visitor.hpp>
#include <tinyrefl/utils/language_features.hpp>
#include <tinyrefl/utils/overloaded_function.hpp>
#include <tinyrefl/utils/typename.hpp>
#include <tinyrefl/visitor.hpp>

#include <ctti/detailed_nameof.hpp>
#include <ctti/type_tag.hpp>

#include <nlohmann/json.hpp>

namespace tinyrefl
{

using entity = tinyrefl::entities::entity_kind;

using json = nlohmann::json;

namespace detail
{

template<typename... Ts, typename Function, std::size_t... Indices>
auto tuple_map_impl(
    const std::tuple<Ts...>& tuple,
    Function                 function,
    tinyrefl::meta::index_sequence<Indices...>)
{
    return std::forward_as_tuple(function(std::get<Indices>(tuple))...);
}

template<typename... Ts, typename Function>
auto tuple_map(const std::tuple<Ts...>& tuple, Function function)
{
    return tuple_map_impl(
        tuple, function, tinyrefl::meta::make_index_sequence_for<Ts...>());
}

template<typename... Ts, std::size_t... Indices>
constexpr auto typelist_to_tuple_impl(
    tinyrefl::meta::list<Ts...>, tinyrefl::meta::index_sequence<Indices...>)
{
    return std::make_tuple(
        tinyrefl::type_tag<tinyrefl::meta::pack_get_t<Indices, Ts...>>{}...);
}

template<typename... Ts>
constexpr auto typelist_to_tuple(tinyrefl::meta::list<Ts...>)
{
    return typelist_to_tuple_impl(
        tinyrefl::meta::list<Ts...>{},
        tinyrefl::meta::make_index_sequence_for<Ts...>{});
}

template<typename First, typename Second, typename... Tail, typename Comparator>
constexpr bool tuple_memberwise_equal(
    const std::tuple<First, Second, Tail...>& tuple, Comparator comparator)
{
    bool equal = true;

    tinyrefl::meta::foreach<tinyrefl::meta::list<Second, Tail...>>(
        [&equal, tuple, comparator](auto /* type */, auto i) {
            constexpr std::size_t index = decltype(i)::type::value;
            equal &=
                comparator(std::get<index>(tuple), std::get<index + 1>(tuple));
        });

    return equal;
}

template<typename T, typename Comparator>
constexpr bool tuple_memberwise_equal(
    const std::tuple<T>& /* tuple */, Comparator /* comparator */)
{
    return true;
}

template<typename Comparator>
constexpr bool tuple_memberwise_equal(
    const std::tuple<>& /* tuple */, Comparator /* comparator */)
{
    return true;
}

template<typename T, typename = void>
struct is_range : public std::false_type
{
};

template<typename T>
struct is_range<
    T,
    tinyrefl::meta::void_t<decltype(
        std::end(std::declval<T>()) - std::begin(std::declval<T>()))>>
    : public std::true_type
{
};

template<typename T, typename = void>
struct is_comparable : public std::false_type
{
};

template<typename T>
struct is_comparable<
    T,
    tinyrefl::meta::void_t<decltype(std::declval<T>() == std::declval<T>())>>
    : public std::true_type
{
};

template<typename BaseClass, typename Class>
const BaseClass& cast(const Class& object)
{
    return static_cast<const BaseClass&>(object);
}

template<typename BaseClass, typename Class>
BaseClass& cast(Class& object)
{
    return static_cast<BaseClass&>(object);
}

struct sink_visitor
{
    template<typename... Args>
    void operator()(Args&&...) const
    {
    }
};
} // namespace detail

template<typename Class, typename... Visitors>
void visit_class(Visitors&&... visitors)
{
    tinyrefl::visit<Class>(std::forward<Visitors>(visitors)...);
}

template<typename Class>
auto make_tuple(const Class& object)
{
    using variables = typename tinyrefl::metadata<Class>::member_variables;

    return detail::tuple_map(
        detail::typelist_to_tuple(variables{}),
        [&object](auto type) -> decltype(auto) {
            constexpr typename decltype(type)::type member_metadata;
            return member_metadata.get(object);
        });
}

template<typename Class>
constexpr auto make_tuple(Class& object)
{
    using variables = typename tinyrefl::metadata<Class>::member_variables;

    return detail::tuple_map(
        detail::typelist_to_tuple(variables{}),
        [&object](auto type) -> decltype(auto) {
            constexpr typename decltype(type)::type member_metadata;
            return member_metadata.get(object);
        });
}

template<typename Class, typename... Ts>
Class make_object(const std::tuple<Ts...>& tuple)
{
    Class result;

    tinyrefl::meta::foreach<
        typename tinyrefl::metadata<Class>::member_variables>(
        [&tuple, &result](auto type, auto index) mutable {
            constexpr typename decltype(type)::type member_metadata;
            member_metadata.get(result) = std::get<index>(tuple);
        });

    return result;
}

template<typename Enum>
constexpr auto enum_cast(const std::underlying_type_t<Enum> value)
    -> std::enable_if_t<
        std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
        Enum>
{
    return tinyrefl::metadata<Enum>().get_value(value).value();
}

template<typename Enum>
constexpr auto enum_cast(const ctti::detail::cstring name) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
    Enum>
{
    return tinyrefl::metadata<Enum>().get_value(name).value();
}

template<typename Enum, std::size_t N>
constexpr auto enum_cast(const char (&name)[N]) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
    Enum>
{
    return enum_cast<Enum>(ctti::detail::cstring{name});
}

template<typename Enum>
auto enum_cast(const std::string& name) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
    Enum>
{
    return enum_cast<Enum>(ctti::detail::cstring{name.c_str(), name.size()});
}

template<typename Enum>
constexpr auto underlying_value(const Enum value) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
    std::underlying_type_t<Enum>>
{
    return tinyrefl::metadata<Enum>().get_value(value).underlying_value();
}

template<typename Enum>
constexpr auto to_string(const Enum value) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
    ctti::detail::cstring>
{
    return tinyrefl::metadata<Enum>()[value].name();
}

template<typename T>
auto to_json(const T& value)
    -> std::enable_if_t<!tinyrefl::has_metadata<T>(), const T&>
{
    return value;
}

template<typename Class>
auto to_json(const Class& object) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value,
    json>;

template<typename Enum>
auto to_json(const Enum value) -> std::enable_if_t<
    tinyrefl::has_metadata<Enum>() && std::is_enum<Enum>::value,
    json>
{
    return tinyrefl::to_string(value).str();
}

template<typename T>
auto from_json(const json& json)
    -> std::enable_if_t<!tinyrefl::has_metadata<T>(), T>
{
    T result;
    nlohmann::from_json(json, result);
    return result;
}

template<typename Enum>
auto from_json(const json& json) -> std::enable_if_t<
    tinyrefl::has_metadata<Enum>() && std::is_enum<Enum>::value,
    Enum>
{
    assert(json.is_string());
    return tinyrefl::enum_cast<Enum>(json.get<std::string>());
}

template<typename Class>
auto from_json(const json& json) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value,
    Class>;

} // namespace tinyrefl

namespace nlohmann
{

template<typename Class>
auto to_json(const Class& object) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value,
    json>
{
    return tinyrefl::to_json(object);
}

template<typename T>
auto from_json(const json& json, T& result)
    -> std::enable_if_t<tinyrefl::has_metadata<T>(), T>
{
    result = tinyrefl::from_json<T>(json);
}

} // namespace nlohmann

namespace tinyrefl
{

template<typename Class>
auto to_json(const Class& object) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value,
    json>
{
    auto result = json::object();

    visit_member_variables(
        object, [&result](const tinyrefl::string& name, const auto& member) {
            result[name.str()] = to_json(member);
        });

    return result;
}

template<typename Class>
auto from_json(const json& json) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value,
    Class>
{
    Class result;

    visit_member_variables(
        result, [&json](const std::string& name, auto& member) {
            member = from_json<std::decay_t<decltype(member)>>(json[name]);
        });

    return result;
}

template<typename Class>
auto to_string(const Class& object) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value,
    std::string>
{
    std::ostringstream ss;
    ss << to_json(object);
    return ss.str();
}

template<typename T>
constexpr auto equal(const T& lhs, const T& rhs)
    -> std::enable_if_t<detail::is_comparable<T>::value, bool>
{
    return lhs == rhs;
}

template<typename Range>
constexpr auto equal(const Range& lhs, const Range& rhs) -> std::enable_if_t<
    detail::is_range<Range>::value && !detail::is_comparable<Range>::value,
    bool>
{
    using std::begin;
    using std::end;

    return std::equal(
        begin(lhs),
        end(lhs),
        begin(rhs),
        end(rhs),
        [](const auto& lhs, const auto& rhs) {
            return tinyrefl::equal(lhs, rhs);
        });
}

template<typename... Class>
auto equal(Class&&... objects) -> std::enable_if_t<
    (!detail::is_comparable<
         std::decay_t<tinyrefl::meta::pack_head_t<Class...>>>::value &&
     (sizeof...(Class) >= 2)) &&
        tinyrefl::has_metadata<
            std::decay_t<tinyrefl::meta::pack_head_t<Class...>>>() &&
        std::is_class<
            std::decay_t<tinyrefl::meta::pack_head_t<Class...>>>::value,
    bool>;

template<typename... Class>
auto memberwise_equal(Class&&... objects) -> std::enable_if_t<
    (sizeof...(Class) >= 2) &&
        tinyrefl::has_metadata<
            std::decay_t<tinyrefl::meta::pack_head_t<Class...>>>() &&
        std::is_class<
            std::decay_t<tinyrefl::meta::pack_head_t<Class...>>>::value,
    bool>
{
    bool equal = true;

    visit_objects_member_variables(std::forward<Class>(objects)...)(
        [&equal](const auto& /* name */, const auto& entities) {
            equal &= detail::tuple_memberwise_equal(
                entities, [](const auto& lhs, const auto& rhs) {
                    return tinyrefl::equal(lhs, rhs);
                });
        });

    return equal;
}

template<typename... Class>
auto equal(Class&&... objects) -> std::enable_if_t<
    (!detail::is_comparable<
         std::decay_t<tinyrefl::meta::pack_head_t<Class...>>>::value &&
     (sizeof...(Class) >= 2)) &&
        tinyrefl::has_metadata<
            std::decay_t<tinyrefl::meta::pack_head_t<Class...>>>() &&
        std::is_class<
            std::decay_t<tinyrefl::meta::pack_head_t<Class...>>>::value,
    bool>
{
    return memberwise_equal(std::forward<Class>(objects)...);
}

template<typename... Class>
bool not_equal(Class&&... objects)
{
    return !equal(std::forward<Class>(objects)...);
}

} // namespace tinyrefl

#if !defined(TINYREFL_NO_REFLECTION_OPERATORS)
#define $(...) ::tinyrefl::metadata<__VA_ARGS__>
#define $$(...) ::tinyrefl::metadata<TINYREFL_STATIC_VALUE(__VA_ARGS__)>
#endif // TINYREFL_NO_REFLECTION_OPERATORS

#endif // TINYREFL_API_HPP
