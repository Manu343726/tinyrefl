#include <tinyrefl/api.hpp>
#include <iostream>
#include <sstream>
#include "example.hpp"
#include "example.hpp.tinyrefl"


template<typename T,
    typename = typename std::enable_if<tinyrefl::has_metadata<T>::value>::type>
std::ostream& operator<<(std::ostream& os, const T& object)
{
    std::ostringstream ss;
    ss << "{";

    tinyrefl::visit_object(object, [&ss](const std::string& name, auto depth, auto member, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        ss << "\"" << name << "\": " << member << ",";
    });

    auto str = ss.str();
    str.pop_back();
    return os << str << "}";
}

template<typename T,
    typename = typename std::enable_if<tinyrefl::has_metadata<T>::value>::type>
bool operator==(const T& lhs, const T& rhs)
{
    bool equal = true;

    tinyrefl::visit_objects(lhs, rhs)([&equal](const std::string& name, auto depth, auto members,
        CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        equal &= std::get<0>(members) == std::get<1>(members);
    });

    return equal;
}

int main()
{
    example::C c;

    static_assert(tinyrefl::has_metadata<example::C>::value, "");
    static_assert(!tinyrefl::has_metadata<int>::value, "");

    tinyrefl::visit_object(c, [](const std::string& name, auto depth, auto entity, auto entity_kind)
    {
        std::size_t hierarchy_level = depth;
        std::cout << std::string(2*hierarchy_level, ' ') << "at '" << name << "' (" << decltype(entity_kind)::value << ")\n";
    });

    std::cout << "c object dump: " << c << "\n";

    std::cout << "is c == c ?: " << std::boolalpha << (c == c) << "\n";
}


