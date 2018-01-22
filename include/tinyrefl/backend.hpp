#ifndef TINYREFL_BACKEND_HPP
#define TINYREFL_BACKEND_HPP

#include <ctti/detail/meta.hpp>
#include <ctti/detail/cstring.hpp>
#include <ctti/detail/hash.hpp>
#include <ctti/detailed_nameof.hpp>
#include <ctti/symbol.hpp>

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

enum class entity_kind
{
    ENUM, CLASS, MEMBER
};

template<typename Pointer>
struct member : public Pointer
{
    using pointer_type = typename Pointer::value_type;
    using pointer_static_value = Pointer;
    static constexpr entity_kind kind = entity_kind::MEMBER;
    static constexpr ctti::name_t name = ctti::detailed_nameof<Pointer>();

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

template<
    typename BaseClasses,
    typename Members
>
struct class_
{
    static constexpr entity_kind kind = entity_kind::CLASS;
    using base_classes = BaseClasses;
    using members = Members;
};


template<typename Values>
struct enum_
{
    static constexpr entity_kind kind = entity_kind::CLASS;
    using values = Values;
};

}

}

#define TINYREFL_REFLECT_MEMBER_IMPL(pointer)                               \
    namespace tinyrefl { namespace backend {                                \
    template<>                                                              \
    struct metadata_of<::ctti::nameof<CTTI_STATIC_VALUE(pointer)>().hash()> \
    {                                                                       \
        using type = tinyrefl::backend::member<CTTI_STATIC_VALUE(pointer)>; \
    };                                                                      \
    } /* namespace backend */ } // namespace tinyrefl

#define TINYREFL_REFLECT_CLASS_IMPL(classname, ...)        \
    namespace tinyrefl { namespace backend {               \
    template<>                                             \
    struct metadata_of<::ctti::nameof<classname>().hash()> \
    {                                                      \
        using type = class_<__VA_ARGS__>;                  \
    };                                                     \
    } /* namespace backend */ } // namespace tinyrefl

#define TINYREFL_REFLECT_ENUM_IMPL(enumname, values)      \
    namespace tinyrefl { namespace backend {              \
    template<>                                            \
    struct metadata_of<::ctti::nameof<enumname>().hash()> \
    {                                                     \
        using type = enum_<values>;                       \
    };                                                    \
    } /* namespace backend */ } // namespace tinyrefl


#ifndef TINYREFL_RUNNING
#define TINYREFL_GODMODE \
    template<typename __TinyRefl__GodModeTemplateParam__BaseClasses, typename __TinyRelf__GodModeTemplateParam__Members> \
    friend struct ::tinyrefl::backend::class_;                                                                            \
    template<tinyrefl::backend::type_hash_t __TinyRefl__GodModeTemplateParam__Hash>                                      \
    friend struct ::tinyrefl::backend::metadata_of;                                                                       \
    template<typename __TinyRefl__GodModeTemplateParam__Pointer>                                                         \
    friend struct ::tinyrefl::backend::member;                                                                            \
    template<typename __TinyRefl__GodModeTemplateParam__Values>                                                          \
    friend struct ::tinyrefl::backend::enum_;
#endif // TINYREFL_RUNNING

#ifndef TINYREFL_DEBUG_HASHES

#define TINYREFL_REFLECT_MEMBER(pointer) \
    TINYREFL_REFLECT_MEMBER_IMPL(pointer)

#define TINYREFL_REFLECT_CLASS(classname, ...) \
    TINYREFL_REFLECT_CLASS_IMPL(classname, __VA_ARGS__)

#define TINYREFL_REFLECT_ENUM(enumname, ...) \
    TINYREFL_REFLECT_ENUM_IMPL(enumclass, __VA_ARGS__)

#else

namespace tinyrefl
{

namespace backend
{

namespace debug
{

template<typename T>
struct entity_hash : public tinyrefl::meta::size_t<0> {};

}

}

}

#define TINYREFL_REFLECT_MEMBER_REGISTER_HASH(pointer)             \
    namespace tinyrefl { namespace backend { namespace debug {     \
    template<>                                                     \
    struct entity_hash<CTTI_STATIC_VALUE(pointer)> : public        \
        tinyrefl::meta::size_t<ctti::nameof<CTTI_STATIC_VALUE(pointer)>().hash()> \
    {};                                                            \
    } /* namespace backend */ } /* namespace tinyrefl */ } // namespace debug

#define TINYREFL_REFLECT_CLASS_REGISTER_HASH(classname)          \
    namespace tinyrefl { namespace backend { namespace debug {   \
    template<>                                                   \
    struct entity_hash<classname> : public                       \
        tinyrefl::meta::size_t<ctti::nameof<classname>().hash()> \
    {};                                                          \
    } /* namespace backend */ } /* namespace tinyrefl */ } // namespace debug

#define TINYREFL_REFLECT_ENUM_REGISTER_HASH(enumname, values)   \
    namespace tinyrefl { namespace backend { namespace debug {  \
    template<>                                                  \
    struct entity_hash<enumname> : public  i                    \
        tinyrefl::meta::size_t<ctti::nameof<enumname>().hash()> \
    {};                                                         \
    } /* namespace backend */ } /* namespace tinyrefl */ } // namespace debug


#define TINYREFL_REFLECT_MEMBER(pointer)  \
    TINYREFL_REFLECT_MEMBER_IMPL(pointer) \
    TINYREFL_REFLECT_MEMBER_REGISTER_HASH(pointer)

#define TINYREFL_REFLECT_CLASS(classname, ...)          \
    TINYREFL_REFLECT_CLASS_IMPL(classname, __VA_ARGS__) \
    TINYREFL_REFLECT_CLASS_REGISTER_HASH(classname)

#define TINYREFL_REFLECT_ENUM(enumname, ...)          \
    TINYREFL_REFLECT_ENUM_IMPL(enumname, __VA_ARGS__) \
    TINYREFL_REFLECT_ENUM_REGISTER_HASH(enumname)

#endif // TINYREFL_DEBUG_HASHES

#endif // TINYREFL_BACKEND_HPP
