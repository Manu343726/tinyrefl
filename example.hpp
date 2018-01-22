#ifndef TINYREFL_EXAMPLE_HPP
#define TINYREFL_EXAMPLE_HPP

#include <string>
#include <tinyrefl/api.hpp>

namespace example
{

struct A
{
    int a = 42;
};


struct B
{
    const char* b = "hello";
};

struct C : public A, public B
{
    TINYREFL_GODMODE
public:
    [[tinyrefl::ignore]] std::string ignore_me;
    std::string hey_im_here;
    B subobject;
private:
    int _private = 42;
};

}

#endif // TINYREFL_EXAMPLE_HPP
