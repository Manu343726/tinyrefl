#ifndef TINYREFL_BACKEND_METADATA_HPP_INCLUDED
#define TINYREFL_BACKEND_METADATA_HPP_INCLUDED

#include <tinyrefl/entities/attribute.hpp>
#include <tinyrefl/entities/entity_kind.hpp>
#include <tinyrefl/types/string.hpp>

#include <tuple>

namespace tinyrefl
{

namespace backend
{

struct no_metadata
    : public tinyrefl::entities::attributes_metadata<tinyrefl::meta::list<>>
{
    struct tinyrefl_entity_tag
    {
    };

    constexpr bool operator==(const no_metadata& other) const
    {
        return false;
    }

    constexpr bool operator!=(const no_metadata& other) const
    {
        return true;
    }

    struct dummy_source_location
    {
        constexpr dummy_source_location() = default;

        constexpr std::size_t line() const
        {
            return 0;
        }

        constexpr std::size_t column() const
        {
            return 0;
        }

        constexpr no_metadata file() const
        {
            return {};
        }
    };

    constexpr no_metadata() = default;

    constexpr bool has_metadata() const
    {
        return false;
    }

    constexpr no_metadata parent() const
    {
        return {};
    }

    // Fake children list so recursive AST visitors work with non reflected
    // entities: The entity appears as reflected with no children so it is
    // handled as a leaf node by the visitor.
    constexpr std::tuple<> children() const
    {
        return {};
    }

    constexpr std::size_t depth() const
    {
        return 0;
    }

    constexpr tinyrefl::entities::entity_kind kind() const
    {
        return tinyrefl::entities::entity_kind::UNKNOWN;
    }

    constexpr tinyrefl::string name() const
    {
        return "";
    }

    constexpr tinyrefl::string full_name() const
    {
        return "";
    }

    constexpr tinyrefl::string display_name() const
    {
        return "";
    }

    constexpr tinyrefl::string full_display_name() const
    {
        return "";
    }

    constexpr dummy_source_location source_location() const
    {
        return {};
    }

    constexpr no_metadata root() const
    {
        return {};
    }
};

template<typename T>
struct entity_metadata
{
    using type = tinyrefl::backend::no_metadata;
};

} // namespace backend

template<typename Type>
using metadata = typename tinyrefl::backend::entity_metadata<Type>::type;

template<tinyrefl::hash_t Id>
constexpr tinyrefl::metadata<tinyrefl::hash_constant<Id>>
    metadata_by_id(tinyrefl::hash_constant<Id>)
{
    return {};
}

template<typename Type>
constexpr auto has_metadata() -> std::enable_if_t<
    !std::is_base_of<tinyrefl::backend::no_metadata, tinyrefl::metadata<Type>>::
        value,
    bool>
{
    return true;
}

template<typename Type>
constexpr auto has_metadata() -> std::enable_if_t<
    std::is_base_of<tinyrefl::backend::no_metadata, tinyrefl::metadata<Type>>::
        value,
    bool>
{
    return false;
}

template<tinyrefl::hash_t Hash>
constexpr bool has_metadata(
    tinyrefl::hash_constant<Hash> = tinyrefl::hash_constant<Hash>{})
{
    return has_metadata<tinyrefl::hash_constant<Hash>>();
}

} // namespace tinyrefl

#endif // TINYREFL_BACKEND_METADATA_HPP_INCLUDED
