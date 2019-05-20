#ifndef TINYREFL_VISITOR_HPP
#define TINYREFL_VISITOR_HPP

#include <tinyrefl/backend/metadata.hpp>
#include <tinyrefl/entities/entity_kind.hpp>
#include <tinyrefl/types/static_value.hpp>
#include <tinyrefl/types/string.hpp>
#include <tinyrefl/utils/meta.hpp>
#include <tinyrefl/utils/overloaded_function.hpp>
#include <tinyrefl/utils/preprocessor.hpp>

namespace tinyrefl
{

template<typename T, typename... Visitors>
constexpr void recursive_visit(Visitors&&... visitors);

namespace impl
{

template<typename Visitor>
struct constexpr_visitor
{
    template<typename Visitor_>
    constexpr constexpr_visitor(Visitor_&& visitor)
        : visitor{std::forward<Visitor_>(visitor)}
    {
    }

    template<typename Entity>
    using callable_with_kind = decltype(std::declval<Visitor>()(
        Entity{}, tinyrefl::entities::kind_constant_v<Entity{}.kind()>));

    template<typename Entity>
    using callable_without_kind = decltype(std::declval<Visitor>()(Entity{}));

    template<typename Entity>
    constexpr auto operator()(const Entity&) const -> std::enable_if_t<
        tinyrefl::meta::is_detected_v<callable_with_kind, Entity> &&
        !tinyrefl::meta::is_detected_v<callable_without_kind, Entity>>
    {
        visitor(Entity{}, tinyrefl::entities::kind_constant_v<Entity{}.kind()>);
    }

    template<typename Entity>
    constexpr auto operator()(const Entity&) const -> std::enable_if_t<
        !tinyrefl::meta::is_detected_v<callable_with_kind, Entity> &&
        tinyrefl::meta::is_detected_v<callable_without_kind, Entity>>
    {
        visitor(Entity{});
    }

    template<typename Entity>
    constexpr auto operator()(const Entity&) const -> std::enable_if_t<
        !tinyrefl::meta::is_detected_v<callable_with_kind, Entity> &&
        !tinyrefl::meta::is_detected_v<callable_without_kind, Entity>>
    {
    }

    template<typename Entity>
    constexpr auto operator()(const Entity&) const -> std::enable_if_t<
        tinyrefl::meta::is_detected_v<callable_with_kind, Entity> &&
        tinyrefl::meta::is_detected_v<callable_without_kind, Entity>>
    {
        visitor(Entity{}, tinyrefl::entities::kind_constant_v<Entity{}.kind()>);
    }

    Visitor visitor;
};

template<typename Visitor>
constexpr constexpr_visitor<std::decay_t<Visitor>>
    make_constexpr_visitor(Visitor&& visitor)
{
    return {std::forward<Visitor>(visitor)};
}

template<typename Visitor>
struct constexpr_recurive_visitor
{
    constexpr constexpr_recurive_visitor(Visitor&& visitor)
        : visitor{std::move(visitor)}
    {
    }

    template<typename Entity>
    using callable_with_kind = decltype(std::declval<Visitor>()(
        Entity{}, tinyrefl::entities::kind_constant_v<Entity{}.kind()>));

    template<typename Entity>
    using callable_without_kind = decltype(std::declval<Visitor>()(Entity{}));

    template<typename Entity>
    constexpr auto operator()(const Entity&) const -> std::enable_if_t<
        tinyrefl::meta::is_detected_v<callable_with_kind, Entity> &&
        !tinyrefl::meta::is_detected_v<callable_without_kind, Entity>>
    {
        visitor(Entity{}, tinyrefl::entities::kind_constant_v<Entity{}.kind()>);

        tinyrefl::meta::foreach(Entity{}.children(), *this);
    }

    template<typename Entity>
    constexpr auto operator()(const Entity&) const -> std::enable_if_t<
        tinyrefl::meta::is_detected_v<callable_without_kind, Entity>>
    {
        visitor(Entity{});

        tinyrefl::meta::foreach(Entity{}.children(), *this);
    }

