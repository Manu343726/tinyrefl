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
    void f(int i) {}

    std::string str;

    enum class Enum
    {
        A, B, C, D = 42
    };

    struct Foo {};

private:
    int _private;
};

}
