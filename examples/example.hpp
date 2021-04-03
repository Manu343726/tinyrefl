#ifndef TINYREFL_EXAMPLE_HPP
#define TINYREFL_EXAMPLE_HPP

#include <string>
#include <tinyrefl/utils/enum_value_attributes.hpp>

namespace example_attributes
{
struct serializable
{
    constexpr serializable(const bool on = true) : on{on} {}

    bool on = true;
};
} // namespace example_attributes

namespace example
{

struct Test
{
    float                                            f = 0.0;
    [[example_attributes::serializable(false)]] int  i = 0;
    [[example_attributes::serializable]] std::string s = "";
};

struct range
{
    constexpr range(const int begin, const int end) : _begin{begin}, _end{end}
    {
    }

    constexpr bool in_range(const int value) const
    {
        return _begin <= value and value <= _end;
    }

    constexpr bool out_of_range(const int value) const
    {
        return not in_range(value);
    }

    constexpr int begin() const
    {
        return _begin;
    }

    constexpr int end() const
    {
        return _end;
    }

private:
    int _begin;
    int _end;
};

struct A
{
    [[example::range(0, 100)]] int a = 42;
    static A                       create();
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
