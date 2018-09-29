#include <string>
#include <tinyrefl/utils/enum_value_attributes.hpp>

namespace my
{
struct [[Name("Base", type = "class")]] Base
{
    std::string str;
};

/**
 * A class
 */
class[[Class]] Class : public Base
{
    [[property("i")]] int i;         //< A member variable
    [[ctor("int: i")]] Class(int i); //< A constructor

    [[property_getter("i")]] int  get_i() const; //< A getter member function
    [[property_setter("i")]] int& get_i();       //< A setter member function

    struct [[Tag]] Tag
    {
    };
    struct AnotherTag
    {
    };

    /**
     * An enum
     */
    enum class [[Enum]] Enum
    {
        A = 42,
        B, //< An enum value
            C TINYREFL_ENUM_VALUE_ATTRIBUTE(C)
    };
};
} // namespace my
