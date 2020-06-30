#ifndef TINYREFL_ENTITIES_NAMESPACE_HPP_INCLUDED
#define TINYREFL_ENTITIES_NAMESPACE_HPP_INCLUDED

#include <tinyrefl/backend/metadata.hpp>
#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/types/string.hpp>
#include <tinyrefl/utils/meta.hpp>
#include <tinyrefl/utils/preprocessor.hpp>

namespace tinyrefl
{

namespace entities
{

template<
    typename Name,
    typename FullName,
    typename Parent,
    typename SourceLocation,
    typename DisplayName,
    typename FullDisplayName,
    typename Functions,
    typename Variables,
    typename Classes,
    typename Enums,
    typename Namespaces,
    typename Attributes>
struct namespace_ : public tinyrefl::entities::entity_with_attributes<
                        Attributes,
                        tinyrefl::entities::entity_kind::NAMESPACE,
                        Name,
                        FullName,
                        Parent,
                        tinyrefl::meta::pack_foldl_t<
                            tinyrefl::meta::defer<tinyrefl::meta::cat>,
                            Functions,
                            Variables,
                            Classes,
                            Enums,
                            Namespaces>,
                        SourceLocation,
                        DisplayName,
                        FullDisplayName>
{
    using functions  = Functions;
    using variables  = Variables;
    using classes    = Classes;
    using enums      = Enums;
    using namespaces = Namespaces;
};

namespace impl
{

template<typename LhsNamespace, typename RhsNamespace>
struct merge_namespaces;

template<
    typename Name,
    typename FullName,
    typename LhsParent,
    typename RhsParent,
    typename LhsSourceLocation,
    typename RhsSourceLocation,
    typename LhsDisplayName,
    typename RhsDisplayName,
    typename LhsFullDisplayName,
    typename RhsFullDisplayName,
    typename LhsFunctions,
    typename LhsVariables,
    typename LhsClasses,
    typename LhsEnums,
    typename LhsNamespaces,
    typename LhsAttributes,
    typename RhsFunctions,
    typename RhsVariables,
    typename RhsClasses,
    typename RhsEnums,
    typename RhsNamespaces,
    typename RhsAttributes>
struct merge_namespaces<
    tinyrefl::entities::namespace_<
        Name,
        FullName,
        LhsParent,
        LhsSourceLocation,
        LhsDisplayName,
        LhsFullDisplayName,
        LhsFunctions,
        LhsVariables,
        LhsClasses,
        LhsEnums,
        LhsNamespaces,
        LhsAttributes>,
    tinyrefl::entities::namespace_<
        Name,
        FullName,
        RhsParent,
        RhsSourceLocation,
        RhsDisplayName,
        RhsFullDisplayName,
        RhsFunctions,
        RhsVariables,
        RhsClasses,
        RhsEnums,
        RhsNamespaces,
        RhsAttributes>>
{
    using type = tinyrefl::entities::namespace_<
        Name,
        FullName,
        LhsParent,
        LhsSourceLocation,
        LhsDisplayName,
        LhsFullDisplayName,
        tinyrefl::meta::remove_duplicates_t<
            tinyrefl::meta::cat_t<LhsFunctions, RhsFunctions>>,
        tinyrefl::meta::remove_duplicates_t<
            tinyrefl::meta::cat_t<LhsVariables, RhsVariables>>,
        tinyrefl::meta::remove_duplicates_t<
            tinyrefl::meta::cat_t<LhsClasses, RhsClasses>>,
        tinyrefl::meta::remove_duplicates_t<
            tinyrefl::meta::cat_t<LhsEnums, RhsEnums>>,
        tinyrefl::meta::remove_duplicates_t<
            tinyrefl::meta::cat_t<LhsNamespaces, RhsNamespaces>>,
        tinyrefl::meta::remove_duplicates_t<
            tinyrefl::meta::cat_t<LhsAttributes, RhsAttributes>>>;
};

template<typename Namespaces>
struct merge_all_namespaces;

template<
    template<typename...>
    class Sequence,
    typename First,
    typename Second,
    typename... Tail>
struct merge_all_namespaces<Sequence<First, Second, Tail...>>
{
    using type = typename merge_namespaces<
        First,
        typename merge_all_namespaces<Sequence<Second, Tail...>>::type>::type;
};

template<template<typename...> class Sequence, typename Namespace>
struct merge_all_namespaces<Sequence<Namespace>>
{
    using type = Namespace;
};

template<tinyrefl::hash_t FullName>
struct filter_by_namespace_name
{
    template<typename Entity>
    struct apply
    {
        using type = tinyrefl::meta::bool_<(
            Entity{}.full_name().hash() == FullName &&
            Entity{}.kind() == tinyrefl::entities::entity_kind::NAMESPACE)>;
    };
};

} // namespace impl

template<typename LhsNamespace, typename RhsNamespace>
using merge_namespaces = typename ::tinyrefl::entities::impl::
    merge_namespaces<LhsNamespace, RhsNamespace>::type;

template<typename Namespaces>
using merge_all_namespaces =
    typename ::tinyrefl::entities::impl::merge_all_namespaces<Namespaces>::type;

#ifndef TINYREFL_ENTITIES
#define TINYREFL_ENTITIES ::tinyrefl::meta::list<>
#endif // TINYREFL_ENTITIES

#define TINYREFL_ENTITIES_NAMESPACE(...)                                       \
    ::tinyrefl::entities::merge_all_namespaces<::tinyrefl::meta::filter_t<     \
        ::tinyrefl::entities::impl::filter_by_namespace_name<tinyrefl::hash(   \
            TINYREFL_PP_STR(__VA_ARGS__))>,                                    \
        TINYREFL_ENTITIES>>                                                    \
    {                                                                          \
    }

} // namespace entities
} // namespace tinyrefl
#endif // TINYREFL_ENTITIES_NAMESPACE_HPP_INCLUDED
