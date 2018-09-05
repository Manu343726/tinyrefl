#include "boost_hana.hpp"
#include "example.hpp"
#include <iostream>
#include "example.hpp.tinyrefl"

int main()
{
    example::C c;

    boost::hana::for_each(c, boost::hana::fuse([](auto name, auto member) {
                              std::cout << boost::hana::to<const char*>(name)
                                        << ": " << member << std::endl;
                          }));
}
