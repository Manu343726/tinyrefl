#include "boost_hana.hpp"
#include "example.hpp"
#include "example.hpp.tinyrefl"
#include <iostream>

int main()
{
    example::C c;

    boost::hana::for_each(c, boost::hana::fuse([](auto name, auto member)
    {
        std::cout << boost::hana::to<const char*>(name) << ": " << member << std::endl;
    }));
}
