#include "boost_fusion.hpp"
#include <boost/fusion/include/algorithm.hpp>
#include "example.hpp"
#include "example.hpp.tinyrefl"
#include <iostream>

int main()
{
    example::C c;

    boost::fusion::for_each(c, [](const auto& member)
    {
        std::cout << member << std::endl;
    });
}
