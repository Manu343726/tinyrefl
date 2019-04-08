#include "example.hpp"
#include <tinyrefl/api.hpp>
#include "example.hpp.tinyrefl"

namespace example
{

int C::static_member_variable;

std::ostream& operator<<(std::ostream& os, const B& b)
{
    return os << tinyrefl::to_string(b);
}

std::ostream& operator<<(std::ostream& os, const Enum& value)
{
    return os << tinyrefl::to_string(value);
}

std::ostream& operator<<(std::ostream& os, const C::Enum& value)
{
    return os << tinyrefl::to_string(value);
}
} // namespace example
