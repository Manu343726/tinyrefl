#include "example.hpp"
#include <iostream>
#include <tinyrefl/api.hpp>
#include "example.hpp.tinyrefl"

int main()
{
    std::cout << tinyrefl::to_string(example::Example{}) << "\n";
}
