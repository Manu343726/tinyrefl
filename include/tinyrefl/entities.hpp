#ifndef TINYREFL_ENTITIES_HPP_INCLUDED
#define TINYREFL_ENTITIES_HPP_INCLUDED

#include <tinyrefl/entities/entity_kind.hpp>
#include <tinyrefl/entities/namespace.hpp>
#include <tinyrefl/matchers.hpp>
#include <tinyrefl/types/string.hpp>
#include <tinyrefl/utils/meta.hpp>

#ifndef TINYREFL_ENTITIES
#error                                                                         \
    "Global entities list TINYREFL_ENTITIES not defined, make sure you have at least one metadata .tinyrefl file included in your translation unit"
#define TINYREFL_ENTITIES                                                      \
    tinyrefl::meta::list<> // dummy list to hide IDE diagnostics
#endif                     // TINYREFL_ENTITIES

namespace tinyrefl
{

namespace impl
{

template<typename Entity>
struct namespace_filter
{
    using type = tinyrefl::meta::bool_<
        Entity{}.kind() == tinyrefl::entities::entity_kind::NAMESPACE>;
};

template<tinyrefl::hash_t Fullname>
struct specific_namespace_filter
{
    template<typename Entity>
    struct apply
    {
        using type = tinyrefl::meta::bool_<
            Entity{}.kind() == tinyrefl::entities::entity_kind::NAMESPACE &&
            Entity{}.full_name().hash() == Fullname>;
    };
};

using all_entities = TINYREFL_ENTITIES;
using namespaces   = tinyrefl::meta::filter_t<
    tinyrefl::meta::defer<tinyrefl::impl::namespace_filter>,
    all_entities>;

template<tinyrefl::hash_t FullName>
using namespace_ =
    tinyrefl::entities::merge_all_namespaces<tinyrefl::meta::filter_t<
        tinyrefl::impl::specific_namespace_filter<FullName>,
        namespaces>>;

struct type_tag_to_entity
{
    constexpr type_tag_to_entity() = default;

    template<typename Entity>
    constexpr Entity operator()(tinyrefl::type_tag<Entity>) const
    {
        return {};
    }
};

} // namespace impl

constexpr auto all_entities = tinyrefl::meta::tuple_map(
    tinyrefl::meta::typelist_to_tuple(impl::all_entities{}),
    impl::type_tag_to_entity{});

constexpr auto namespaces = tinyrefl::meta::tuple_map(
    tinyrefl::meta::typelist_to_tuple(impl::namespaces{}),
    impl::type_tag_to_entity{});

template<tinyrefl::hash_t FullName>
constexpr auto namespace_ = impl::namespace_<FullName>{};

template<typename Matcher>
constexpr auto matches(const Matcher& matcher)
{
    return tinyrefl::meta::tuple_filter(tinyrefl::all_entities, matcher);
}

} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_HPP_INCLUDED
