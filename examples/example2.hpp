#ifndef TINYREFL_EXAMPLE2_HPP
#define TINYREFL_EXAMPLE2_HPP

#include <string>
#include <tinyrefl/utils/enum_value_attributes.hpp>

namespace example
{

struct type_in_example_namespace_declared_in_example2_file
{
};

constexpr int function_declared_in_example2_header()
{
    return 42;
}

} // namespace example

namespace example2
{

struct A
{
    int a = 42;
};


struct B
{
    const char* b = "hello";
};

enum class Enum
{
    A,
    B,
    C,
    D,
    E = 42
};

struct C : public A, public B
{
public:
    [[tinyrefl::ignore]] std::string ignore_me;
    std::string                      hey_im_here = "hey, I'm here";
    B                                subobject;

    C() = default;
    explicit C(const std::string& str) {}
    C(int a, int b) {}

    void f(int a, int b) const {};
    void f(int a, int b){};
    void f(){};
    void f() const {};

    enum class Enum
    {
        A,
        B,
        C,
        D,
        E,
        F
    };

    Enum e = Enum::A;
};

std::ostream& operator<<(std::ostream& os, const B& b);
std::ostream& operator<<(std::ostream& os, const Enum& value);
std::ostream& operator<<(std::ostream& os, const C::Enum& value);
} // namespace example2

#endif // TINYREFL_EXAMPLE2_HPP
