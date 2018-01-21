#include <tinyrefl/api.hpp>
#include <iostream>
#include "example.hpp"
#include "example.hpp.tinyrefl"


template<typename T,
    typename = typename std::enable_if<tinyrefl::has_metadata<T>::value>::type>
std::ostream& operator<<(std::ostream& os, const T& object)
{
    os << "{";

    tinyrefl::visit_object(object, [&os](const std::string& name, auto depth, auto member, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        os << "\"" << name << "\": " << member << ",";
    });

    return os;
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
}


