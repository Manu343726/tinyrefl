#ifndef TINYREFL_TOOL_MODEL_ENUM_HPP
#define TINYREFL_TOOL_MODEL_ENUM_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace tinyrefl
{
namespace tool
{
namespace model
{
struct enum_value
{
    std::string  full_name;
    std::string  name;
    std::int64_t value;

    std::string to_string() const;
};

struct enum_
{
    std::string             full_name;
    std::string             name;
    std::vector<enum_value> values;

    std::string to_string() const;
};
}
}
}

#endif // TINYREFL_TOOL_MODEL_ENUM_HPP
