#include "no_reflection.hpp"
#include <vector>
#include <string>

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

    typedef struct
    {
        int a, b;
    } TypedefStruct;

    InnerClassWithMembers innerClassInstance;

private:
    int _private;
};

}
