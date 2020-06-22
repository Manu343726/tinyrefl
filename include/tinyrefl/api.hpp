#ifndef TINYREFL_API_HPP
#define TINYREFL_API_HPP

#include <algorithm>
#include <cassert>
#include <sstream>

#include <tinyrefl/backend/backend.hpp>
#include <tinyrefl/backend/metadata.hpp>
#include <tinyrefl/object_visitor.hpp>
#include <tinyrefl/utils/language_features.hpp>
#include <tinyrefl/utils/meta.hpp>
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

template<typename First, typename Second, typename... Tail, typename Comparator>
constexpr bool tuple_memberwise_equal(
    const std::tuple<First, Second, Tail...>& tuple, Comparator comparator)
{
    bool equal = true;

    tinyrefl::meta::indexed_foreach(
        tuple,
        std::make_pair(
            tinyrefl::meta::size_t<0>{},
            tinyrefl::meta::size_t<sizeof...(Tail) + 1>{}),
        [&equal, tuple, comparator](const auto& item, auto i) {
            constexpr std::size_t index = decltype(i)::type::value;
            equal &= comparator(item, std::get<index + 1>(tuple));
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

template<typename Class>
auto make_tuple(const Class& object)
{
    using variables = typename tinyrefl::metadata<Class>::member_variables;

    return tinyrefl::meta::tuple_map(
        tinyrefl::meta::typelist_to_tuple(variables{}),
        [&object](auto type) -> decltype(auto) {
            constexpr typename decltype(type)::type member_metadata;
            return member_metadata.get(object);
        });
}

template<typename Class>
constexpr auto make_tuple(Class& object)
{
    using variables = typename tinyrefl::metadata<Class>::member_variables;

    return tinyrefl::meta::tuple_map(
        tinyrefl::meta::typelist_to_tuple(variables{}),
        [&object](auto type) -> decltype(auto) {
            constexpr typename decltype(type)::type member_metadata;
            return member_metadata.get(object);
        });
}

template<typename Class, typename... Ts>
Class make_object(const std::tuple<Ts...>& tuple)
{
    Class result;
    static_assert(
        tinyrefl::has_metadata<Class>(),
        "tinyrefl::make_object() can only be called with a class known by tinyrefl");

    tinyrefl::meta::indexed_foreach(
        tinyrefl::metadata<Class>()
            .template all_children<
                tinyrefl::entities::entity_kind::MEMBER_VARIABLE>(),
        [&tuple, &result](const auto& variable, auto index) {
            constexpr std::size_t Index = decltype(index)::type::value;
            variable.get(result) =
                std::get<Index>(tuple);
        });

    return result;
}

template<typename Enum>
constexpr auto enum_cast(const std::underlying_type_t<Enum> value)
    -> std::enable_if_t<
        std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
        Enum>
{
    return tinyrefl::metadata<Enum>()[value].value();
}

template<typename Enum>
constexpr auto enum_cast(const tinyrefl::string name) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
    Enum>
{
    return tinyrefl::metadata<Enum>()[name].value();
}

template<typename Enum, std::size_t N>
constexpr auto enum_cast(const char (&name)[N]) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
    Enum>
{
    return enum_cast<Enum>(tinyrefl::string{name});
}

template<typename Enum>
auto enum_cast(const std::string& name) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
    Enum>
{
    return enum_cast<Enum>(tinyrefl::string{name.c_str(), name.size()});
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
    tinyrefl::string>
{
    return tinyrefl::metadata<Enum>()[value].name();
}

namespace detail
{

template<typename T, typename = void>
struct has_adl_to_json : std::false_type
{
};

template<typename T>
struct has_adl_to_json<
    T,
    tinyrefl::meta::void_t<decltype(to_json(
        std::declval<nlohmann::json&>(), std::declval<T>()))>> : std::true_type
{
};

template<typename T, typename = void>
struct has_adl_from_json : std::false_type
{
};

template<typename T>
struct has_adl_from_json<
    T,
    tinyrefl::meta::void_t<decltype(from_json(
        std::declval<nlohmann::json>(), std::declval<T&>()))>> : std::true_type
{
};

template<typename T, typename = void>
struct has_custom_to_json : has_adl_to_json<T>
{
};

template<typename T, typename = void>
struct has_custom_from_json : has_adl_from_json<T>
{
};

} // namespace detail

template<typename T>
auto to_json(const T& value) -> std::enable_if_t<
    !tinyrefl::has_metadata<T>() || detail::has_custom_to_json<T>::value,
    json>
{
    return value;
}

template<typename Enum>
auto to_json(const Enum value) -> std::enable_if_t<
    tinyrefl::has_metadata<Enum>() && std::is_enum<Enum>::value &&
        !detail::has_custom_to_json<Enum>::value,
    json>
{
    return tinyrefl::to_string(value).str();
}

template<typename Class>
auto to_json(const Class& object) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value &&
        !detail::has_custom_to_json<Class>::value,
    json>;

template<typename T>
auto from_json(const json& json) -> std::enable_if_t<
    !tinyrefl::has_metadata<T>() || detail::has_custom_from_json<T>::value,
    T>
{
    T result;
    result = json.get<T>();
    return result;
}

template<typename Enum>
auto from_json(const json& json) -> std::enable_if_t<
    tinyrefl::has_metadata<Enum>() && std::is_enum<Enum>::value &&
        !detail::has_custom_from_json<Enum>::value,
    Enum>
{
    assert(json.is_string());
    return tinyrefl::enum_cast<Enum>(json.get<std::string>());
}

template<typename Class>
auto from_json(const json& json) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value &&
        !detail::has_custom_from_json<Class>::value,
    Class>;

} // namespace tinyrefl

namespace tinyrefl
{

template<typename Class>
auto to_json(const Class& object) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value &&
        !detail::has_custom_to_json<Class>::value,
    json>
{
    auto result = json::object();

    visit_member_variables(
        object, [&result](const tinyrefl::string& name, const auto& member) {
            result[name.str()] = tinyrefl::to_json(member);
        });

    return result;
}

template<typename Class>
auto from_json(const json& json) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value &&
        !detail::has_custom_from_json<Class>::value,
    Class>
{
    Class result;

    visit_member_variables(
        result, [&json](const tinyrefl::string& name, auto& member) {
            member = tinyrefl::from_json<std::decay_t<decltype(member)>>(
                json[name.str()]);
        });

    return result;
}

template<typename Class>
auto to_string(const Class& object) -> std::enable_if_t<
    tinyrefl::has_metadata<Class>() && std::is_class<Class>::value,
    std::string>
{
    std::ostringstream ss;
    ss << tinyrefl::to_json(object);
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

    tinyrefl::visit_member_variables(
        std::forward_as_tuple(std::forward<Class>(objects)...),
        [&equal](const auto& /* name */, const auto&... entities) {
            equal &= detail::tuple_memberwise_equal(
                std::forward_as_tuple(entities...),
                [](const auto& lhs, const auto& rhs) {
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
