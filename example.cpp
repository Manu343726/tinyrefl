#include <tinyrefl/api.hpp>
#include <iostream>
#include <sstream>
#include "example.hpp"
#include "example.hpp.tinyrefl"


template<typename T,
    typename = typename std::enable_if<tinyrefl::has_metadata<T>::value>::type>
std::ostream& operator<<(std::ostream& os, const T& object)
{
    std::size_t total = 0;
    std::size_t current = 0;
    os << "{";

    tinyrefl::visit_object(object, [&total](const std::string& name, auto depth, auto member, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        total++;
    });
    tinyrefl::visit_object(object, [&os, &current, total](const std::string& name, auto depth, auto member, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        os << "\"" << name << "\": " << member;

        if(current < total - 1)
        {
            os << ", ";
        }

        current++;
    });

    os << "}";

    return os;
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

    tinyrefl::visit_class<example::C>([](const std::string& name, auto depth, auto entity, auto entity_kind)
    {
        std::size_t hierarchy_level = depth;
        std::cout << std::string(2*hierarchy_level, ' ') << "at '" << name << "' (" << decltype(entity_kind)::value << ")\n";
    });

    std::cout << "c object dump: " << c << "\n";

    std::cout << "is c == c ?: " << std::boolalpha << (c == c) << "\n";

    std::cout << ctti::nameof<CTTI_STATIC_VALUE(example::Enum::A)>() << "\n";
    std::cout << ctti::detailed_nameof<CTTI_STATIC_VALUE(example::Enum::A)>().name() << "\n";

    for(std::size_t i = 0; i < $(example::Enum).count(); ++i)
    {
        std::cout << " - example::Enum[" << i << "]: \"" << $(example::Enum).get_name(i) << "\" ("
            << $(example::Enum).get_underlying_value(i) << ")\n";
    }

    static_assert( $(example::Enum).is_enumerated_value(42), "");
    static_assert(!$(example::Enum).is_enumerated_value(42*42), "");

    std::cout << "enum to string: " << tinyrefl::metadata<example::Enum>().get_name(example::Enum::A) << "\n";
    example::Enum enum_value_a = tinyrefl::metadata<example::Enum>().get_value("A");
}


