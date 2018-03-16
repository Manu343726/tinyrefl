#ifndef TINYREFL_BACKEND_HPP
#define TINYREFL_BACKEND_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/cstring.hpp>
#include <ctti/detail/hash.hpp>
#include <ctti/detailed_nameof.hpp>
#include <ctti/symbol.hpp>
#include <ctti/detail/algorithm.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <stdexcept>
#include <initializer_list>

namespace tinyrefl
{

namespace meta = ctti::meta;

namespace backend
{

using type_hash_t = ctti::detail::hash_t;

struct no_metadata {};

template<type_hash_t Hash>
struct metadata_of
{
    using type = no_metadata;
};

template<type_hash_t Hash, typename = void>
struct metadata_registered_for_hash : public tinyrefl::meta::bool_<!std::is_same<
    typename metadata_of<Hash>::type,
    no_metadata
>::value> {};

template<typename T>
using metadata_of_type = metadata_of<ctti::nameof<T>().hash()>;

template<typename T>
using metadata_registered_for_type = metadata_registered_for_hash<
    ctti::nameof<T>().hash()
>;

enum class entity_kind
{
    NAMESPACE,
    CLASS,
    ENUM,
    BASE_CLASS,
    MEMBER_VARIABLE,
    MEMBER_CLASS,
    MEMBER_FUNCTION,
    MEMBER_ENUM,
    OBJECT
};

std::ostream& operator<<(std::ostream& os, const entity_kind e)
{
    switch(e)
    {
    case entity_kind::NAMESPACE:
        return os << "namespace";
    case entity_kind::CLASS:
        return os << "class";
    case entity_kind::ENUM:
        return os << "enum";
    case entity_kind::BASE_CLASS:
        return os << "base_class";
    case entity_kind::MEMBER_FUNCTION:
        return os << "member function";
    case entity_kind::MEMBER_VARIABLE:
        return os << "member variable";
    case entity_kind::MEMBER_CLASS:
        return os << "member class";
    case entity_kind::MEMBER_ENUM:
        return os << "member enum";
    case entity_kind::OBJECT:
        return os << "object";
    }

    return os;
}

template<const ctti::detail::cstring& Name, typename Pointer>
struct member : public Pointer
{
    using pointer_type = typename Pointer::value_type;
    using pointer_static_value = Pointer;
    static constexpr entity_kind kind = (std::is_member_function_pointer<pointer_type>::value ? entity_kind::MEMBER_FUNCTION : entity_kind::MEMBER_VARIABLE);
    static constexpr ctti::name_t name = Name;

    constexpr member() = default;

    constexpr pointer_type get() const
    {
        return Pointer::value;
    }

    template<typename Class>
    constexpr auto get(const Class& object) const -> decltype(object.*Pointer::value)
    {
        return object.*get();
    }

    template<typename Class>
    constexpr auto get(Class& object) const -> decltype(object.*Pointer::value)
    {
        return object.*get();
    }

    template<typename Class, typename... Args>
    constexpr auto get(const Class& object, Args&&... args) const -> decltype((object.*Pointer::value)(std::forward<Args>(args)...))
    {
        return (object.*get())(std::forward<Args>(args)...);
    }

    template<typename Class, typename... Args>
    constexpr auto get(Class& object, Args&&... args) const -> decltype((object.*Pointer::value)(std::forward<Args>(args)...))
    {
        return (object.*get())(std::forward<Args>(args)...);
    }
};

template<typename Signature>
struct constructor;

template<typename Class, typename... Args>
struct constructor<Class(Args...)>
{
    using class_type = Class;
    using args = tinyrefl::meta::list<Args...>;
    using decayed_args = tinyrefl::meta::list<tinyrefl::meta::decay_t<Args>...>;

    constexpr constructor() = default;

    constexpr auto construct(Args... args) const -> tinyrefl::meta::enable_if_t<
        std::is_move_constructible<Class>::value || std::is_copy_constructible<Class>::value,
        Class
    >
    {
        return Class{std::forward<Args>(args)...};
    }

    void inplace_construct(void* where, Args... args) const
    {
        new(where) Class{std::forward<Args>(args)...};
    }

    void inplace_construct(Class* where, Args... args) const
    {
        new(where) Class{std::move(args)...};
    }
};

template<
    typename BaseClasses,
    typename Members,
    typename Classes,
    typename Enums
>
struct class_
{
    static constexpr entity_kind kind = entity_kind::CLASS;
    using base_classes = BaseClasses;
    using members = tinyrefl::meta::fmap_t<tinyrefl::meta::defer<metadata_of_type>, Members>;
    using classes = Classes;
    using enums = Enums;

private:
    template<typename Total, typename BaseClass,
        typename HasMetadata = tinyrefl::meta::bool_<metadata_registered_for_type<BaseClass>::value>>
    struct accumulate
    {
        using type = Total;
    };

