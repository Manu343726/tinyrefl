#include "example.hpp"
#include <tinyrefl/api.hpp>
#include "example.hpp.tinyrefl"

namespace my_namespace
{

std::ostream& operator<<(std::ostream& os, const MyClass::Enum value)
{
    switch (value)
    {
    case MyClass::Enum::A:
        return os << "A";
    case MyClass::Enum::B:
        return os << "B";
    case MyClass::Enum::C:
        return os << "C";
    case MyClass::Enum::D:
        return os << "D";
    }

    return os << "WTF";
}

bool operator==(const MyClass& lhs, const MyClass& rhs)
{
    return tinyrefl::memberwise_equal(lhs, rhs);
}
}
