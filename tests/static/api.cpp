#include "template_using_reflection.hpp"
#include "members.hpp"
#include "members.hpp.tinyrefl"
#include <ctti/tests/static/static_test.hpp>

using check = tinyrefl::test::AssertMetadataAvailableForTemplateParam<foo::Foo>;
constexpr check triggerCheck;

EXPECT_FALSE(tinyrefl::has_attribute<foo::Foo>("attribute"));

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
