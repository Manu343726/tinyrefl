#include <tinyrefl/api.hpp>
#include <iostream>
#include "example.hpp"
#include "example.hpp.tinyrefl"

int main()
{
    example::C c;

    static_assert(tinyrefl::has_metadata<example::C>::value, "");
    static_assert(!tinyrefl::has_metadata<int>::value, "");

    tinyrefl::visit_object(c, [](const auto& e, const std::string& name)
    {
        std::cout << "at '" << name << "' (type: " << ctti::nameof<decltype(e)>() << ")\n";
    });
}


