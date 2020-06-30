
#include "static_test.hpp"
#include <tinyrefl/backend/backend.hpp>

// String hashing from TINYREFL_STRING() macros works as expected
EXPECT_EQ(ctti::detail::cstring{TINYREFL_PP_STR(hello)}, "hello");
EXPECT_NE(
    ctti::detail::cstring{"hello"}.hash(),
    tinyrefl::default_string_constant.hash());

// Hash based string lookup works for known (defined) strings
TINYREFL_REGISTER_STRING(hello)
TINYREFL_REGISTER_STRING(world)
EXPECT_NE(
    tinyrefl::string_constant<TINYREFL_STRING(hello)>(),
    tinyrefl::default_string_constant);
EXPECT_EQ(tinyrefl::string_constant<TINYREFL_STRING(hello)>(), "hello");
EXPECT_NE(
    tinyrefl::string_constant<TINYREFL_STRING(world)>(),
    tinyrefl::default_string_constant);
EXPECT_EQ(tinyrefl::string_constant<TINYREFL_STRING(world)>(), "world");
EXPECT_EQ(
    tinyrefl::string_constant<TINYREFL_STRING(
        here is an string that it seems no one has registered before)>(),
    tinyrefl::default_string_constant);

#ifdef TINYREFL_OLD_API

namespace foo
{
namespace bar
{
template<ctti::detail::hash_t Hash>
constexpr ctti::detail::cstring get_string()
{
    return "default string";
}
} // namespace bar
} // namespace foo

namespace foo
{
namespace bar
{
template<>
constexpr ctti::detail::cstring
    get_string<ctti::detail::cstring{"foobar::foo"}.hash()>()
{
    return "foobar::foo";
}
} // namespace bar
} // namespace foo

EXPECT_EQ(
    foo::bar::get_string<ctti::detail::cstring{"foobar::foo"}.hash()>(),
    "foobar::foo");

TINYREFL_DEFINE_STRING(hello::world)
EXPECT_EQ(
    ctti::name_t{
        tinyrefl::backend::string_constant<TINYREFL_STRING(hello::world)>()}
        .name(),
    "world");

#endif // TINYREFL_OLD_API
