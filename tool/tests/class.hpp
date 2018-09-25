#include <string>

namespace my
{
struct Base
{
    std::string str;
};
class Class : public Base
{
    int i;
    Class(int i);

    int  get_i() const;
    int& get_i();

    struct Tag
    {
    };
    struct AnotherTag
    {
    };

    enum class Enum
    {
        A = 42,
        B,
        C
    };
};
} // namespace my
