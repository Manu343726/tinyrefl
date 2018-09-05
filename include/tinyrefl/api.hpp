#ifndef TINYREFL_API_HPP
#define TINYREFL_API_HPP

#include <algorithm>
#include <cassert>
#include <ctti/detailed_nameof.hpp>
#include <ctti/type_tag.hpp>
#include <nlohmann/json.hpp>
#include <tinyrefl/backend.hpp>
#include <tinyrefl/utils/typename.hpp>

#define TINYREFL_STATIC_VALUE(...) CTTI_STATIC_VALUE(__VA_ARGS__)

namespace tinyrefl
{

using entity = tinyrefl::backend::entity_kind;

template<typename T>
using type_tag = ctti::type_tag<T>;

template<typename T, T Value>
using static_value = ctti::static_value<T, Value>;

struct as_static_value
{
};

namespace detail
{

template<typename... Args>
struct SelectOverloadedMemberFunction
{
    constexpr SelectOverloadedMemberFunction() = default;

    template<typename R, typename Class>
    constexpr auto operator()(R (Class::*Ptr)(Args...)) const
    {
        return Ptr;
    }
};
}

template<typename... Args>
constexpr detail::SelectOverloadedMemberFunction<Args...> select_overload =
    detail::SelectOverloadedMemberFunction<Args...>{};

using json = nlohmann::json;

template<typename T>
using metadata = typename tinyrefl::backend::metadata_of_type<T>;

template<typename T>
constexpr bool has_metadata()
{
    return tinyrefl::backend::metadata_registered_for_type<T>::value;
}

template<typename Metadata>
constexpr bool has_attribute(
    const Metadata& metadata, const ctti::detail::cstring& attribute)
{
    return tinyrefl::backend::has_attribute(metadata, attribute);
}

template<typename T>
constexpr auto has_attribute(const ctti::detail::cstring& attribute)
    -> tinyrefl::meta::enable_if_t<has_metadata<T>(), bool>
{
    return tinyrefl::backend::has_attribute(metadata<T>{}, attribute);
}

template<typename T>
constexpr auto has_attribute(const ctti::detail::cstring & /* attribute */)
    -> tinyrefl::meta::enable_if_t<!has_metadata<T>(), bool>
{
    return false;
}

template<typename T>
std::string default_object_name(const T& object)
{
    return "<" << tinyrefl::utils::type_name<T>() << " object @" << &object
               << ">";
}

namespace detail
{

template<typename Functions>
struct overloaded_function;

template<typename Head, typename Second, typename... Tail>
struct overloaded_function<tinyrefl::meta::list<Head, Second, Tail...>>
    : public Head,
      public overloaded_function<tinyrefl::meta::list<Second, Tail...>>
{
    using Head::operator();
    using overloaded_function<tinyrefl::meta::list<Second, Tail...>>::
        operator();

    constexpr overloaded_function(Head head, Second second, Tail... tail)
        : Head{head},
          overloaded_function<tinyrefl::meta::list<Second, Tail...>>{second,
                                                                     tail...}
    {
    }
};

template<typename Function, typename Args, typename = void>
struct is_invokable : public std::false_type
{
};

template<typename Function, typename... Args>
struct is_invokable<
    Function,
    tinyrefl::meta::list<Args...>,
    tinyrefl::meta::void_t<typename std::result_of<Function(Args...)>::type>>
    : std::true_type
{
};

template<typename Head>
struct overloaded_function<tinyrefl::meta::list<Head>> : public Head
{
    constexpr overloaded_function(Head head) : Head{head}
    {
    }

