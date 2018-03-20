#include <ctti/tests/static/static_test.hpp>
#include <tinyrefl/backend.hpp>

// String hashing from TINYREFL_STRING() macros works as expected
EXPECT_EQ(ctti::detail::cstring{TINYREFL_PP_STR(hello)}, "hello");
EXPECT_NE(ctti::detail::cstring{"hello"}.hash(), tinyrefl::backend::default_string_constant.hash());

// Hash based string lookup works for known (defined) strings
TINYREFL_DEFINE_STRING(hello)
TINYREFL_DEFINE_STRING(world)
EXPECT_NE(tinyrefl::backend::string_constant<TINYREFL_STRING(hello)>(), tinyrefl::backend::default_string_constant);
EXPECT_EQ(tinyrefl::backend::string_constant<TINYREFL_STRING(hello)>(), "hello");
EXPECT_NE(tinyrefl::backend::string_constant<TINYREFL_STRING(world)>(), tinyrefl::backend::default_string_constant);
EXPECT_EQ(tinyrefl::backend::string_constant<TINYREFL_STRING(world)>(), "world");
EXPECT_EQ(tinyrefl::backend::string_constant<TINYREFL_STRING(here is an string that it seems no one has registered before)>(), tinyrefl::backend::default_string_constant);

namespace foo { namespace bar {
template<ctti::detail::hash_t Hash>
constexpr ctti::detail::cstring get_string()
{
    return "default string";
}

} }

namespace foo { namespace bar {
template<>
constexpr ctti::detail::cstring get_string<ctti::detail::cstring{"foobar::foo"}.hash()>()
{
    return "foobar::foo";
}

} }

EXPECT_EQ(foo::bar::get_string<ctti::detail::cstring{"foobar::foo"}.hash()>(), "foobar::foo");

TINYREFL_DEFINE_STRING(hello::world)
EXPECT_EQ(ctti::name_t{tinyrefl::backend::string_constant<TINYREFL_STRING(hello::world)>()}.name(), "world");

struct Foo
{
    constexpr Foo() : a{0}, b{0}, c{0}, d{0}{};
    int a, b, c, d;
};

TINYREFL_DEFINE_STRING(a)
TINYREFL_DEFINE_STRING(b)
TINYREFL_DEFINE_STRING(c)
TINYREFL_DEFINE_STRING(d)
TINYREFL_DEFINE_STRING(Foo::a)
TINYREFL_DEFINE_STRING(Foo::b)
TINYREFL_DEFINE_STRING(Foo::c)
TINYREFL_DEFINE_STRING(Foo::d)
using a_member = TINYREFL_MEMBER(TINYREFL_STRING(a), TINYREFL_STRING(Foo::a), TINYREFL_TYPE(Foo, Foo), TINYREFL_VALUE(&Foo::a));
using b_member = TINYREFL_MEMBER(TINYREFL_STRING(b), TINYREFL_STRING(Foo::b), TINYREFL_TYPE(Foo, Foo), TINYREFL_VALUE(&Foo::b));

// Check member name strings are passed correctly to the member metadata template
EXPECT_NE(a_member::name.full_name(), tinyrefl::backend::default_string_constant);
EXPECT_EQ(a_member::name.full_name(), "Foo::a");
EXPECT_NE(b_member::name.full_name(), tinyrefl::backend::default_string_constant);
EXPECT_EQ(b_member::name.full_name(), "Foo::b");

// Different members hash to different ids
EXPECT_NE(a_member::name.full_name().hash(), b_member::name.full_name().hash());

// Check member pointers are passed correctly to the member metadata template
EXPECT_EQ(a_member{}.get(), &Foo::a);
constexpr Foo foobar;
EXPECT_EQ(&(a_member{}.get(foobar)), &foobar.a);

// The should be no double definition errors here, each member must specialize its own reflect (backend::metadata_of<>) template
TINYREFL_REFLECT_MEMBER(a_member)
TINYREFL_REFLECT_MEMBER(b_member)

// Same as above, but using full macro syntax (Exact syntax used by the metadata codegen tool)
TINYREFL_REFLECT_MEMBER(TINYREFL_MEMBER(TINYREFL_STRING(c), TINYREFL_STRING(Foo::c), TINYREFL_TYPE(Foo, Foo), TINYREFL_VALUE(&Foo::c)))
TINYREFL_REFLECT_MEMBER(TINYREFL_MEMBER(TINYREFL_STRING(d), TINYREFL_STRING(Foo::d), TINYREFL_TYPE(Foo, Foo), TINYREFL_VALUE(&Foo::d)))
