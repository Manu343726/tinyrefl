#include "boost_fusion.hpp"
#include "example.hpp"
#include <boost/fusion/include/algorithm.hpp>
#include <iostream>
#include "example.hpp.tinyrefl"

int main()
{
    example::C c;

    boost::fusion::for_each(
        c, [](const auto& member) { std::cout << member << std::endl; });
}
