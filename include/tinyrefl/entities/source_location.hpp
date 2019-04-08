#ifndef TINYREFL_ENTITIES_SOURCE_LOCATION_HPP
#define TINYREFL_ENTITIES_SOURCE_LOCATION_HPP

#include <tinyrefl/backend/metadata.hpp>

namespace tinyrefl
{

namespace entities
{

template<typename File, std::size_t Line, std::size_t Column>
struct source_location
{
    constexpr source_location() = default;

    constexpr auto file() const
    {
        return tinyrefl::metadata<File>();
    }

    constexpr std::size_t line() const
    {
        return Line;
    }

    constexpr std::size_t column() const
    {
        return Column;
    }
};

} // namespace entities

} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_SOURCE_LOCATION_HPP
