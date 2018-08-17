#ifndef TINYREFL_EXAMPLES_RTTR_HPP
#define TINYREFL_EXAMPLES_RTTR_HPP

#include <tinyrefl/api.hpp>
#include <rttr/registration>

namespace tinyrefl
{

// Helper function to register a constructor given its argument types
template<typename Class, typename... ConstructorArgs>
void register_constructor(rttr::registration::class_<Class>& registration, tinyrefl::meta::list<ConstructorArgs...>)
{
    registration.template constructor<ConstructorArgs...>();
}

// Register types with no tinyrefl metadata
template<typename T>
auto register_rttr_type() -> std::enable_if_t<
    !tinyrefl::has_metadata<T>()
>
{
    // Does nothing
}

// Register the given enumeration values into the given enumeration
// Since enum value registration is done in one variadic call for all the values,
// we pass them as a variadic template we can unpack.
//
// EnumRegistration here is the result of calling
// rttr::registration::enumeration() or rttr::registration::class_<C>::enumeration()
template<typename EnumRegistration, typename... EnumValues>
void register_rttr_enum_values(EnumRegistration&& registration, tinyrefl::meta::list<EnumValues...>)
{
    registration(rttr::value(EnumValues{}.value_name().begin(), EnumValues{}.value())...);
}

// Register the given enumeration values into the given enumeration
//
// Pass a variadic pack of enum values to the overload that does the job
template<typename Enum, typename EnumRegistration>
auto register_rttr_enum(EnumRegistration&& registration) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>()
>
{
    register_rttr_enum_values(std::forward<EnumRegistration>(registration), typename tinyrefl::metadata<Enum>::values{});
}

// Register enumeration types into rttr
template<typename Enum>
auto register_rttr_type() -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>()
>
{
    register_rttr_enum<Enum>(rttr::registration::enumeration<Enum>(
        tinyrefl::metadata<Enum>().name().full_name().begin()));
}

template<typename Enum, typename Class>
auto register_rttr_member_enum(rttr::registration::class_<Class>& registration) -> std::enable_if_t<
    std::is_enum<Enum>::value && tinyrefl::has_metadata<Enum>()
>
{
    register_rttr_enum<Enum>(registration.template enumeration<Enum>(tinyrefl::metadata<Enum>().name().full_name().begin()));
}

template<typename Class>
auto register_rttr_type() -> std::enable_if_t<
    std::is_class<Class>::value && tinyrefl::has_metadata<Class>()
>
{
    static_assert(tinyrefl::has_metadata<Class>(),
        "Cannot register class, no tinyrefl metadata found");

    using metadata = tinyrefl::metadata<Class>;

    /*
     * IMPORTANT NOTE: Somehow RTTR expects all name arguments to have
     * global scope (full program lifetime), it seems that the API stores string_views
     * to the given strings directly.
     * To make sure everything works, we get all names from static reflection metadata
     */

    rttr::registration::class_<Class>registration{metadata::name.full_name().begin()};

    // Here we take each class member (functions, enums, variables, etc) and register
    // them accordingly into the rttr class
    tinyrefl::visit_class<Class>(
        [&registration](const std::string& name, auto depth, auto entity, TINYREFL_STATIC_VALUE(tinyrefl::entity::CONSTRUCTOR))
    {
        using metadata = decltype(entity);
        register_constructor(registration, typename metadata::args{});
    },  [&registration](auto name, auto depth, auto entity, TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        registration.property(entity.name.name().begin(), entity.get());
    },  [&registration](auto name, auto depth, auto entity, TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_FUNCTION))
    {
        registration.method(entity.name.name().begin(), entity.get());
    },  [&registration](auto name, auto depth, auto entity, TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_ENUM))
    {
        using type = typename decltype(entity)::type;
        register_rttr_member_enum<type>(registration);
    });
}

// Register multiple types at the same time
template<typename Types>
void register_rttr_types()
{
    tinyrefl::meta::foreach<Types>([](auto type, auto index)
    {
        register_rttr_type<typename decltype(type)::type>();
    });
}

} // namespace tinyrefl

#define TINYREFL_REGISTER_RTTR_TYPES(...)                                       \
    RTTR_REGISTRATION {                                                         \
        ::tinyrefl::register_rttr_types<::tinyrefl::meta::list<__VA_ARGS__>>(); \
    }

#endif // TINYREFL_EXAMPLES_RTTR_HPP
