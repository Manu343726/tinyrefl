#ifndef TINYREFL_API_HPP
#define TINYREFL_API_HPP

#include <tinyrefl/backend.hpp>
#include <ctti/type_tag.hpp>
#include <ctti/detailed_nameof.hpp>
#include <tinyrefl/utils/typename.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <masquerade.hpp>

namespace tinyrefl
{

using entity = tinyrefl::backend::entity_kind;


template<typename T>
using type_tag = ctti::type_tag<T>;

template<typename T>
using metadata = typename tinyrefl::backend::metadata_of_type<T>;

template<typename T>
using has_metadata = tinyrefl::backend::metadata_registered_for_type<T>;

template<typename Metadata>
constexpr bool has_attribute(const Metadata& metadata, const ctti::detail::cstring& attribute)
{
    return tinyrefl::backend::has_attribute(metadata, attribute);
}

template<typename T>
constexpr auto has_attribute(const ctti::detail::cstring& attribute) -> tinyrefl::meta::enable_if_t<
    has_metadata<T>::value, bool
>
{
    return tinyrefl::backend::has_attribute(metadata<T>{}, attribute);
}

template<typename T>
constexpr auto has_attribute(const ctti::detail::cstring& attribute) -> tinyrefl::meta::enable_if_t<
    !has_metadata<T>::value, bool
>
{
    return false;
}

template<typename T>
std::string default_object_name(const T& object)
{
    return fmt::format("<{} object @{:#x}>", tinyrefl::utils::type_name<T>(), reinterpret_cast<std::uintptr_t>(&object));
}

namespace detail
{

template<typename Functions>
struct overloaded_function;

template<typename Head, typename Second, typename... Tail>
struct overloaded_function<tinyrefl::meta::list<Head, Second, Tail...>> : public Head, public
    overloaded_function<tinyrefl::meta::list<Second, Tail...>>
{
    using Head::operator();
    using overloaded_function<tinyrefl::meta::list<Second, Tail...>>::operator();

    constexpr overloaded_function(Head head, Second second, Tail... tail) :
        Head{head},
        overloaded_function<tinyrefl::meta::list<Second, Tail...>>{second, tail...}
    {}
};

template<typename Function, typename Args, typename = void>
struct is_invokable : public std::false_type {};

template<typename Function, typename... Args>
struct is_invokable<Function, tinyrefl::meta::list<Args...>, tinyrefl::meta::void_t<typename std::result_of<Function(Args...)>::type>> : std::true_type {};

template<typename Head>
struct overloaded_function<tinyrefl::meta::list<Head>> : public Head
{
    constexpr overloaded_function(Head head) :
        Head{head}
    {}

