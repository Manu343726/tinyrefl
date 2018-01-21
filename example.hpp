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

struct [[tinyrefl::exclude(tinyrefl::entity::MEMBER_VARIABLE, "_.*")]]
    C : public A, public B
{
    TINYREFL_GODMODE
public:
    [[tinyrefl::ignore_no_really]] std::string ignore_me;
    std::string hey_im_here;
private:
    int _private;
};

}

#endif // TINYREFL_EXAMPLE_HPP
