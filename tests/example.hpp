#include "no_reflection.hpp"
#include <vector>
#include <string>
#include <ostream>

namespace my_namespace
{

struct Foo
{
    std::vector<int> vector;
};

class MyClass : public BaseClass, public my_namespace::Foo
{
public:
    void f(int i) {(void)i;}

    std::string str;

    enum class Enum
    {
        A, B, C, D = 42
    };

    struct Foo {};

    struct InnerClassWithMembers
    {
        int a, b, c;
    };

    InnerClassWithMembers innerClassInstance;
    Enum enum_value;
};

std::ostream& operator<<(std::ostream& os, const MyClass::Enum value);
bool operator==(const MyClass& lhs, const MyClass& rhs);

}