    template<typename Total, typename BaseClass>
    struct accumulate<Total, BaseClass, std::true_type>
    {
        using type = tinyrefl::meta::size_t<Total::value + metadata_of_type<BaseClass>::type::members::size + metadata_of_type<BaseClass>::type::total_members::value>;
    };

public:
    using total_base_members = tinyrefl::meta::foldl_t<tinyrefl::meta::defer<accumulate>, tinyrefl::meta::size_t<0>, base_classes>;
    using total_members = tinyrefl::meta::size_t<members::size + total_base_members::value>;
};

template<typename T, std::size_t N>
struct constexpr_array
{
    constexpr constexpr_array(const std::initializer_list<T>& init) :
        constexpr_array{init, tinyrefl::meta::make_index_sequence<N>()}
    {}

    constexpr const T& operator[](std::size_t i) const
    {
        return _array[i];
    }

    constexpr const T* begin() const
    {
        return ctti::detail::begin(_array);
    }

    constexpr const T* end() const
    {
        return ctti::detail::end(_array);
    }

    constexpr std::size_t size() const
    {
        return N;
    }

private:
    T _array[N];

    template<std::size_t... Is>
    constexpr constexpr_array(const std::initializer_list<T>& init, tinyrefl::meta::index_sequence<Is...>) :
        _array{*(init.begin() + Is)...}
    {}
};

template<typename List>
struct typelist_to_array;

template<typename... Values>
struct typelist_to_array<tinyrefl::meta::list<Values...>>
{
    using value_type = decltype(tinyrefl::meta::pack_head_t<Values...>::value);
    using array_type = constexpr_array<value_type, ctti::detail::max(1ul, sizeof...(Values))>;

    static constexpr array_type value = {Values::value...};
};

template<typename... Values>
constexpr typename typelist_to_array<tinyrefl::meta::list<Values...>>::array_type typelist_to_array<tinyrefl::meta::list<Values...>>::value;

template<typename Enum, typename Values>
struct enum_;

template<typename Enum, Enum... Values>
struct enum_<Enum, tinyrefl::meta::list<ctti::static_value<Enum, Values>...>>
{
    static constexpr entity_kind kind = entity_kind::CLASS;
    using values = tinyrefl::meta::list<ctti::static_value<Enum, Values>...>;
    using names_array_t = constexpr_array<ctti::detail::cstring, ctti::detail::max(1ul, values::size)>;
    using enum_type = Enum;
    using underlying_type = typename std::underlying_type<enum_type>::type;
    using underlying_values = tinyrefl::meta::list<ctti::static_value<underlying_type, static_cast<underlying_type>(Values)>...>;

    constexpr enum_() = default;

    constexpr std::size_t count() const
    {
        return values::size;
    }

    constexpr enum_type get_value(const ctti::detail::cstring& name) const
    {
        return find_value_by_name(name);
    }

    constexpr enum_type get_value(const std::size_t i) const
    {
        return get_values()[i];
    }

    constexpr underlying_type get_underlying_value(const ctti::detail::cstring& name) const
    {
        return static_cast<underlying_type>(get_value(name));
    }

    constexpr underlying_type get_underlying_value(std::size_t i) const
    {
        return static_cast<underlying_type>(get_value(i));
    }

    constexpr bool is_enumerated_value(const underlying_type value) const
    {
        return find_value_index(value) >= 0;
    }

    constexpr ctti::detail::cstring get_name(std::size_t i) const
    {
        return get_names()[i];
    }

    constexpr ctti::detail::cstring get_name(const enum_type value) const
    {
        return find_name_by_value(value);
    }

    constexpr auto get_values() const
    {
        return typelist_to_array<values>::value;
    }

    constexpr names_array_t get_names() const
    {
        return { ctti::detailed_nameof<CTTI_STATIC_VALUE(Values)>().name()... };
    }

    constexpr auto get_underlying_values() const
    {
        return typelist_to_array<underlying_values>::value;
    }

private:
    constexpr enum_type find_value_by_name(const ctti::detail::cstring& name, std::size_t i = 0) const
    {
        return (i < count()) ?
            (get_name(i) == name ?
                get_value(i) :
                find_value_by_name(name, i + 1))
            : throw std::runtime_error{fmt::format("Unknown {} enum value \"{}\"",
                ctti::nameof<enum_type>(), name)};
    }

    constexpr ctti::detail::cstring find_name_by_value(const enum_type value, std::size_t i = 0) const
    {
        return (i < count()) ?
            (get_value(i) == value ?
                get_name(i) :
                find_name_by_value(value, i + 1))
            : throw std::runtime_error{fmt::format("Unknown {} enum value '{}'",
                ctti::nameof<enum_type>(), static_cast<typename std::underlying_type<enum_type>::type>(value))};
    }

