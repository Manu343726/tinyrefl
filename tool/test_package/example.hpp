#ifndef TINYREFL_TESTPACKAGE_EXAMPLE_HPP
#define TINYREFL_TESTPACKAGE_EXAMPLE_HPP

namespace example
{

class Example
{
public:
    Example() = default;

    int getInt() const
    {
        return 42;
    }

    int i = 42;

    enum class Enum
    {
        A,
        B,
        C
    };

    Enum e = Enum::A;
};

}; // namespace example

#endif // TINYREFL_TESTPACKAGE_EXAMPLE_HPP