    template<typename Entity>
    constexpr auto operator()(const Entity&) const -> std::enable_if_t<
        !tinyrefl::meta::is_detected_v<callable_with_kind, Entity> &&
        !tinyrefl::meta::is_detected_v<callable_without_kind, Entity>>
    {
        tinyrefl::meta::foreach(Entity{}.children(), *this);
    }

    Visitor visitor;
};

template<typename Visitor>
constexpr constexpr_recurive_visitor<std::decay_t<Visitor>>
    make_constexpr_recurive_visitor(Visitor&& visitor)
{
    return {std::forward<Visitor>(visitor)};
}

template<typename Visitor>
struct constexpr_kind_restricted_visitor_base
{
    template<typename Visitor_>
    constexpr constexpr_kind_restricted_visitor_base(Visitor_&& visitor)
        : visitor{std::forward<Visitor_>(visitor)}
    {
    }

    Visitor visitor;

    template<typename Entity>
    constexpr void call_visitor(Entity entity) const
    {
        visitor(entity);
    }
};

template<typename Visitor, typename Kinds>
struct constexpr_kind_restricted_visitor;

template<typename Visitor, tinyrefl::entities::entity_kind Kind>
struct constexpr_kind_restricted_visitor<
    Visitor,
    tinyrefl::meta::list<tinyrefl::entities::kind_constant<Kind>>>
    : public constexpr_kind_restricted_visitor_base<Visitor>
{
    using constexpr_kind_restricted_visitor_base<
        Visitor>::constexpr_kind_restricted_visitor_base;

    template<typename Entity>
    constexpr void operator()(
        const Entity& entity, tinyrefl::entities::kind_constant<Kind>) const
    {
        constexpr_kind_restricted_visitor_base<Visitor>::call_visitor(entity);
    }
};

template<
    typename Visitor,
    tinyrefl::entities::entity_kind First,
    tinyrefl::entities::entity_kind Second,
    tinyrefl::entities::entity_kind... Tail>
struct constexpr_kind_restricted_visitor<
    Visitor,
    tinyrefl::meta::list<
        tinyrefl::entities::kind_constant<First>,
        tinyrefl::entities::kind_constant<Second>,
        tinyrefl::entities::kind_constant<Tail>...>>
    : public constexpr_kind_restricted_visitor<
          Visitor,
          tinyrefl::meta::list<
              tinyrefl::entities::kind_constant<Second>,
              tinyrefl::entities::kind_constant<Tail>...>>
{
    using base = constexpr_kind_restricted_visitor<
        Visitor,
        tinyrefl::meta::list<
            tinyrefl::entities::kind_constant<Second>,
            tinyrefl::entities::kind_constant<Tail>...>>;

    using base::base;
    using base::operator();

    template<typename Entity>
    constexpr void operator()(
        const Entity& entity, tinyrefl::entities::kind_constant<First>) const
    {
        constexpr_kind_restricted_visitor_base<Visitor>::call_visitor(entity);
    }
};

template<tinyrefl::entities::entity_kind... Kind, typename Visitor>
constexpr auto make_constexpr_kind_restricted_visitor(Visitor&& visitor)
{
    return constexpr_kind_restricted_visitor<
        std::decay_t<Visitor>,
        tinyrefl::meta::list<tinyrefl::entities::kind_constant<Kind>...>>{
        std::forward<Visitor>(visitor)};
}

} // namespace impl

template<typename... Entities, typename... Visitors>
constexpr void
    visit(const std::tuple<Entities...>& entities, Visitors&&... visitors)
{
    tinyrefl::meta::foreach(
        entities,
        tinyrefl::impl::make_constexpr_visitor(tinyrefl::overloaded_function(
            std::forward<Visitors>(visitors)...)));
}

template<typename Entity, typename... Visitors>
constexpr void visit(Entity entity, Visitors&&... visitors)
{
    tinyrefl::visit(entity.children(), std::forward<Visitors>(visitors)...);
}

template<tinyrefl::hash_t Id, typename... Visitors>
constexpr void visit(tinyrefl::hash_constant<Id> id, Visitors&&... visitors)
{
    tinyrefl::visit(
        tinyrefl::metadata_by_id(id), std::forward<Visitors>(visitors)...);
}

template<typename T, typename... Visitors>
constexpr void visit(Visitors&&... visitors)
{
    tinyrefl::visit(
        tinyrefl::metadata<T>(), std::forward<Visitors>(visitors)...);
}

template<typename Entity, typename... Visitors>
constexpr void recursive_visit(Entity entity, Visitors&&... visitors)
{
    const auto visitor = tinyrefl::impl::make_constexpr_recurive_visitor(
        tinyrefl::overloaded_function_default(
            std::forward<Visitors>(visitors)...));

    visitor(entity);
}

template<typename... Entities, typename... Visitors>
constexpr void recursive_visit(
    const std::tuple<Entities...>& entities, Visitors&&... visitors)
{
    tinyrefl::meta::foreach(
        entities,
        tinyrefl::impl::make_constexpr_recurive_visitor(
            tinyrefl::overloaded_function_default(
                std::forward<Visitors>(visitors)...)));
}

template<tinyrefl::hash_t Id, typename... Visitors>
constexpr void
    recursive_visit(tinyrefl::hash_constant<Id> id, Visitors&&... visitors)
{
    tinyrefl::recursive_visit(
        tinyrefl::metadata_by_id(id), std::forward<Visitors>(visitors)...);
}

template<typename T, typename... Visitors>
constexpr void recursive_visit(Visitors&&... visitors)
{
    tinyrefl::recursive_visit(
        tinyrefl::metadata<T>(), std::forward<Visitors>(visitors)...);
}

template<tinyrefl::hash_t Id, typename... Visitors>
constexpr void visit(Visitors&&... visitors)
{
    tinyrefl::visit<tinyrefl::hash_constant<Id>>(
        std::forward<Visitors>(visitors)...);
}

template<tinyrefl::hash_t Id, typename... Visitors>
constexpr void recursive_visit(Visitors&&... visitors)
{
    tinyrefl::recursive_visit<tinyrefl::hash_constant<Id>>(
        std::forward<Visitors>(visitors)...);
}

#define TINYREFL_VISITOR_FUNCTION(kind, Kind)                          \
    template<typename Visitor>                                         \
    constexpr auto TINYREFL_PP_CAT(kind, _visitor)(Visitor && visitor) \
    {                                                                  \
        return tinyrefl::impl::make_constexpr_kind_restricted_visitor< \
            tinyrefl::entities::entity_kind::Kind>(                    \
            std::forward<Visitor>(visitor));                           \
    }

TINYREFL_VISITOR_FUNCTION(namespace, NAMESPACE);
TINYREFL_VISITOR_FUNCTION(function, FUNCTION);
TINYREFL_VISITOR_FUNCTION(variable, VARIABLE);
TINYREFL_VISITOR_FUNCTION(enum, ENUM);
TINYREFL_VISITOR_FUNCTION(enum_value, ENUM_VALUE);
TINYREFL_VISITOR_FUNCTION(class, CLASS);
TINYREFL_VISITOR_FUNCTION(constructor, CONSTRUCTOR);
TINYREFL_VISITOR_FUNCTION(member_function, MEMBER_FUNCTION);
TINYREFL_VISITOR_FUNCTION(member_variable, MEMBER_VARIABLE);
TINYREFL_VISITOR_FUNCTION(static_member_function, STATIC_MEMBER_FUNCTION);
TINYREFL_VISITOR_FUNCTION(static_member_variable, STATIC_MEMBER_VARIABLE);

#undef TINYREFL_VISITOR_FUNCTION

#ifndef TINYREFL_ENTITIES
#define TINYREFL_ENTITIES ::tinyrefl::meta::list<>
#endif // TINYREFL_ENTITIES

#define TINYREFL_VISIT_ENTITIES(...) \
    ::tinyrefl::visit(               \
        ::tinyrefl::meta::make_tuple(TINYREFL_ENTITIES{}), __VA_ARGS__)

#define TINYREFL_RECURSIVE_VISIT_ENTITIES(...) \
    ::tinyrefl::recursive_visit(               \
        ::tinyrefl::meta::make_tuple(TINYREFL_ENTITIES{}), __VA_ARGS__)

} // namespace tinyrefl

#endif // TINYREFL_VISITOR_HPP
