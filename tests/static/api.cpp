#include "template_using_reflection.hpp"
#include "members.hpp"
#include "members.hpp.tinyrefl"
#include CTTI_STATIC_TESTS_HEADER

using check = tinyrefl::test::AssertMetadataAvailableForTemplateParam<foo::Foo>;
EXPECT_TRUE(std::is_pointer<check*>::value);
EXPECT_FALSE(tinyrefl::has_attribute<foo::Foo>("attribute"));

#ifdef __clang__
template<typename T>
constexpr auto overload_on_attributes() -> tinyrefl::meta::enable_if_t<
    tinyrefl::has_attribute<T>("return_42"), int
>
{
    return 42;
}

template<typename T>
constexpr auto overload_on_attributes() -> tinyrefl::meta::enable_if_t<
    !tinyrefl::has_attribute<T>("return_42"), int
>
{
    return 0;
}

EXPECT_EQ(overload_on_attributes<foo::Foo>(), 0);
#endif // __clang__
