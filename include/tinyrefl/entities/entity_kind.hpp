#ifndef TINYREFL_ENTITIES_ENTITY_KIND_HPP_INCLUDED
#define TINYREFL_ENTITIES_ENTITY_KIND_HPP_INCLUDED

#include <ostream>
#include <tinyrefl/types/static_value.hpp>

namespace tinyrefl
{

namespace entities
{
enum class entity_kind
{
    NAMESPACE,
    CLASS,
    ENUM,
    ENUM_VALUE,
    BASE_CLASS,
    MEMBER_VARIABLE,
    MEMBER_CLASS,
    MEMBER_FUNCTION,
    CONSTRUCTOR,
    MEMBER_ENUM,
    OBJECT,
    STATIC_MEMBER_FUNCTION,
    STATIC_MEMBER_VARIABLE,
    FUNCTION,
    VARIABLE,
    FILE,
    UNKNOWN
};

template<tinyrefl::entities::entity_kind Kind>
using kind_constant = TINYREFL_STATIC_VALUE(Kind);

template<tinyrefl::entities::entity_kind Kind>
constexpr tinyrefl::entities::kind_constant<Kind> kind_constant_v =
    tinyrefl::entities::kind_constant<Kind>{};

inline std::ostream& operator<<(std::ostream& os, const entity_kind e)
{
    switch(e)
    {
    case entity_kind::NAMESPACE:
        return os << "namespace";
    case entity_kind::CLASS:
        return os << "class";
    case entity_kind::ENUM:
        return os << "enum";
    case entity_kind::ENUM_VALUE:
        return os << "enum value";
    case entity_kind::BASE_CLASS:
        return os << "base class";
    case entity_kind::MEMBER_FUNCTION:
        return os << "member function";
    case entity_kind::CONSTRUCTOR:
        return os << "constructor";
    case entity_kind::MEMBER_VARIABLE:
        return os << "member variable";
    case entity_kind::MEMBER_CLASS:
        return os << "member class";
    case entity_kind::MEMBER_ENUM:
        return os << "member enum";
    case entity_kind::OBJECT:
        return os << "object";
    case entity_kind::STATIC_MEMBER_FUNCTION:
        return os << "static member function";
    case entity_kind::STATIC_MEMBER_VARIABLE:
        return os << "static member variable";
    case entity_kind::VARIABLE:
        return os << "variable";
    case entity_kind::FUNCTION:
        return os << "function";
    case entity_kind::FILE:
        return os << "file";
    case entity_kind::UNKNOWN:
        return os << "unknown";
    }

    return os;
}
} // namespace entities
} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_ENTITY_KIND_HPP_INCLUDED
