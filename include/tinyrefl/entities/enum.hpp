#ifndef TINYREFL_ENTITIES_ENUM_HPP_INCLUDED
#define TINYREFL_ENTITIES_ENUM_HPP_INCLUDED

#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/entities/enum_value.hpp>
#include <tinyrefl/types/array.hpp>
#include <tinyrefl/types/array_view.hpp>

namespace tinyrefl
{

namespace entities
{

template<
    typename Name,
    typename FullName,
    typename Parent,
    typename SourceLocation,
    typename Enum,
    typename Values,
    typename Attributes>
struct enum_ : public tinyrefl::entities::entity_with_attributes<
                   Attributes,
                   tinyrefl::entities::entity_kind::ENUM,
                   Name,
                   FullName,
                   Parent,
                   Values,
                   SourceLocation>
{
    using enum_type = Enum;

    constexpr enum_() = default;

    constexpr tinyrefl::array_view<tinyrefl::entities::enum_value<Enum>>
        values() const
    {
        return tinyrefl::typelist_to_array<
            Values,
            tinyrefl::entities::enum_value<Enum>,
            tinyrefl::entities::detail::get_enum_value>::value;
    }

    constexpr const tinyrefl::entities::enum_value<Enum>
        operator[](const std::size_t index) const
    {
        return values()[index];
    }

    constexpr const tinyrefl::entities::enum_value<Enum>
        operator[](const tinyrefl::string name) const
    {
        for(const auto& value : values())
        {
            if(name == value.name() || name == value.full_name())
            {
                return value;
            }
        }

        throw std::out_of_range{"enum value not known"};
    }

    constexpr const tinyrefl::entities::enum_value<Enum>
        operator[](const Enum enum_value) const
    {
        for(const auto& value : values())
        {
            if(enum_value == value.value())
            {
                return value;
            }
        }

        throw std::out_of_range{"enum value not known"};
    }

    constexpr const tinyrefl::entities::enum_value<Enum>
        operator[](const std::underlying_type_t<Enum> enum_value) const
    {
        for(const auto& value : values())
        {
            if(enum_value == value.underlying_value())
            {
                return value;
            }
        }

        throw std::out_of_range{"enum value not known"};
    }

    using iterator = const tinyrefl::entities::enum_value<Enum>*;

    constexpr iterator begin() const
    {
        return values().begin();
    }

    constexpr iterator end() const
    {
        return values().end();
    }
};
} // namespace entities

} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_ENUM_HPP_INCLUDED