    using Head::operator();
};

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

template<typename Class, typename Visitor, std::size_t Depth, entity ClassKind>
tinyrefl::meta::enable_if_t<
    !std::is_class<Class>::value || !has_metadata<Class>()>
    visit_class(
        Visitor visitor,
        tinyrefl::meta::size_t<Depth>,
        ctti::static_value<entity, ClassKind>)
{
    visitor(
        tinyrefl::utils::type_name<Class>(),
        tinyrefl::meta::size_t<Depth>(),
        tinyrefl::type_tag<Class>(),
        TINYREFL_STATIC_VALUE(ClassKind)());
}

template<typename Class, typename Visitor, std::size_t Depth, entity ClassKind>
tinyrefl::meta::enable_if_t<
    std::is_class<Class>::value && has_metadata<Class>()>
    visit_class(
        Visitor visitor,
        tinyrefl::meta::size_t<Depth>,
        ctti::static_value<entity, ClassKind>)
{
    // visit base classes first (if reflected)
    tinyrefl::meta::foreach<typename metadata<Class>::base_classes>(
        [visitor](auto Base, auto /* Index */) {
            using base_class = typename decltype(Base)::type;

            tinyrefl::detail::visit_class<base_class>(
                visitor,
                tinyrefl::meta::size_t<Depth + 1>(),
                TINYREFL_STATIC_VALUE(entity::BASE_CLASS)());
        });

    visitor(
        tinyrefl::utils::type_name<Class>(),
        tinyrefl::meta::size_t<Depth>(),
        tinyrefl::type_tag<Class>(),
        TINYREFL_STATIC_VALUE(ClassKind)());

    tinyrefl::meta::foreach<typename metadata<Class>::constructors>(
        [visitor](auto Ctor, auto /* Index */) {
            using ctor = typename decltype(Ctor)::type;

            visitor(
                ctor::name.str(),
                tinyrefl::meta::size_t<Depth>(),
                ctor(),
                TINYREFL_STATIC_VALUE(entity::CONSTRUCTOR)());
        });

    tinyrefl::meta::foreach<typename metadata<Class>::member_variables>(
        [visitor](auto Member, auto /* Index */) {
            using member = typename decltype(Member)::type;

            visitor(
                member::name.name().str(),
                tinyrefl::meta::size_t<Depth>(),
                member(),
                TINYREFL_STATIC_VALUE(entity::MEMBER_VARIABLE)());
        });

    tinyrefl::meta::foreach<typename metadata<Class>::member_functions>(
        [visitor](auto Member, auto /* Index */) {
            using member = typename decltype(Member)::type;

            visitor(
                member::name.name().str(),
                tinyrefl::meta::size_t<Depth>(),
                member(),
                TINYREFL_STATIC_VALUE(entity::MEMBER_FUNCTION)());
        });

    tinyrefl::meta::foreach<typename metadata<Class>::classes>(
        [visitor](auto class_, auto /* Index */) {
            using class_type = typename decltype(class_)::type;

            visitor(
                ctti::detailed_nameof<class_type>().name().str(),
                tinyrefl::meta::size_t<Depth>(),
                class_,
                TINYREFL_STATIC_VALUE(entity::MEMBER_CLASS)());
        });

    tinyrefl::meta::foreach<typename metadata<Class>::enums>(
        [visitor](auto enum_, auto /* Index */) {
            using enum_type = typename decltype(enum_)::type;

            visitor(
                ctti::detailed_nameof<enum_type>().name().str(),
                tinyrefl::meta::size_t<Depth>(),
                enum_,
                TINYREFL_STATIC_VALUE(entity::MEMBER_ENUM)());
        });
}

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
}

template<typename... Functions>
using overloaded_function_t =
    tinyrefl::detail::overloaded_function<tinyrefl::meta::list<Functions...>>;

template<typename... Functions>
auto overloaded_function(Functions... functions)
{
    return overloaded_function_t<Functions...>{functions...};
}

namespace detail
{

template<typename Function>
struct function_fallback
{
    template<typename... Args>
    constexpr auto operator()(Args&&... args) const
        -> std::enable_if_t<!tinyrefl::detail::is_invokable<
            Function,
            tinyrefl::meta::list<decltype(std::forward<Args>(args))...>>::value>
    {
    }
};
}

template<typename... Functions>
auto overloaded_function_default(Functions... functions)
{
    using overloaded_t = overloaded_function_t<Functions...>;
    return overloaded_function(
        functions..., tinyrefl::detail::function_fallback<overloaded_t>{});
}

template<typename Class, typename... Visitors>
void visit_class(Visitors... visitors)
{
    tinyrefl::detail::visit_class<Class>(
        tinyrefl::overloaded_function_default(visitors...),
        tinyrefl::meta::size_t<0>(),
        TINYREFL_STATIC_VALUE(entity::CLASS)());
}

template<typename Class, typename... Visitors>
void visit_object(const Class& object, Visitors... visitors)
{
    auto visitor = tinyrefl::overloaded_function_default(visitors...);

    visit_class<typename std::decay<Class>::type>(
        [&](const std::string& name,
            auto               depth,
            auto               entity,
            TINYREFL_STATIC_VALUE(tinyrefl::entity::BASE_CLASS)) {
            visitor(
                name,
                depth,
                tinyrefl::detail::cast<typename decltype(entity)::type>(object),
                TINYREFL_STATIC_VALUE(tinyrefl::entity::OBJECT)());
        },
        [&](const std::string& name,
            auto               depth,
            auto               entity,
            TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
            visitor(
                name,
                depth,
                entity.get(object),
                TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)());
        });
}

