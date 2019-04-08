#ifndef TINYREFL_BACKEND_METADATA_HPP_INCLUDED
#define TINYREFL_BACKEND_METADATA_HPP_INCLUDED

#include <tinyrefl/entities/entity_kind.hpp>
#include <tinyrefl/types/string.hpp>

#include <tuple>

namespace tinyrefl
{

namespace backend
{

struct no_metadata
{
    struct tinyrefl_entity_tag
    {
    };

    constexpr no_metadata() = default;

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
    !std::is_same<tinyrefl::metadata<Type>, tinyrefl::backend::no_metadata>::
        value,
    bool>
{
    return true;
}

template<typename Type>
constexpr auto has_metadata() -> std::enable_if_t<
    std::is_same<tinyrefl::metadata<Type>, tinyrefl::backend::no_metadata>::
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
