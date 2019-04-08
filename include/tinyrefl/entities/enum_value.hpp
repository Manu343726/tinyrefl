#ifndef TINYREFL_ENTITIES_ENUM_VALUE_HPP_INCLUDED
#define TINYREFL_ENTITIES_ENUM_VALUE_HPP_INCLUDED

#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/types/string.hpp>

namespace tinyrefl
{

namespace entities
{

template<typename Enum>
struct enum_value : attributes_interface<enum_value<Enum>>
{
    constexpr enum_value(
        const Enum                                                value,
        const tinyrefl::string&                                   name,
        const tinyrefl::string&                                   full_name,
        const tinyrefl::array_view<tinyrefl::entities::attribute> attributes)
        : _value{value},
          _name{name},
          _full_name{full_name},
          _attributes{attributes}
    {
    }

    constexpr Enum value() const
    {
        return _value;
    }

    constexpr auto underlying_value() const
    {
        return static_cast<std::underlying_type_t<Enum>>(_value);
    }

    constexpr tinyrefl::string name() const
    {
        return _name;
    }

    constexpr tinyrefl::string full_name() const
    {
        return _full_name;
    }

    constexpr tinyrefl::array_view<tinyrefl::entities::attribute>
        attributes() const
    {
        return _attributes;
    }

private:
    Enum                                                _value;
    tinyrefl::string                                    _name;
    tinyrefl::string                                    _full_name;
    tinyrefl::array_view<tinyrefl::entities::attribute> _attributes;
};

template<
    typename Name,
    typename FullName,
    typename Parent,
    typename SourceLocation,
    typename Enum,
    typename Value,
    typename Attributes>
struct enum_value_metadata : public tinyrefl::entities::entity_with_attributes<
                                 Attributes,
                                 tinyrefl::entities::entity_kind::ENUM_VALUE,
                                 Name,
                                 FullName,
                                 Parent,
                                 tinyrefl::entities::no_children,
                                 SourceLocation>,
                             public enum_value<Enum>
{
private:
    using entity_with_attributes = tinyrefl::entities::entity_with_attributes<
        Attributes,
        tinyrefl::entities::entity_kind::ENUM_VALUE,
        Name,
        FullName,
        Parent,
        tinyrefl::entities::no_children,
        SourceLocation>;

public:
    constexpr enum_value_metadata()
        : enum_value<Enum>{Value::value,
                           tinyrefl::string_constant<Name>(),
                           tinyrefl::string_constant<FullName>(),
                           entity_with_attributes::attributes()}
    {
    }

    // Get name() and full_name() from enum_value<> base to
    // avoid ambiguity with entity<> name() and full_name()
    using enum_value<Enum>::name;
    using enum_value<Enum>::full_name;

    using enum_type          = Enum;
    using value_static_value = Value;
};

namespace detail
{

struct get_enum_value
{
    template<typename EnumValueMetadata>
    static constexpr tinyrefl::entities::enum_value<
        typename EnumValueMetadata::enum_type>
        transform(const EnumValueMetadata& metadata)
    {
        return static_cast<const tinyrefl::entities::enum_value<
            typename EnumValueMetadata::enum_type>&>(metadata);
    }
};

} // namespace detail
} // namespace entities

} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_ENUM_VALUE_HPP_INCLUDED
