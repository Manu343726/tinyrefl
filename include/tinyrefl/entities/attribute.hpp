#ifndef TINYREFL_ENTITIES_ATTRIBUTE_HPP_INCLUDED
#define TINYREFL_ENTITIES_ATTRIBUTE_HPP_INCLUDED

#include <tinyrefl/types/array.hpp>
#include <tinyrefl/types/name.hpp>
#include <tinyrefl/types/string.hpp>

namespace tinyrefl
{

namespace backend
{

template<typename Hash>
constexpr std::nullptr_t attribute_instance = nullptr;
}

namespace entities
{

struct attribute
{
    constexpr attribute(
        const tinyrefl::string&                       name,
        const tinyrefl::string&                       full_name,
        const tinyrefl::string&                       namespace_,
        const tinyrefl::string&                       full_attribute,
        const tinyrefl::array_view<tinyrefl::string>& arguments)
        : _name{name},
          _full_name{full_name},
          _namespace_{namespace_},
          _full_attribute{full_attribute},
          _arguments{arguments}
    {
    }

    constexpr tinyrefl::string name() const
    {
        return _name;
    }

    constexpr tinyrefl::string full_name() const
    {
        return _full_name;
    }

    constexpr tinyrefl::string namespace_() const
    {
        return _namespace_;
    }

    constexpr tinyrefl::string full_attribute() const
    {
        return _full_attribute;
    }

    constexpr tinyrefl::array_view<tinyrefl::string> arguments() const
    {
        return _arguments;
    }

    friend constexpr bool
        operator==(const attribute& lhs, const tinyrefl::string& full_name)
    {
        return full_name == lhs.full_name();
    }

    friend constexpr bool operator==(const attribute& lhs, const attribute& rhs)
    {
        return lhs.full_name() == rhs.full_name();
    }

    friend constexpr bool
        operator!=(const attribute& lhs, const tinyrefl::string& name)
    {
        return !(lhs == name);
    }

    friend constexpr bool operator!=(const attribute& lhs, const attribute& rhs)
    {
        return !(lhs == rhs);
    }

private:
    tinyrefl::string                       _name;
    tinyrefl::string                       _full_name;
    tinyrefl::string                       _namespace_;
    tinyrefl::string                       _full_attribute;
    tinyrefl::array_view<tinyrefl::string> _arguments;
};

template<
    typename Name,
    typename FullName,
    typename Namespace,
    typename FullAttribute,
    typename Args>
struct attribute_metadata : public tinyrefl::entities::attribute
{
    constexpr attribute_metadata()
        : attribute{
              tinyrefl::string_constant<Name>(),
              tinyrefl::string_constant<FullName>(),
              tinyrefl::string_constant<Namespace>(),
              tinyrefl::string_constant<FullAttribute>(),
              tinyrefl::typelist_to_array_of_strings<Args>::value}
    {
    }

    static constexpr const auto& instance()
    {
        return ::tinyrefl::backend::attribute_instance<FullAttribute>;
    }
};

template<typename Metadata>
constexpr const attribute*
    find_attribute(const Metadata& metadata, const tinyrefl::string& attribute)
{
    return tinyrefl::find(metadata.attributes(), attribute);
}

template<typename Metadata>
constexpr bool
    has_attribute(const Metadata& metadata, const tinyrefl::string& attribute)
{
    return find_attribute(metadata, attribute) != metadata.attributes().end();
}

template<typename Derived>
struct attributes_interface
{
    constexpr bool has_attribute(const tinyrefl::string& name) const
    {
        return tinyrefl::entities::has_attribute(
            static_cast<const Derived&>(*this), name);
    }

    constexpr const tinyrefl::entities::attribute&
        attribute(tinyrefl::string name) const
    {
        return *tinyrefl::entities::find_attribute(
            static_cast<const Derived&>(*this), name);
    }

    constexpr const tinyrefl::entities::attribute&
        attribute(std::size_t i) const
    {
        return static_cast<const Derived*>(this)->attributes()[i];
    }
};

namespace impl
{

struct attribute_metadata_to_attribute
{
    template<typename AttributeMetadata>
    static constexpr tinyrefl::entities::attribute
        transform(const AttributeMetadata& metadata)
    {
        return static_cast<const tinyrefl::entities::attribute&>(metadata);
    }
};

template<typename... AttributeMetadata>
constexpr auto make_tuple(tinyrefl::meta::list<AttributeMetadata...>)
{
    return std::tie(AttributeMetadata::instance()...);
}

} // namespace impl

template<typename Attributes>
struct attributes_metadata
    : public attributes_interface<attributes_metadata<Attributes>>
{

    constexpr tinyrefl::array_view<tinyrefl::entities::attribute>
        attributes() const
    {
        return tinyrefl::typelist_to_array<
            Attributes,
            tinyrefl::entities::attribute,
            tinyrefl::entities::impl::attribute_metadata_to_attribute>::value;
    }

    constexpr auto attribute_objects() const
    {
        return tinyrefl::entities::impl::make_tuple(Attributes{});
    }
};
} // namespace entities

} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_ATTRIBUTE_HPP_INCLUDED
