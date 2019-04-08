#ifndef TINYREFL_EXAMPLE_HPP
#define TINYREFL_EXAMPLE_HPP

#include <string>
#include <tinyrefl/utils/enum_value_attributes.hpp>

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
    explicit C(const std::string& str){}[[C]] C(int a, int b){}

        [[ f, f1, f2 ]] void f(int a, int b) const {};
    [[f]] void f(int a, int b) {};
    [[f]] void f() {};
    [[f]] void f() const {};

    enum class Enum
    {
        A TINYREFL_ENUM_VALUE_ATTRIBUTE(A),
        B TINYREFL_ENUM_VALUE_ATTRIBUTE(B),
        C TINYREFL_ENUM_VALUE_ATTRIBUTE(C),
        D TINYREFL_ENUM_VALUE_ATTRIBUTE(D),
        E TINYREFL_ENUM_VALUE_ATTRIBUTE(E),
        F TINYREFL_ENUM_VALUE_ATTRIBUTE(F)
    };

    [[e]] Enum e = Enum::A;

    static void static_member_function() {}
    static int  static_member_variable;
};

constexpr int variable = 42;

constexpr int function()
{
    return 42;
}

std::ostream& operator<<(std::ostream& os, const B& b);
std::ostream& operator<<(std::ostream& os, const Enum& value);
std::ostream& operator<<(std::ostream& os, const C::Enum& value);

namespace nested_namespace
{
class Foo
{
};
} // namespace nested_namespace

} // namespace example

namespace example_namespace = example;

#endif // TINYREFL_EXAMPLE_HPP