    using Head::operator();
};

template<typename... Ts, typename Function, std::size_t... Indices>
auto tuple_map_impl(const std::tuple<Ts...>& tuple, Function function, tinyrefl::meta::index_sequence<Indices...>)
{
    return std::forward_as_tuple(function(std::get<Indices>(tuple))...);
}

template<typename... Ts, typename Function>
auto tuple_map(const std::tuple<Ts...>& tuple, Function function)
{
    return tuple_map_impl(tuple, function, tinyrefl::meta::make_index_sequence_for<Ts...>());
}

template<typename... Ts, std::size_t... Indices>
constexpr auto typelist_to_tuple_impl(tinyrefl::meta::list<Ts...>, tinyrefl::meta::index_sequence<Indices...>)
{
    return std::make_tuple(tinyrefl::type_tag<tinyrefl::meta::pack_get_t<Indices, Ts...>>{}...);
}

template<typename... Ts>
constexpr auto typelist_to_tuple(tinyrefl::meta::list<Ts...>)
{
    return typelist_to_tuple_impl(tinyrefl::meta::list<Ts...>{}, tinyrefl::meta::make_index_sequence_for<Ts...>{});
}

template<typename Class, typename Visitor, std::size_t Depth, entity ClassKind>
tinyrefl::meta::enable_if_t<!std::is_class<Class>::value || !has_metadata<Class>::value>
visit_class(Visitor visitor, tinyrefl::meta::size_t<Depth>, ctti::static_value<entity, ClassKind>)
{
    visitor(tinyrefl::utils::type_name<Class>(),
            tinyrefl::meta::size_t<Depth>(),
            tinyrefl::type_tag<Class>(),
            CTTI_STATIC_VALUE(ClassKind)());
}

template<typename Class, typename Visitor, std::size_t Depth, entity ClassKind>
tinyrefl::meta::enable_if_t<std::is_class<Class>::value && has_metadata<Class>::value>
visit_class(Visitor visitor, tinyrefl::meta::size_t<Depth>, ctti::static_value<entity, ClassKind>)
{
    // visit base classes first (if reflected)
    tinyrefl::meta::foreach<typename metadata<Class>::base_classes>([visitor](auto Base, auto /* Index */)
    {
        using base_class = typename decltype(Base)::type;

        tinyrefl::detail::visit_class<base_class>(visitor, tinyrefl::meta::size_t<Depth + 1>(), CTTI_STATIC_VALUE(entity::BASE_CLASS)());
    });

    visitor(tinyrefl::utils::type_name<Class>(),
            tinyrefl::meta::size_t<Depth>(),
            tinyrefl::type_tag<Class>(),
            CTTI_STATIC_VALUE(ClassKind)());

    tinyrefl::meta::foreach<typename metadata<Class>::member_variables>([visitor](auto Member, auto /* Index */)
    {
        using member = typename decltype(Member)::type;

        visitor(member::name.name().str(),
                tinyrefl::meta::size_t<Depth>(),
                member(),
                CTTI_STATIC_VALUE(entity::MEMBER_VARIABLE)());
    });

    tinyrefl::meta::foreach<typename metadata<Class>::member_functions>([visitor](auto Member, auto /* Index */)
    {
        using member = typename decltype(Member)::type;

        visitor(member::name.name().str(),
                tinyrefl::meta::size_t<Depth>(),
                member(),
                CTTI_STATIC_VALUE(entity::MEMBER_FUNCTION)());
    });

    tinyrefl::meta::foreach<typename metadata<Class>::classes>([visitor](auto class_, auto /* Index */)
    {
        using class_type = typename decltype(class_)::type;

        visitor(ctti::detailed_nameof<class_type>().name().str(),
                tinyrefl::meta::size_t<Depth>(),
                class_,
                CTTI_STATIC_VALUE(entity::MEMBER_CLASS)());
    });

    tinyrefl::meta::foreach<typename metadata<Class>::enums>([visitor](auto enum_, auto /* Index */)
    {
        using enum_type = typename decltype(enum_)::type;

        visitor(ctti::detailed_nameof<enum_type>().name().str(),
                tinyrefl::meta::size_t<Depth>(),
                enum_,
                CTTI_STATIC_VALUE(entity::MEMBER_ENUM)());
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
    void operator()(Args&&...) const {}
};

}

template<typename... Functions>
using overloaded_function_t = tinyrefl::detail::overloaded_function<
    tinyrefl::meta::list<Functions...>
>;

template<typename... Functions>
auto overloaded_function(Functions... functions)
{
    return overloaded_function_t<Functions...>{functions...};
}

template<typename... Functions>
auto overloaded_function_default(Functions... functions)
{
    using overloaded_t = overloaded_function_t<Functions...>;
    return overloaded_function(
        functions...,
        [](auto&&... args) -> tinyrefl::meta::enable_if_t<
            !tinyrefl::detail::is_invokable<overloaded_t, tinyrefl::meta::list<decltype(args)...>>::value
        >{}
    );
}

template<typename Class, typename... Visitors>
void visit_class(Visitors... visitors)
{
    tinyrefl::detail::visit_class<Class>(tinyrefl::overloaded_function_default(visitors...), tinyrefl::meta::size_t<0>(), CTTI_STATIC_VALUE(entity::CLASS)());
}

template<typename Class, typename... Visitors>
void visit_object(const Class& object, Visitors... visitors)
{
    auto visitor = tinyrefl::overloaded_function_default(visitors...);

    visit_class<typename std::decay<Class>::type>(
        [&](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(tinyrefl::entity::BASE_CLASS))
    {
        visitor(name, depth, tinyrefl::detail::cast<typename decltype(entity)::type>(object), CTTI_STATIC_VALUE(tinyrefl::entity::OBJECT)());
    },
        [&](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        visitor(name, depth, entity.get(object), CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)());
    });
}

template<typename Class, typename... Visitors>
void visit_object(Class& object, Visitors... visitors)
{
    auto visitor = tinyrefl::overloaded_function_default(visitors...);

    visit_class<typename std::decay<Class>::type>(
        [&](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(tinyrefl::entity::BASE_CLASS))
    {
        visitor(name, depth, tinyrefl::detail::cast<typename decltype(entity)::type>(object), CTTI_STATIC_VALUE(tinyrefl::entity::OBJECT)());
    },
        [&](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        visitor(name, depth, entity.get(object), CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)());
    });
}

template<typename... Class>
auto visit_objects(Class&... objects)
{
    return [objects = std::forward_as_tuple(objects...)](auto... visitors)
    {
        auto visitor = tinyrefl::overloaded_function_default(visitors...);

        visit_class<typename std::decay<tinyrefl::meta::pack_head_t<Class...>>::type>(
            [&objects, visitor](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(tinyrefl::entity::BASE_CLASS))
        {
            visitor(
                name,
                depth,
                tinyrefl::detail::tuple_map(objects, [entity](auto& object) -> decltype(auto) { return tinyrefl::detail::cast<typename decltype(entity)::type>(object); }),
                CTTI_STATIC_VALUE(tinyrefl::entity::OBJECT)()
            );
        },
            [&objects, visitor](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
        {
            visitor(
                name,
                depth,
                tinyrefl::detail::tuple_map(objects, [entity](auto& object) -> decltype(auto) { return entity.get(object); }),
                CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)()
            );
        });
    };
}

template<typename... Class>
auto visit_objects(const Class&... objects)
{
    return [objects = std::forward_as_tuple(objects...)](auto... visitors)
    {
        auto visitor = tinyrefl::overloaded_function_default(visitors...);

        visit_class<typename std::decay<tinyrefl::meta::pack_head_t<Class...>>::type>(
            [&objects, visitor](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(tinyrefl::entity::BASE_CLASS))
        {
            visitor(
                name,
                depth,
                tinyrefl::detail::tuple_map(objects, [entity](const auto& object) -> decltype(auto) { return tinyrefl::detail::cast<typename decltype(entity)::type>(object); }),
                CTTI_STATIC_VALUE(tinyrefl::entity::OBJECT)()
            );
        },
            [&objects, visitor](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
        {
            visitor(
                name,
                depth,
                tinyrefl::detail::tuple_map(objects, [entity](const auto& object) -> decltype(auto) { return entity.get(object); }),
                CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)()
            );
        });
    };
}

template<typename Class, typename... Visitors>
void visit_member_variables(const Class& object, Visitors... visitors)
{
    visit_object(object, [visitor = overloaded_function(visitors...)](const auto& name, auto /* depth */, const auto& member, CTTI_STATIC_VALUE(entity::MEMBER_VARIABLE))
    {
        visitor(name, member);
    });
}

template<typename Class, typename... Visitors>
void visit_member_variables(Class& object, Visitors... visitors)
{
    visit_object(object, [visitor = overloaded_function(visitors...)](const auto& name, auto /* depth */, auto& member, CTTI_STATIC_VALUE(entity::MEMBER_VARIABLE))
    {
        visitor(name, member);
    });
}

template<typename Class>
auto make_tuple(const Class& object)
{
    using variables = typename tinyrefl::metadata<Class>::member_variables;

    return detail::tuple_map(detail::typelist_to_tuple(variables{}), [&object](auto type) -> decltype(auto)
    {
        constexpr typename decltype(type)::type member_metadata;
        return member_metadata.get(object);
    });
}

template<typename Class>
constexpr auto make_tuple(Class& object)
{
    using variables = typename tinyrefl::metadata<Class>::member_variables;

    return detail::tuple_map(detail::typelist_to_tuple(variables{}), [&object](auto type) -> decltype(auto)
    {
        constexpr typename decltype(type)::type member_metadata;
        return member_metadata.get(object);
    });
}

template<typename Class, typename... Ts>
Class make_object(const std::tuple<Ts...>& tuple)
{
    Class result;

    tinyrefl::meta::foreach<typename tinyrefl::metadata<Class>::member_variables>([&tuple, &result] (auto type, auto index) mutable
    {
        constexpr typename decltype(type)::type member_metadata;
        member_metadata.get(result) = std::get<index>(tuple);
    });

    return result;
}

template<typename Enum>
constexpr auto enum_cast(const std::underlying_type_t<Enum> value) ->
    std::enable_if_t<
        std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
        Enum
    >
{
    return tinyrefl::metadata<Enum>().get_value(value).value();
}

template<typename Enum>
constexpr auto enum_cast(const ctti::detail::cstring name) ->
    std::enable_if_t<
        std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
        Enum
    >
{
    return tinyrefl::metadata<Enum>().get_value(name).value();
}

template<typename Enum>
auto enum_cast(const std::string& name) ->
    std::enable_if_t<
        std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
        Enum
    >
{
    return enum_cast<Enum>(name.c_str());
}

template<typename Enum>
constexpr auto underlying_value(const Enum value) ->
    std::enable_if_t<
        std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
        std::underlying_type_t<Enum>
    >
{
    return tinyrefl::metadata<Enum>().get_value(value).underlying_value();
}

template<typename Enum>
constexpr auto to_string(const Enum value) ->
    std::enable_if_t<
        std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>(),
        ctti::detail::cstring
    >
{
    return tinyrefl::metadata<Enum>().get_value(value).name();
}

}

#if !defined(TINYREFL_NO_REFLECTION_OPERATORS)
#define $(...) ::tinyrefl::metadata<__VA_ARGS__>
#define $$(...) ::tinyrefl::metadata<CTTI_STATIC_VALUE(__VA_ARGS__)>
#endif // TINYREFL_NO_REFLECTION_OPERATORS

#endif // TINYREFL_API_HPP
