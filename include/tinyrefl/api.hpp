#ifndef TINYREFL_API_HPP
#define TINYREFL_API_HPP

#include <tinyrefl/backend.hpp>
#include <ctti/detailed_nameof.hpp>
#include <ctti/type_tag.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <masquerade.hpp>

namespace tinyrefl
{

enum class entity
{
    NAMESPACE,
    CLASS,
    BASE_CLASS,
    MEMBER_VARIABLE,
    MEMBER_FUNCTION,
    OBJECT
};

std::ostream& operator<<(std::ostream& os, const entity e)
{
    switch(e)
    {
    case entity::NAMESPACE:
        return os << "namespace";
    case entity::CLASS:
        return os << "class";
    case entity::BASE_CLASS:
        return os << "base_class";
    case entity::MEMBER_FUNCTION:
        return os << "member function";
    case entity::MEMBER_VARIABLE:
        return os << "member variable";
    case entity::OBJECT:
        return os << "object";
    }

    return os;
}

template<typename T>
using type_tag = ctti::type_tag<T>;

template<typename T>
using metadata = typename tinyrefl::backend::metadata_of<ctti::nameof<T>().hash()>::type;

template<typename T>
using has_metadata = tinyrefl::backend::metadata_registered_for_hash<
    ctti::nameof<T>().hash()
>;

template<typename T>
std::string default_object_name(const T& object)
{
    return fmt::format("<{} object @{:#x}>", ctti::nameof<T>(), reinterpret_cast<std::uintptr_t>(&object));
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


    template<typename... Args>
    auto operator()(Args&&... args) const -> typename std::enable_if<is_invokable<Head, tinyrefl::meta::list<Args&&...>>::value>::type
    {
        (*static_cast<const Head*>(this))(std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto operator()(Args&&... args) const -> typename std::enable_if<!is_invokable<Head, tinyrefl::meta::list<Args&&...>>::value>::type
    {
        // does nothing
    }

    template<typename... Args>
    auto operator()(Args&&... args) -> typename std::enable_if<is_invokable<Head, tinyrefl::meta::list<Args&&...>>::value>::type
    {
        (*static_cast<Head*>(this))(std::forward<Args>(args)...);
    }

    template<typename... Args>
    auto operator()(Args&&... args) -> typename std::enable_if<!is_invokable<Head, tinyrefl::meta::list<Args&&...>>::value>::type
    {
        // does nothing
    }
};

template<typename... Ts, typename Function, std::size_t... Indices>
auto tuple_map_impl(const std::tuple<Ts...>& tuple, Function function, tinyrefl::meta::index_sequence<Indices...>)
{
    return std::make_tuple(function(std::get<Indices>(tuple))...);
}

template<typename... Ts, typename Function>
auto tuple_map(const std::tuple<Ts...>& tuple, Function function)
{
    return tuple_map_impl(tuple, function, tinyrefl::meta::make_index_sequence_for<Ts...>());
}

template<typename Class, typename Visitor, std::size_t Depth>
tinyrefl::meta::enable_if_t<std::is_class<Class>::value && has_metadata<Class>::value>
visit_class(Visitor visitor, tinyrefl::meta::size_t<Depth>)
{
    // visit base classes first (if reflected)
    tinyrefl::meta::foreach<typename metadata<Class>::base_classes>([visitor](auto Base, auto Index)
    {
        using base_class = typename decltype(Base)::type;

        tinyrefl::detail::visit_class<base_class>(visitor, tinyrefl::meta::size_t<Depth + 1>());
    });

    visitor(ctti::nameof<Class>().str(),
            tinyrefl::meta::size_t<Depth>(),
            tinyrefl::type_tag<Class>(),
            CTTI_STATIC_VALUE(entity::CLASS)());

    tinyrefl::meta::foreach<typename metadata<Class>::members>([visitor](auto Member, auto Index)
    {
        using member = typename decltype(Member)::type;

        visitor(ctti::detailed_nameof<typename member::pointer_static_value>().name().str(),
                tinyrefl::meta::size_t<Depth>(),
                member(),
                CTTI_STATIC_VALUE(entity::MEMBER_VARIABLE)());
    });
}

template<typename Class, typename Visitor, std::size_t Depth>
tinyrefl::meta::enable_if_t<!std::is_class<Class>::value || !has_metadata<Class>::value>
visit_class(Visitor visitor, tinyrefl::meta::size_t<Depth>)
{
    visitor(ctti::nameof<Class>().str(),
            tinyrefl::meta::size_t<Depth>(),
            tinyrefl::type_tag<Class>(),
            CTTI_STATIC_VALUE(entity::CLASS)());
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
constexpr tinyrefl::detail::overloaded_function<tinyrefl::meta::list<Functions...>>
overloaded_function(Functions... functions)
{
    return tinyrefl::detail::overloaded_function<tinyrefl::meta::list<Functions...>>{functions...};
}

template<typename Class, typename... Visitors>
void visit_class(Visitors... visitors)
{
    tinyrefl::detail::visit_class<Class>(tinyrefl::overloaded_function(visitors...), tinyrefl::meta::size_t<0>());
}

template<typename Class, typename... Visitors>
void visit_object(Class&& object, Visitors... visitors)
{
    auto visitor = tinyrefl::overloaded_function(visitors...);

    visit_class<typename std::decay<Class>::type>(
        [&object = object, visitor = visitor](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(entity::CLASS))
    {
        visitor(name, depth, tinyrefl::detail::cast<typename decltype(entity)::type>(object), CTTI_STATIC_VALUE(entity::OBJECT)());
    },
        [&object = object, visitor = visitor](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(entity::MEMBER_VARIABLE))
    {
        visitor(name, depth, entity.get(object), CTTI_STATIC_VALUE(entity::MEMBER_VARIABLE)());
    });
}

template<typename... Class>
auto visit_objects(Class&&... objects)
{
    return [objects = std::make_tuple(std::forward<Class>(objects)...)](auto... visitors)
    {
        auto visitor = tinyrefl::overloaded_function(visitors...);

        visit_class<typename std::decay<tinyrefl::meta::pack_head_t<Class...>>::type>(
            [&objects = objects, visitor = visitor](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(entity::CLASS))
        {
            visitor(
                name,
                depth,
                tinyrefl::detail::tuple_map(objects, [entity](auto&& object){ return tinyrefl::detail::cast<typename decltype(entity)::type>(object); }),
                CTTI_STATIC_VALUE(entity::OBJECT)()
            );
        },
            [&objects = objects, visitor = visitor](const std::string& name, auto depth, auto entity, CTTI_STATIC_VALUE(entity::MEMBER_VARIABLE))
        {
            visitor(
                name,
                depth,
                tinyrefl::detail::tuple_map(objects, [entity](auto&& object){ return entity.get(object); }),
                CTTI_STATIC_VALUE(entity::MEMBER_VARIABLE)()
            );
        });
    };
}

}

#endif // TINYREFL_API_HPP
