#include <tinyrefl/tinyrefl.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    if(argc > 1)
    {
        return !tinyrefl::reflect_file(argv[1]);
    }
    else
    {
        std::cerr << "usage: tool <path to file>\n";
        return 1;
    }
}
