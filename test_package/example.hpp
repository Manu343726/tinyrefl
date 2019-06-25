#ifndef TINYREFL_TESTPACKAGE_EXAMPLE_HPP
#define TINYREFL_TESTPACKAGE_EXAMPLE_HPP

#include <tinyrefl/api.hpp>

namespace example
{

class [[foobar]] Example
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

/*

inline void to_json(tinyrefl::json& json, const Example& ex)
{
    json["i"] = 42;
    json["e"] = "A";
}
*/
}; // namespace example

#endif // TINYREFL_TESTPACKAGE_EXAMPLE_HPP