template<typename Class, typename... Visitors>
void visit_object(Class& object, Visitors... visitors)
{
    auto visitor = tinyrefl::overloaded_function_default(visitors...);

    visit_class<typename std::decay<Class>::type>(
        [&](const std::string& name,
            auto               depth,
            auto               entity,
            TINYREFL_STATIC_VALUE(tinyrefl::entity::BASE_CLASS)) {
            visitor(
                name,
                depth,
                tinyrefl::detail::cast<typename decltype(entity)::type>(object),
                TINYREFL_STATIC_VALUE(tinyrefl::entity::OBJECT)());
        },
        [&](const std::string& name,
            auto               depth,
            auto               entity,
            TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
            visitor(
                name,
                depth,
                entity.get(object),
                TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)());
        });
}

template<typename... Class, typename... Visitors>
auto visit_objects(const std::tuple<Class...>& objects, Visitors... visitors)
{
    auto visitor = tinyrefl::overloaded_function_default(visitors...);

    visit_class<
        typename std::decay<tinyrefl::meta::pack_head_t<Class...>>::type>(
        [&objects, visitor](
            const std::string& name,
            auto               depth,
            auto               entity,
            TINYREFL_STATIC_VALUE(tinyrefl::entity::BASE_CLASS)) {
            visitor(
                name,
                depth,
                tinyrefl::detail::tuple_map(
                    objects,
                    [](auto&& object) -> decltype(auto) {
                        return tinyrefl::detail::cast<typename decltype(
                            entity)::type>(
                            std::forward<decltype(object)>(object));
                    }),
                TINYREFL_STATIC_VALUE(tinyrefl::entity::OBJECT)());
        },
        [&objects, visitor](
            const std::string& name,
            auto               depth,
            auto               entity,
            TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
            visitor(
                name,
                depth,
                tinyrefl::detail::tuple_map(
                    objects,
                    [entity](auto&& object) -> decltype(auto) {
                        return entity.get(
                            std::forward<decltype(object)>(object));
                    }),
                TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)());
        });
}

template<typename... Class>
auto visit_objects(Class&&... objects)
{
    return [objects = std::forward_as_tuple(std::forward<Class>(objects)...)](
        auto... visitors)
    {
        return visit_objects(objects, visitors...);
    };
}

template<typename Class, typename... Visitors>
void visit_member_variables(const Class& object, Visitors... visitors)
{
    visit_object(
        object,
        [visitor = overloaded_function(visitors...)](
            const auto& name,
            auto /* depth */,
            const auto& member,
            TINYREFL_STATIC_VALUE(entity::MEMBER_VARIABLE)) {
            visitor(name, member);
        });
}

template<typename Class, typename... Visitors>
void visit_member_variables(Class& object, Visitors... visitors)
{
    visit_object(
        object,
        [visitor = overloaded_function(visitors...)](
            const auto& name,
            auto /* depth */,
            auto& member,
            TINYREFL_STATIC_VALUE(entity::MEMBER_VARIABLE)) {
            visitor(name, member);
        });
}

template<typename... Class>
auto visit_objects_member_variables(Class&&... objects)
{
    return [objects = std::forward_as_tuple(std::forward<Class>(objects)...)](
        auto... visitors)
    {
        auto make_visitor = [](auto visitor) {
            return [visitor](
                const auto& name,
                auto /* depth */,
                auto&& entities,
                TINYREFL_STATIC_VALUE(entity::MEMBER_VARIABLE)) {
                visitor(name, entities);
            };
        };

        return visit_objects(objects, make_visitor(visitors)...);
    };
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
    return tinyrefl::metadata<Enum>().get_value(value).name();
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
        object, [&result](const std::string& name, const auto& member) {
            result[name] = to_json(member);
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
    (sizeof...(Class) >= 2) && tinyrefl::has_metadata<std::decay_t<
                                   tinyrefl::meta::pack_head_t<Class...>>>() &&
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