    constexpr int find_value_index(const underlying_type value, std::size_t i = 0) const
    {
        return (i < count()) ?
            (get_underlying_values()[i] == value ?
                static_cast<int>(i) :
                find_value_index(value, i + 1))
            : -1;
    }
};

template<ctti::detail::hash_t Hash>
constexpr ctti::detail::cstring string_constant = "Unknown tinyrefl string constant";

}

}

#ifndef TINYREFL_PP_CAT
#define TINYREFL_PP_CAT_IMPL(x, y) x ## y
#define TINYREFL_PP_CAT(x, y) TINYREFL_PP_CAT_IMPL(x, y)
#endif // TINYREFL_PP_CAT

#ifndef TINYREFL_PP_STR
#define TINYREFL_PP_STR(...) #__VA_ARGS__
#endif // TINYREFL_PP_STR

#ifndef TINYREFL_PP_UNWRAP
#define TINYREFL_PP_UNWRAP(...) __VA_ARGS__
#endif // TINYREFL_PP_UNWRAP

#define TINYREFL_DEFINE_STRINGS
#define TINYREFL_STRING(string) ::tinyrefl::backend::string_constant<ctti::detail::cstring{TINYREFL_PP_STR(string)}.hash()>
#define TINYREFL_DEFINE_STRING(string)                                                               \
    namespace tinyrefl { namespace backend {                                                               \
        template<> constexpr ::ctti::detail::cstring string_constant<ctti::detail::cstring{TINYREFL_PP_STR(string)}.hash()> = TINYREFL_PP_STR(string); \
    } /* namespace backend */ } // namespace tinyrefl

#define TINYREFL_SEQUENCE(elems) ::tinyrefl::meta::list<TINYREFL_PP_UNWRAP elems>
#define TINYREFL_TYPE(name, fullname) fullname
#define TINYREFL_VALUE(value) value
#define TINYREFL_MEMBER(name, fullname, type, pointer) ::tinyrefl::backend::member<fullname, CTTI_STATIC_VALUE(pointer)>
#define TINYREFL_ENUM_VALUE(name, type, value) CTTI_STATIC_VALUE(value)

#define TINYREFL_REFLECT_MEMBER(member)                  \
    namespace tinyrefl { namespace backend {             \
    template<>                                           \
    struct metadata_of<member::name.full_name().hash()>               \
    {                                                    \
        using type = member;                            \
    };                                                   \
    } /* namespace backend */ } // namespace tinyrefl

#define TINYREFL_REFLECT_CLASS(classname, ...)              \
    namespace tinyrefl { namespace backend {               \
    template<>                                             \
    struct metadata_of<::ctti::nameof<classname>().hash()> \
    {                                                      \
        using type = class_<__VA_ARGS__>;                  \
    };                                                     \
    } /* namespace backend */ } // namespace tinyrefl

#define TINYREFL_REFLECT_ENUM(enumname, ...)              \
    namespace tinyrefl { namespace backend {              \
    template<>                                            \
    struct metadata_of<::ctti::nameof<enumname>().hash()> \
    {                                                     \
        using type = enum_<enumname, __VA_ARGS__>;        \
    };                                                    \
    } /* namespace backend */ } // namespace tinyrefl


#define TINYREFL_GODMODE                                                                                         \
    struct tinyrefl_godmode_tag {};                                                                              \
    template<                                                                                                    \
        typename __TinyRefl__GodModeTemplateParam__BaseClasses,                                                  \
        typename __TinyRelf__GodModeTemplateParam__Members,                                                      \
        typename __TinyRelf__GodModeTemplateParam__Classes,                                                      \
        typename __TinyRelf__GodModeTemplateParam__Enums                                                         \
    >                                                                                                            \
    friend struct ::tinyrefl::backend::class_;                                                                   \
    template<tinyrefl::backend::type_hash_t __TinyRefl__GodModeTemplateParam__Hash>                              \
    friend struct ::tinyrefl::backend::metadata_of;                                                              \
    template<const ::ctti::detail::cstring& __TinyRefl__GodModeTemplateParamName, typename __TinyRefl__GodModeTemplateParam__Pointer> \
    friend struct ::tinyrefl::backend::member;                                                                   \
    template<typename __TinyRefl__GodModeTemplateParam__Signature>                                               \
    friend struct ::tinyrefl::backend::constructor;                                                              \
    template<typename __TinyRefl__GodModeTemplateParam__Enum, typename __TinyRefl__GodModeTemplateParam__Values> \
    friend struct ::tinyrefl::backend::enum_;

#endif // TINYREFL_BACKEND_HPP
