#include "metastuff.hpp"
#include "example.hpp"
#include <cassert>
#include <iostream>
#include "example.hpp.tinyrefl"

int main()
{
    static_assert(
        meta::isRegistered<example::C>(), "C class is not registered");

    if(!meta::hasMember<example::C>("hey_im_here"))
    {
        std::cerr << "example::C has no member named \"hey_im_here\"\n";
        return EXIT_FAILURE;
    }

    example::C c;
    c.hey_im_here = "foobar";

    std::cout << "example::C::hey_im_here: \""
              << meta::getMemberValue<std::string>(c, "hey_im_here") << "\"\n";
}
