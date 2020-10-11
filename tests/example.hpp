#include "no_reflection.hpp"
#include <ostream>
#include <string>
#include <tinyrefl/types/string.hpp>
#include <tinyrefl/utils/enum_value_attributes.hpp>
#include <unordered_map>
#include <vector>

namespace attribute_examples
{

struct ctor
{
};

struct f
{
};

struct str
{
};

struct Enum
{
};

struct InnerClass
{
};

struct InnerClassWithMembers
{
    constexpr InnerClassWithMembers(int param1, const tinyrefl::string param2)
    {
    }
};

struct A
{
};

} // namespace attribute_examples

namespace my_namespace
{

struct BaseClassWithReflectionData
{
    std::vector<int, std::allocator<int>> vector;
};

class MyClass : public BaseClassWithoutReflectionData,
                public my_namespace::BaseClassWithReflectionData
{
public:
    [[attribute_examples::ctor]] MyClass() = default;
    MyClass(int, int) {}
    MyClass(std::vector<std::string>) {}

    [[attribute_examples::f]] void f(int i)
    {
        (void)i;
    }
    void overloaded() const {};
    void overloaded(){};
    void overloaded(int firstArg) const {};
    void overloaded(int firstArg){};

    [[attribute_examples::str]] std::string str;

    enum class [[attribute_examples::Enum]] Enum{
        A TINYREFL_ENUM_VALUE_ATTRIBUTE(attribute_examples::A), B, C, D = 42};

    struct [[attribute_examples::InnerClass]] InnerClass{};

    struct [[attribute_examples::InnerClassWithMembers(
        42, "foo")]] InnerClassWithMembers
    {
        int a, b, c;
    };

    InnerClassWithMembers innerClassInstance;
    Enum                  enum_value;
    std::unordered_map<std::string, std::string>
        functionReturningTemplateWithMultipleParameters();
};

std::ostream& operator<<(std::ostream& os, const MyClass::Enum value);
bool          operator==(const MyClass& lhs, const MyClass& rhs);
} // namespace my_namespace
