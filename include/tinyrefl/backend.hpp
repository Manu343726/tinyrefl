#ifndef TINYREFL_BACKEND_HPP
#define TINYREFL_BACKEND_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/cstring.hpp>
#include <ctti/detail/hash.hpp>
#include <ctti/name.hpp>
#include <ctti/symbol.hpp>
#include <ctti/detail/algorithm.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <stdexcept>
#include <initializer_list>
#include <tinyrefl/utils/typename.hpp>

namespace tinyrefl
{

namespace meta = ctti::meta;

namespace backend
{

using type_hash_t = ctti::detail::hash_t;

constexpr ctti::detail::cstring default_string_constant = "default tinyrefl string constant";

template<ctti::detail::hash_t Hash>
constexpr ctti::detail::cstring string_constant()
{
    return default_string_constant;
}

struct no_metadata {};

template<typename Type, typename = void>
struct metadata_of
{
    using type = no_metadata;
};

template<typename Type, typename = void>
struct metadata_registered_for_type : public tinyrefl::meta::bool_<!std::is_same<
    typename metadata_of<Type>::type,
    no_metadata
>::value> {};

template<typename T>
using metadata_of_type = typename metadata_of<T>::type;

enum class entity_kind
{
    NAMESPACE,
    CLASS,
    ENUM,
    ENUM_VALUE,
    BASE_CLASS,
    MEMBER_VARIABLE,
    MEMBER_CLASS,
    MEMBER_FUNCTION,
    MEMBER_ENUM,
    OBJECT
};

inline std::ostream& operator<<(std::ostream& os, const entity_kind e)
{
    switch(e)
    {
    case entity_kind::NAMESPACE:
        return os << "namespace";
    case entity_kind::CLASS:
        return os << "class";
    case entity_kind::ENUM:
        return os << "enum";
    case entity_kind::ENUM_VALUE:
        return os << "enum_value";
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

template<ctti::detail::hash_t Name, typename Pointer>
struct member : public Pointer
{
    using pointer_type = typename Pointer::value_type;
    using pointer_static_value = Pointer;
    static constexpr entity_kind kind = (std::is_member_function_pointer<pointer_type>::value ? entity_kind::MEMBER_FUNCTION : entity_kind::MEMBER_VARIABLE);
    static constexpr ctti::name_t name = tinyrefl::backend::string_constant<Name>();

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

template<ctti::detail::hash_t Name, typename Pointer>
constexpr ctti::name_t member<Name, Pointer>::name;

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
    using members = Members;
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
        using type = tinyrefl::meta::size_t<Total::value + metadata_of_type<BaseClass>::members::size + metadata_of_type<BaseClass>::total_members::value>;
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

template<ctti::detail::hash_t Name, typename Value>
struct enum_value : public Value
{
    using value_type = typename Value::value_type;
    using underlying_value_type = typename std::underlying_type<value_type>::type;
    using value_static_value = Value;
    static constexpr entity_kind kind = entity_kind::ENUM_VALUE;
    static constexpr ctti::name_t name = tinyrefl::backend::string_constant<Name>();

    constexpr enum_value() = default;

    constexpr ctti::detail::cstring value_name() const
    {
        return name.name();
    }

    constexpr ctti::name_t full_value_name() const
    {
        return name;
    }

    constexpr value_type value() const
    {
        return Value::value;
    }

    constexpr underlying_value_type underlying_value() const
    {
        return static_cast<underlying_value_type>(value());
    }

    friend constexpr bool operator==(const enum_value& lhs, const ctti::detail::cstring& name)
    {
        return lhs.value_name() == name;
    }

    friend constexpr bool operator==(const enum_value& lhs, const value_type value)
    {
        return lhs.value() == value;
    }

    friend constexpr bool operator==(const enum_value& lhs, const underlying_value_type value)
    {
        return lhs.underlying_value() == value;
    }

    friend constexpr bool operator!=(const enum_value& lhs, const ctti::detail::cstring& name)
    {
        return !(lhs == name);
    }

    friend constexpr bool operator!=(const enum_value& lhs, const value_type value)
    {
        return !(lhs == value);
    }

    friend constexpr bool operator!=(const enum_value& lhs, const underlying_value_type value)
    {
        return !(lhs == value);
    }
};

template<ctti::detail::hash_t Name, typename Value>
constexpr ctti::name_t enum_value<Name, Value>::name;

template<ctti::detail::hash_t Name, typename Enum, typename Values>
struct enum_;

template<ctti::detail::hash_t Name, typename Enum, typename... Values>
struct enum_<Name, Enum, tinyrefl::meta::list<Values...>>
{
    static constexpr entity_kind kind = entity_kind::ENUM;
    static constexpr ctti::name_t enum_name = tinyrefl::backend::string_constant<Name>();
    using values = tinyrefl::meta::list<Values...>;
    using enum_type = Enum;
    using underlying_type = typename std::underlying_type<enum_type>::type;

    struct value_t
    {
        template<typename EnumValueMetadata>
        constexpr value_t(const EnumValueMetadata& enum_value) :
            value_t{enum_value.value(), enum_value.value_name()}
        {}

        constexpr value_t(const enum_type value, const ctti::detail::cstring& name) :
            _value{value},
            _name{name}
        {}

        constexpr ctti::detail::cstring name() const
        {
            return _name;
        }

        constexpr enum_type value() const
        {
            return _value;
        }

        constexpr underlying_type underlying_value() const
        {
            return static_cast<underlying_type>(value());
        }

        friend constexpr bool operator==(const value_t& lhs, const ctti::detail::cstring& name)
        {
            return lhs.value_name() == name;
        }

        friend constexpr bool operator==(const value_t& lhs, const enum_type value)
        {
            return lhs.value() == value;
        }

        friend constexpr bool operator==(const value_t& lhs, const underlying_type value)
        {
            return lhs.underlying_value() == value;
        }

        friend constexpr bool operator!=(const value_t& lhs, const ctti::detail::cstring& name)
        {
            return !(lhs == name);
        }

        friend constexpr bool operator!=(const value_t& lhs, const enum_type value)
        {
            return !(lhs == value);
        }

        friend constexpr bool operator!=(const value_t& lhs, const underlying_type value)
        {
            return !(lhs == value);
        }

    private:
        enum_type _value;
        ctti::detail::cstring _name;
    };

    using values_array = std::array<value_t, values::size>;
    static constexpr values_array enum_values = {value_t{Values{}}...};
    static constexpr value_t invalid_value = value_t{enum_type{}, "invalid enum value"};

    constexpr enum_() = default;

    constexpr std::size_t count() const
    {
        return values::size;
    }

    constexpr const ctti::name_t& name() const
    {
        return enum_::enum_name;
    }

    constexpr const value_t& get_value(const ctti::detail::cstring& name) const
    {
        return find_value_by_name(name);
    }

    constexpr const value_t& get_value(const std::size_t i) const
    {
        return get_values()[i];
    }

    constexpr const value_t& get_value(const enum_type value) const
    {
        return get_values()[find_value_index(value)];
    }

    constexpr bool is_enumerated_value(const underlying_type value) const
    {
        return find_value_index(value) >= 0;
    }

    constexpr const values_array& get_values() const
    {
        return enum_values;
    }

private:
    constexpr const value_t& find_value_by_name(const ctti::detail::cstring& name, std::size_t i = 0) const
    {
        return (i < count()) ?
            (get_value(i).name() == name ?
                get_value(i) :
                find_value_by_name(name, i + 1))
            : invalid_value;
    }

    constexpr int find_value_index(const underlying_type value, std::size_t i = 0) const
    {
        return (i < count()) ?
            (get_values()[i] == value ?
                static_cast<int>(i) :
                find_value_index(value, i + 1))
            : -1;
    }

    constexpr int find_value_index(const enum_type value, std::size_t i = 0) const
    {
        return (i < count()) ?
            (get_values()[i] == value ?
                static_cast<int>(i) :
                find_value_index(value, i + 1))
            : -1;
    }
};

template<ctti::detail::hash_t Name, typename Enum, typename... Values>
constexpr typename enum_<Name, Enum, tinyrefl::meta::list<Values...>>::values_array enum_<Name, Enum, tinyrefl::meta::list<Values...>>::enum_values;
template<ctti::detail::hash_t Name, typename Enum, typename... Values>
constexpr ctti::name_t enum_<Name, Enum, tinyrefl::meta::list<Values...>>::enum_name;
template<ctti::detail::hash_t Name, typename Enum, typename... Values>
constexpr typename enum_<Name, Enum, tinyrefl::meta::list<Values...>>::value_t enum_<Name, Enum, tinyrefl::meta::list<Values...>>::invalid_value;

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
#define TINYREFL_STRING(...) ctti::detail::cstring{TINYREFL_PP_STR(__VA_ARGS__)}.hash()
#define TINYREFL_DEFINE_STRING(...)                                                               \
    namespace tinyrefl { namespace backend {                                                               \
        template<> \
        constexpr ::ctti::detail::cstring string_constant<ctti::detail::cstring{TINYREFL_PP_STR(__VA_ARGS__)}.hash()>() { \
            return ::ctti::detail::cstring{TINYREFL_PP_STR(__VA_ARGS__)}; \
        } \
    } /* namespace backend */ } // namespace tinyrefl

#define TINYREFL_SEQUENCE(elems) ::tinyrefl::meta::list<TINYREFL_PP_UNWRAP elems>
#define TINYREFL_TYPE(name, fullname) fullname
#define TINYREFL_VALUE(value) value
#define TINYREFL_MEMBER(name, fullname, type, pointer) ::tinyrefl::backend::member<fullname, CTTI_STATIC_VALUE(pointer)>
#define TINYREFL_ENUM_VALUE(name, fullname, type, value) ::tinyrefl::backend::enum_value<fullname, CTTI_STATIC_VALUE(value)>

#define TINYREFL_REFLECT_MEMBER(member)                  \
    namespace tinyrefl { namespace backend {             \
    template<>                                           \
    struct metadata_of<typename member::pointer_static_value, void>               \
    {                                                    \
        using type = member;                            \
    };                                                   \
    } /* namespace backend */ } // namespace tinyrefl

#define TINYREFL_REFLECT_ENUM_VALUE(value)                  \
    namespace tinyrefl { namespace backend {             \
    template<>                                           \
    struct metadata_of<typename value::value_static_value, void>               \
    {                                                    \
        using type = value;                            \
    };                                                   \
    } /* namespace backend */ } // namespace tinyrefl

#define TINYREFL_REFLECT_CLASS(classname, ...)              \
    namespace tinyrefl { namespace backend {               \
    template<>                                             \
    struct metadata_of<classname, void> \
    {                                                      \
        using type = class_<__VA_ARGS__>;                  \
    };                                                     \
    } /* namespace backend */ } // namespace tinyrefl

#define TINYREFL_REFLECT_ENUM(name, enum_type, values)    \
    namespace tinyrefl { namespace backend {              \
    template<>                                            \
    struct metadata_of<enum_type, void> \
    {                                                     \
        using type = enum_<name, enum_type, values>; \
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
    template<typename __TinyRefl__GodModeTemplateParam__Type, typename __TinyRefl__GodModeTemplateParam__Void>                              \
    friend struct ::tinyrefl::backend::metadata_of;                                                              \
    template<ctti::detail::hash_t __TinyRefl__GodModeTemplateParam__Name, typename __TinyRefl__GodModeTemplateParam__Pointer> \
    friend struct ::tinyrefl::backend::member;                                                                   \
    template<typename __TinyRefl__GodModeTemplateParam__Signature>                                               \
    friend struct ::tinyrefl::backend::constructor;                                                              \
    template<ctti::detail::hash_t __TinyRefl__GodModeTemplateParam__Name, typename __TinyRefl__GodModeTemplateParam__Enum, typename __TinyRefl__GodModeTemplateParam__Values> \
    friend struct ::tinyrefl::backend::enum_;

#endif // TINYREFL_BACKEND_HPP
