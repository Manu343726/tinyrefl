#ifndef TINYREFL_API_HPP
#define TINYREFL_API_HPP

#include <tinyrefl/backend.hpp>
#include <ctti/detailed_nameof.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace tinyrefl
{

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

template<typename Class, typename Visitor>
tinyrefl::meta::enable_if_t<std::is_class<Class>::value && has_metadata<Class>::value>
visit_object(const Class& object, Visitor visitor)
{
    // visit base classes first (if reflected)
    tinyrefl::meta::foreach<typename metadata<Class>::base_classes>([&object, visitor](auto Base, auto Index)
    {
        using base_class = typename decltype(Base)::type;

        const auto& subobject = static_cast<const base_class&>(object);
        visit_object(subobject, visitor);
    });

    visitor(object);

    tinyrefl::meta::foreach<typename metadata<Class>::members>([&object, visitor](auto Member, auto Index)
    {
        using member = typename decltype(Member)::type;
        visitor(object.*member::value, ctti::detailed_nameof<typename member::pointer_static_value>().name().str());
    });
}

template<typename Class, typename Visitor>
tinyrefl::meta::enable_if_t<std::is_class<Class>::value && has_metadata<Class>::value>
visit_object(Class& object, Visitor visitor)
{
    // visit base classes first (if reflected)
    tinyrefl::meta::foreach<typename metadata<Class>::base_classes>([&object, visitor](auto Base, auto Index)
    {
        using base_class = typename decltype(Base)::type;

        auto& subobject = static_cast<base_class&>(object);
        visit_object(subobject, visitor);
    });

    visitor(object, default_object_name(object));

    tinyrefl::meta::foreach<typename metadata<Class>::members>([&object, visitor](auto Member, auto Index)
    {
        using member = typename decltype(Member)::type;
        visitor(object.*member::value, ctti::detailed_nameof<typename member::pointer_static_value>().name().str());
    });
}

template<typename Class, typename Visitor>
tinyrefl::meta::enable_if_t<!std::is_class<Class>::value || !has_metadata<Class>::value>
visit_object(const Class& object, Visitor visitor)
{
    visitor(object, default_object_name(object));
}

template<typename Class, typename Visitor>
tinyrefl::meta::enable_if_t<!std::is_class<Class>::value || !has_metadata<Class>::value>
visit_object(Class& object, Visitor visitor)
{
    visitor(object, default_object_name(object));
}

}

#endif // TINYREFL_API_HPP
