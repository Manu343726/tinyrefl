#ifndef TINYREFL_ENTITIES_CLASS_HPP_INCLUDED
#define TINYREFL_ENTITIES_CLASS_HPP_INCLUDED

#include <tinyrefl/backend/metadata.hpp>
#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/entities/type.hpp>
#include <tinyrefl/utils/meta.hpp>

namespace tinyrefl
{

namespace entities
{

namespace detail
{

template<
    typename Name,
    typename FullName,
    typename Parent,
    typename SourceLocation,
    typename Class,
    typename BaseClasses,
    typename Constructors,
    typename MemberFunctions,
    typename MemberVariables,
    typename StaticMemberFunctions,
    typename StaticMemberVariables,
    typename Classes,
    typename Enums,
    typename Attributes>
using class_base = tinyrefl::entities::entity_with_attributes<
    Attributes,
    tinyrefl::entities::entity_kind::CLASS,
    Name,
    FullName,
    Parent,
    tinyrefl::meta::pack_foldl_t<
        tinyrefl::meta::defer<tinyrefl::meta::cat>,
        Constructors,
        MemberFunctions,
        MemberVariables,
        StaticMemberFunctions,
        StaticMemberVariables,
        Classes,
        Enums>,
    SourceLocation>;

struct ClassToAllChildren
{
    template<typename Entity>
    constexpr auto operator()(const Entity& entity) const
    {
        return entity.all_children();
    }
};

template<typename... Bases>
constexpr auto collect_inherited_children(const std::tuple<Bases...>& bases)
{
    return tinyrefl::meta::tuple_flatmap(bases, ClassToAllChildren{});
}

template<tinyrefl::entities::entity_kind Kind, typename... Bases>
constexpr auto collect_inherited_children(const std::tuple<Bases...>& bases)
{
    return tinyrefl::meta::tuple_filter(
        collect_inherited_children(bases),
        ::tinyrefl::entities::impl::EntityFilter<Kind>{});
}
} // namespace detail

template<typename Class>
struct no_reflected_base_class : public tinyrefl::entities::type<Class>
{
    using class_type = Class;

    constexpr std::tuple<> bases() const
    {
        return {};
    }

    constexpr std::tuple<> inherited_children() const
    {
        return {};
    }

    constexpr std::tuple<> all_children() const
    {
        return {};
    }

    template<tinyrefl::entities::entity_kind Kind>
    constexpr std::tuple<> inherited_children() const
    {
        return {};
    }

    template<tinyrefl::entities::entity_kind Kind>
    constexpr std::tuple<> all_children() const
    {
        return {};
    }

    constexpr tinyrefl::entities::entity_kind kind() const
    {
        return tinyrefl::entities::entity_kind::BASE_CLASS;
    }
};

template<typename Class>
struct base_class : public tinyrefl::entities::type<Class>
{
    constexpr tinyrefl::entities::entity_kind kind() const
    {
        return tinyrefl::entities::entity_kind::BASE_CLASS;
    }
};

template<
    typename Name,
    typename FullName,
    typename Parent,
    typename SourceLocation,
    typename Class,
    typename BaseClasses,
    typename Constructors,
    typename MemberFunctions,
    typename MemberVariables,
    typename StaticMemberFunctions,
    typename StaticMemberVariables,
    typename Classes,
    typename Enums,
    typename Attributes>
struct class_ : public tinyrefl::entities::detail::class_base<
                    Name,
                    FullName,
                    Parent,
                    SourceLocation,
                    Class,
                    BaseClasses,
                    Constructors,
                    MemberFunctions,
                    MemberVariables,
                    StaticMemberFunctions,
                    StaticMemberVariables,
                    Classes,
                    Enums,
                    Attributes>,
                public tinyrefl::entities::type_entity<Class>
{
private:
    using Base = tinyrefl::entities::detail::class_base<
        Name,
        FullName,
        Parent,
        SourceLocation,
        Class,
        BaseClasses,
        Constructors,
        MemberFunctions,
        MemberVariables,
        StaticMemberFunctions,
        StaticMemberVariables,
        Classes,
        Enums,
        Attributes>;

public:
    using class_type              = Class;
    using base_classes            = BaseClasses;
    using constructors            = Constructors;
    using member_functions        = MemberFunctions;
    using member_variables        = MemberVariables;
    using static_member_functions = StaticMemberFunctions;
    using static_member_variables = StaticMemberVariables;
    using member_classes          = tinyrefl::meta::fmap_t<
        tinyrefl::meta::defer<tinyrefl::backend::entity_metadata>,
        Classes>;
    using member_enums = tinyrefl::meta::fmap_t<
        tinyrefl::meta::defer<tinyrefl::backend::entity_metadata>,
        Enums>;

    using Base::full_name;
    using Base::name;

    constexpr auto bases() const
    {
        return tinyrefl::meta::make_tuple(
            tinyrefl::meta::fmap_t<
                tinyrefl::meta::defer<BaseClassMetadata>,
                base_classes>{});
    }

    constexpr auto inherited_children() const
    {
        return detail::collect_inherited_children(bases());
    }

    constexpr auto all_children() const
    {
        return std::tuple_cat(inherited_children(), Base::children());
    }

    template<tinyrefl::entities::entity_kind Kind>
    constexpr auto inherited_children() const
    {
        return detail::collect_inherited_children<Kind>(bases());
    }

    template<tinyrefl::entities::entity_kind Kind>
    constexpr auto all_children() const
    {
        return std::tuple_cat(
            inherited_children<Kind>(), Base::template children<Kind>());
    }

private:
    template<
        typename BaseClass,
        bool HasMetadata = tinyrefl::has_metadata<BaseClass>()>
    struct BaseClassMetadataImpl
    {
        using type = base_class<BaseClass>;
    };

    template<typename BaseClass>
    struct BaseClassMetadataImpl<BaseClass, false>
    {
        using type = no_reflected_base_class<BaseClass>;
    };

    template<typename BaseClass>
    using BaseClassMetadata = BaseClassMetadataImpl<BaseClass>;
};
} // namespace entities
} // namespace tinyrefl
#endif // TINYREFL_ENTITIES_CLASS_HPP_INCLUDED
