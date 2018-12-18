#include "no_reflection.hpp"
#include <ostream>
#include <string>
#include <tinyrefl/utils/enum_value_attributes.hpp>
#include <unordered_map>
#include <vector>

namespace my_namespace
{

struct Foo
{
    std::vector<int, std::allocator<int>> vector;
};

class MyClass : public BaseClass, public my_namespace::Foo
{
public:
    [[ctor]] MyClass() = default;
    MyClass(int, int)
    {
    }
    MyClass(std::vector<std::string>){}

        [[f]] void f(int i)
    {
        (void)i;
    }
    void overloaded() const {};
    void overloaded(){};
    void overloaded(int) const {};
    void overloaded(int){};

    [[str]] std::string str;

    enum class[[Enum]] Enum{A TINYREFL_ENUM_VALUE_ATTRIBUTE(A), B, C, D = 42};

    struct[[Foo]] Foo{};

    struct[[InnerClassWithMembers(42, "foo")]] InnerClassWithMembers
    {
        int a, b, c;
    };

    InnerClassWithMembers innerClassInstance;
    Enum                  enum_value;
    std::unordered_map<std::string, std::string>
        functionReturningTemplateWithMultipleParameters();
};

std::ostream& operator<<(std::ostream& os, const MyClass::Enum value);
bool operator==(const MyClass& lhs, const MyClass& rhs);
}
