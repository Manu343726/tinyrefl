#ifndef TINYREFL_MATCHERS_MATCHER_HPP_INCLUDED
#define TINYREFL_MATCHERS_MATCHER_HPP_INCLUDED

#include <limits>
#include <type_traits>

#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/entities/function_signature.hpp>
#include <tinyrefl/entities/type.hpp>

namespace tinyrefl
{

namespace matchers
{

template<typename T, typename Function>
struct equals_t
{
    constexpr equals_t(T value, Function function)
        : _value{std::move(value)}, _function{std::move(function)}
    {
    }

    template<typename Entity>
    constexpr bool operator()(const Entity& entity) const
    {
        return _value == _function(entity);
    }

private:
    T        _value;
    Function _function;
};

namespace impl
{

struct identity_t
{
    constexpr identity_t() = default;

    template<typename T>
    constexpr const T& operator()(const T& value) const
    {
        return value;
    }
};

template<template<typename...> class Trait, typename... Ts>
struct matches_trait_t
{
    constexpr matches_trait_t() = default;

    template<typename Entity>
    constexpr bool operator()(const Entity& entity) const
    {
        return false;
    }

    template<typename T>
    constexpr bool operator()(const tinyrefl::entities::type<T>& type) const
    {
        return static_cast<bool>(Trait<T, Ts...>{});
    }
};

} // namespace impl

template<typename T, typename Function>
constexpr equals_t<std::decay_t<T>, std::decay_t<Function>>
    equals(T&& value, Function&& function)
{
    return {std::forward<T>(value), std::forward<Function>(function)};
}

template<typename T>
constexpr auto equals(T&& value)
{
    return equals(std::forward<T>(value), impl::identity_t{});
}

#define TINYREFL_MATCHERS_GET_MEMBER(member, default_value)                    \
    namespace                                                                  \
    {                                                                          \
    template<typename T, typename = void>                                      \
    struct has_##member : public tinyrefl::meta::false_                        \
    {                                                                          \
    };                                                                         \
                                                                               \
    template<typename T>                                                       \
    struct has_##member<                                                       \
        T,                                                                     \
        tinyrefl::meta::void_t<decltype(std::declval<T>().member())>>          \
        : public tinyrefl::meta::true_                                         \
    {                                                                          \
    };                                                                         \
    }                                                                          \
                                                                               \
    struct get_##member                                                        \
    {                                                                          \
        template<typename Entity>                                              \
        constexpr decltype(auto) operator()(const Entity& entity) const        \
        {                                                                      \
            return call(entity, has_##member<Entity>{});                       \
        }                                                                      \
                                                                               \
    private:                                                                   \
        template<typename Entity>                                              \
        static constexpr decltype(auto)                                        \
            call(const Entity& entity, const tinyrefl::meta::true_&)           \
        {                                                                      \
            return entity.member();                                            \
        }                                                                      \
                                                                               \
        template<typename Entity>                                              \
        static constexpr decltype(auto)                                        \
            call(const Entity& entity, const tinyrefl::meta::false_&)          \
        {                                                                      \
            return default_value;                                              \
        }                                                                      \
    };                                                                         \
                                                                               \
    template<typename T>                                                       \
    constexpr auto matches_##member(T&& value)                                 \
    {                                                                          \
        return equals(std::forward<T>(value), get_##member{});                 \
    }

namespace
{
struct invalid_type
{
};
} // namespace

TINYREFL_MATCHERS_GET_MEMBER(name, tinyrefl::string{""});
TINYREFL_MATCHERS_GET_MEMBER(full_name, tinyrefl::string{""});
TINYREFL_MATCHERS_GET_MEMBER(display_name, tinyrefl::string{""});
TINYREFL_MATCHERS_GET_MEMBER(full_display_name, tinyrefl::string{""});
TINYREFL_MATCHERS_GET_MEMBER(kind, tinyrefl::entities::entity_kind::UNKNOWN);
TINYREFL_MATCHERS_GET_MEMBER(parent, tinyrefl::backend::no_metadata{});
TINYREFL_MATCHERS_GET_MEMBER(ancestors, std::make_tuple());
TINYREFL_MATCHERS_GET_MEMBER(children, std::make_tuple());
TINYREFL_MATCHERS_GET_MEMBER(bases, std::make_tuple());
TINYREFL_MATCHERS_GET_MEMBER(index, std::numeric_limits<std::size_t>::max());
TINYREFL_MATCHERS_GET_MEMBER(attributes, std::make_tuple());
TINYREFL_MATCHERS_GET_MEMBER(arguments, std::make_tuple());
TINYREFL_MATCHERS_GET_MEMBER(decayed_arguments, std::make_tuple());
TINYREFL_MATCHERS_GET_MEMBER(parameters, std::make_tuple());
TINYREFL_MATCHERS_GET_MEMBER(
    signature,
    (tinyrefl::entities::
         function_signature<tinyrefl::meta::list<>, invalid_type()>{}));
TINYREFL_MATCHERS_GET_MEMBER(
    return_type, tinyrefl::entities::type<invalid_type>{})
TINYREFL_MATCHERS_GET_MEMBER(type, tinyrefl::entities::type<invalid_type>{})
TINYREFL_MATCHERS_GET_MEMBER(decayed, tinyrefl::entities::type<invalid_type>{})

#undef TINYREFL_MATCHERS_GET_MEMBER

constexpr auto ofKind(const tinyrefl::entities::entity_kind kind)
{
    return matches_kind(kind);
}

#define TINYREFL_MATCHERS_KIND_MATCHER(kind, Kind)                             \
    constexpr auto is##kind()                                                  \
    {                                                                          \
        return ofKind(tinyrefl::entities::entity_kind::Kind);                  \
    }

TINYREFL_MATCHERS_KIND_MATCHER(namespace, NAMESPACE)
TINYREFL_MATCHERS_KIND_MATCHER(class, CLASS)
TINYREFL_MATCHERS_KIND_MATCHER(enum, ENUM)
TINYREFL_MATCHERS_KIND_MATCHER(enum_value, ENUM_VALUE)
TINYREFL_MATCHERS_KIND_MATCHER(base_class, BASE_CLASS)
TINYREFL_MATCHERS_KIND_MATCHER(member_variable, MEMBER_VARIABLE)
TINYREFL_MATCHERS_KIND_MATCHER(member_class, MEMBER_CLASS)
TINYREFL_MATCHERS_KIND_MATCHER(member_function, MEMBER_FUNCTION)
TINYREFL_MATCHERS_KIND_MATCHER(constructor, CONSTRUCTOR)
TINYREFL_MATCHERS_KIND_MATCHER(member_enum, MEMBER_ENUM)
TINYREFL_MATCHERS_KIND_MATCHER(object, OBJECT)
TINYREFL_MATCHERS_KIND_MATCHER(static_member_function, STATIC_MEMBER_FUNCTION)
TINYREFL_MATCHERS_KIND_MATCHER(static_member_variable, STATIC_MEMBER_VARIABLE)
TINYREFL_MATCHERS_KIND_MATCHER(function, FUNCTION)
TINYREFL_MATCHERS_KIND_MATCHER(variable, VARIABLE)
TINYREFL_MATCHERS_KIND_MATCHER(file, FILE)
TINYREFL_MATCHERS_KIND_MATCHER(unknown, UNKNOWN)

#undef TINYREFL_MATCHERS_KIND_MATCHER

namespace impl
{

template<typename Getter, typename InnerMatcher>
constexpr auto
    match_related_entities(Getter&& getter, InnerMatcher&& innerMatcher)
{
    return tinyrefl::meta::combine(
        tinyrefl::meta::hof::map(std::forward<Getter>(getter)),
        tinyrefl::meta::hof::any_of(std::forward<InnerMatcher>(innerMatcher)));
}
} // namespace impl

template<typename InnerMatcher>
constexpr auto not_(InnerMatcher&& innerMatcher)
{
    return tinyrefl::meta::hof::not_(std::forward<InnerMatcher>(innerMatcher));
}

template<typename InnerMatcher>
constexpr auto hasChild(InnerMatcher&& innerMatcher)
{
    return impl::match_related_entities(
        get_children{}, std::forward<InnerMatcher>(innerMatcher));
}

template<typename InnerMatcher>
constexpr auto hasParent(InnerMatcher&& innerMatcher)
{
    return tinyrefl::meta::combine(
        get_parent{}, std::forward<InnerMatcher>(innerMatcher));
}

template<typename InnerMatcher>
constexpr auto hasAncestor(InnerMatcher&& innerMatcher)
{
    return impl::match_related_entities(
        get_ancestors{}, std::forward<InnerMatcher>(innerMatcher));
}

template<typename InnerMatcher>
constexpr auto hasBase(InnerMatcher&& innerMatcher)
{
    return impl::match_related_entities(
        get_bases{}, std::forward<InnerMatcher>(innerMatcher));
}

template<typename InnerMatcher>
constexpr auto hasArgument(InnerMatcher&& innerMatcher)
{
    return impl::match_related_entities(
        get_arguments{}, std::forward<InnerMatcher>(innerMatcher));
}

template<typename InnerMatcher>
constexpr auto hasType(InnerMatcher&& innerMatcher)
{
    return impl::match_related_entities(
        get_type{}, std::forward<InnerMatcher>(innerMatcher));
}

template<typename InnerMatcher>
constexpr auto hasDecayedType(InnerMatcher&& innerMatcher)
{
    return impl::match_related_entities(
        tinyrefl::meta::combine(get_type{}, get_decayed{}),
        std::forward<InnerMatcher>(innerMatcher));
}

template<typename T>
constexpr auto type(const tinyrefl::entities::type_entity<T>& type)
{
    return equals(type);
}

template<typename T>
constexpr auto type()
{
    return type(tinyrefl::entities::type_entity<T>{});
}

template<typename InnerMatcher>
constexpr auto returns(InnerMatcher&& innerMatcher)
{
    return impl::match_related_entities(
        tinyrefl::meta::combine(get_signature{}, get_return_type{}),
        std::forward<InnerMatcher>(innerMatcher));
}

template<typename T>
constexpr auto returns()
{
    return returns(type<T>());
}

template<typename InnerMatcher>
constexpr auto hasAttribute(InnerMatcher&& innerMatcher)
{
    return impl::match_related_entities(
        get_attributes{}, std::forward<InnerMatcher>(innerMatcher));
}

constexpr auto parameterCountIs(const std::size_t n)
{
    return tinyrefl::meta::combine(
        get_signature{},
        get_parameters{},
        tinyrefl::meta::hof::tuple_size(),
        equals(n));
}

template<typename InnerMatcher>
constexpr auto hasParameter(InnerMatcher&& innerMatcher)
{
    return impl::match_related_entities(
        tinyrefl::meta::combine(get_signature{}, get_parameters{}),
        std::forward<InnerMatcher>(innerMatcher));
}

constexpr auto withIndex(const std::size_t i)
{
    return tinyrefl::meta::combine(get_index{}, equals(i));
}

namespace impl
{
template<std::size_t... Is, typename... InnerMatchers>
constexpr auto
    hasParameters(std::index_sequence<Is...>, InnerMatchers&&... innerMatchers)
{
    return allOf(
        parameterCountIs(sizeof...(InnerMatchers)),
        hasParameter(allOf(
            withIndex(Is), std::forward<InnerMatchers>(innerMatchers)))...);
}
} // namespace impl

template<typename... InnerMatchers>
constexpr auto hasParameters(InnerMatchers&&... innerMatchers)
{
    return impl::hasParameters(
        std::index_sequence_for<InnerMatchers...>{},
        std::forward<InnerMatchers>(innerMatchers)...);
}

template<typename String>
constexpr auto named(String&& name)
{
    return matches_name(std::forward<String>(name));
}

template<typename String>
constexpr auto display_named(String&& displayName)
{
    return matches_display_name(std::forward<String>(displayName));
}

template<typename String>
constexpr auto full_named(String&& name)
{
    return matches_full_name(std::forward<String>(name));
}

template<typename String>
constexpr auto full_display_named(String&& displayName)
{
    return matches_full_display_name(std::forward<String>(displayName));
}

template<typename... InnerMatchers>
constexpr auto allOf(InnerMatchers&&... innerMatchers)
{
    return tinyrefl::meta::hof::all(
        std::make_tuple(std::forward<InnerMatchers>(innerMatchers)...));
}

template<typename... InnerMatchers>
constexpr auto anyOf(InnerMatchers&&... innerMatchers)
{
    return tinyrefl::meta::hof::any(
        std::make_tuple(std::forward<InnerMatchers>(innerMatchers)...));
}

template<template<typename...> class Trait, typename... Ts>
constexpr auto trait(const std::tuple<Ts...>& types)
{
    return impl::matches_trait_t<Trait, Ts...>{};
}

} // namespace matchers

template<typename Entity, typename Matcher>
constexpr bool matches(const Entity& entity, const Matcher& matcher)
{
    return matcher(entity);
}

template<typename... Entities, typename Matcher>
constexpr auto
    matches(const std::tuple<Entities...>& entities, const Matcher& matcher)
{
    return tinyrefl::meta::tuple_filter(entities, matcher);
}

template<typename T, typename Matcher>
constexpr bool matches(const Matcher& matcher)
{
    return matcher(tinyrefl::metadata<T>());
}

} // namespace tinyrefl

#endif // TINYREFL_MATCHERS_MATCHER_HPP_INCLUDED
