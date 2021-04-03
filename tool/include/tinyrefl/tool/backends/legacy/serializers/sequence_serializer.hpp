#ifndef TINYREFL_TOOL_SERIALIZERS_SEQUENCE_SERIALIZER_HPP_INCLUDED
#define TINYREFL_TOOL_SERIALIZERS_SEQUENCE_SERIALIZER_HPP_INCLUDED

#include <algorithm>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <string>
#include <tinyrefl/tool/backends/legacy/serializer.hpp>
#include <vector>

namespace tinyrefl
{

namespace tool
{

class sequence_serializer : public tinyrefl::tool::serializer
{
public:
    using tinyrefl::tool::serializer::serializer;

    template<typename Range>
    std::string raw_sequence(
        const Range&       range,
        const std::string& separator   = ", ",
        const std::string& item_format = "{}")
    {
        return raw_sequence(
            range,
            [&item_format](const auto& item) {
                return fmt::format(item_format, item);
            },
            separator);
    }

    template<typename Range, typename Transform>
    std::string raw_sequence(
        const Range&       range,
        Transform          transform,
        const std::string& separator = ", ")
    {
        std::vector<std::string> formatted_items;

        std::transform(
            std::begin(range),
            std::end(range),
            std::back_inserter(formatted_items),
            transform);

        return fmt::format(
            "{}",
            fmt::join(
                formatted_items.begin(), formatted_items.end(), separator));
    }

    template<typename Range>
    std::string sequence(const Range& range)
    {
        return fmt::format("TINYREFL_SEQUENCE(({}))", raw_sequence(range));
    }

    template<typename Range, typename Transform>
    std::string sequence(const Range& range, Transform transform)
    {
        return fmt::format(
            "TINYREFL_SEQUENCE(({}))", raw_sequence(range, transform));
    }
};
} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_SERIALIZERS_SEQUENCE_SERIALIZER_HPP_INCLUDED
