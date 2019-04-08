#ifndef TINYREFL_ENTITIES_ENTITY_HPP_INCLUDED
#define TINYREFL_ENTITIES_ENTITY_HPP_INCLUDED

#include <tinyrefl/backend/metadata.hpp>
#include <tinyrefl/entities/attribute.hpp>
#include <tinyrefl/entities/entity_kind.hpp>
#include <tinyrefl/types/name.hpp>
#include <tinyrefl/types/string.hpp>
#include <tinyrefl/types/type.hpp>

namespace tinyrefl
{

namespace entities
{
template<
    tinyrefl::entities::entity_kind Kind,
    typename Name,
    typename FullName,
    typename Parent_,
    typename Children_,
    typename SourceLocation_,
    typename DisplayName     = void,
    typename FullDisplayName = void>
struct entity
{
    struct tinyrefl_entity_tag
    {
    };

    using Parent         = tinyrefl::metadata<Parent_>;
    using Children       = Children_;
    using SourceLocation = SourceLocation_;

    constexpr Parent parent() const
    {
        return {};
    }

    constexpr auto children() const
    {
        return tinyrefl::meta::make_tuple(Children{});
    }

    constexpr tinyrefl::entities::entity_kind kind() const
    {
        return Kind;
    }

    constexpr tinyrefl::string name() const
    {
        return tinyrefl::string_constant<Name>();
    }

    constexpr tinyrefl::string full_name() const
    {
        return tinyrefl::string_constant<FullName>();
    }

    constexpr tinyrefl::string display_name() const
    {
        return _display_name(tinyrefl::Type<DisplayName>());
    }

    constexpr tinyrefl::string full_display_name() const
    {
        return _full_display_name(tinyrefl::Type<FullDisplayName>());
    }

    constexpr SourceLocation source_location() const
    {
        return {};
    }

    constexpr auto root() const
    {
        return source_location().file();
    }

private:
    template<typename _DisplayName>
    static constexpr tinyrefl::string
        _display_name(tinyrefl::Type<_DisplayName>)
    {
        return tinyrefl::string_constant<_DisplayName>();
    }

    static constexpr tinyrefl::string _display_name(tinyrefl::Type<void>)
    {
        return tinyrefl::string_constant<Name>();
    }

    template<typename _FullDisplayName>
    static constexpr tinyrefl::string
        _full_display_name(tinyrefl::Type<_FullDisplayName>)
    {
        return tinyrefl::string_constant<_FullDisplayName>();
    }

    static constexpr tinyrefl::string _full_display_name(tinyrefl::Type<void>)
    {
        return tinyrefl::string_constant<FullName>();
    }
};

template<
    typename Attributes,
    tinyrefl::entities::entity_kind Kind,
    typename Name,
    typename FullName,
    typename Parent,
    typename Children,
    typename SourceLocation,
    typename DisplayName     = void,
    typename FullDisplayName = void>
struct entity_with_attributes : public entity<
                                    Kind,
                                    Name,
                                    FullName,
                                    Parent,
                                    Children,
                                    SourceLocation,
                                    DisplayName,
                                    FullDisplayName>,
                                public attributes_metadata<Attributes>
{
};

using no_children   = tinyrefl::meta::list<>;
using no_parent     = tinyrefl::hash_constant<0>;
using no_attributes = tinyrefl::meta::list<>;

template<typename T, typename = void>
struct is_entity : public std::false_type
{
};

template<typename T>
struct is_entity<T, std::void_t<typename T::tinyrefl_entity_tag>>
    : public std::true_type
{
};

} // namespace entities

} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_ENTITY_HPP_INCLUDED
