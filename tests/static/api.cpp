#include "template_using_reflection.hpp"
#include "members.hpp"
#include "members.hpp.tinyrefl"
#include "../example.hpp"
#include "../example.hpp.tinyrefl"
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

EXPECT_TRUE((std::is_same<decltype(tinyrefl::select_overload<>(&foo::Foo::f)), void(foo::Foo::*)()>::value));
EXPECT_TRUE((std::is_same<decltype(tinyrefl::select_overload<int>(&foo::Foo::f)), void(foo::Foo::*)(int)>::value));

EXPECT_TRUE(tinyrefl::meta::get_t<0, tinyrefl::metadata<my_namespace::MyClass>::constructors>().has_attribute("ctor"));
EXPECT_EQ((tinyrefl::meta::get_t<0, tinyrefl::metadata<my_namespace::MyClass>::constructors>().get_attribute("ctor").args.size()), 0);
EXPECT_EQ((tinyrefl::meta::get_t<0, tinyrefl::metadata<my_namespace::MyClass>::constructors>().get_attribute("ctor").full_attribute), "ctor");
EXPECT_EQ((tinyrefl::meta::get_t<0, tinyrefl::metadata<my_namespace::MyClass>::constructors>().get_attribute("ctor").name.name()), "ctor");
EXPECT_EQ((tinyrefl::meta::get_t<0, tinyrefl::metadata<my_namespace::MyClass>::constructors>().get_attribute("ctor").namespace_.name()), "");
EXPECT_TRUE(tinyrefl::has_attribute<tinyrefl::static_value<void(my_namespace::MyClass::*)(int), &my_namespace::MyClass::f>>("f"));
EXPECT_TRUE((tinyrefl::metadata<tinyrefl::static_value<void(my_namespace::MyClass::*)(int), &my_namespace::MyClass::f>>().get_attribute("f").args.size()), 1);
EXPECT_TRUE((tinyrefl::metadata<tinyrefl::static_value<void(my_namespace::MyClass::*)(int), &my_namespace::MyClass::f>>().get_attribute("f").args[0]), "");

EXPECT_TRUE(tinyrefl::has_attribute<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>("str"));
EXPECT_EQ((tinyrefl::metadata<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>().get_attribute("str").args.size()), 0);
EXPECT_EQ((tinyrefl::metadata<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>().get_attribute("str").full_attribute), "str");
EXPECT_EQ((tinyrefl::metadata<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>().get_attribute("str").namespace_.name()), "");
EXPECT_EQ((tinyrefl::metadata<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>().get_attribute("str").name.name()), "str");
EXPECT_TRUE(tinyrefl::has_attribute<my_namespace::MyClass::Foo>("Foo"));
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attribute("Foo").args.size()), 0);
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attribute("Foo").full_attribute), "Foo");
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attribute("Foo").namespace_.name()), "");
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attribute("Foo").name.name()), "Foo");
EXPECT_TRUE(tinyrefl::has_attribute<my_namespace::MyClass::Enum>("Enum"));
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::Enum>().get_attribute("Enum").args.size()), 0);
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::Enum>().get_attribute("Enum").full_attribute), "Enum");
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::Enum>().get_attribute("Enum").namespace_.name()), "");
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::Enum>().get_attribute("Enum").name.name()), "Enum");
EXPECT_TRUE(tinyrefl::has_attribute<TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>("A"));
EXPECT_EQ((tinyrefl::metadata<TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>().get_attribute("A").args.size()), 0);
EXPECT_EQ((tinyrefl::metadata<TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>().get_attribute("A").full_attribute), "A");
EXPECT_EQ((tinyrefl::metadata<TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>().get_attribute("A").namespace_.name()), "");
EXPECT_EQ((tinyrefl::metadata<TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>().get_attribute("A").name.name()), "A");
EXPECT_TRUE(tinyrefl::has_attribute<my_namespace::MyClass::InnerClassWithMembers>("InnerClassWithMembers"));
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>().get_attribute("InnerClassWithMembers").args.size()), 3);
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>().get_attribute("InnerClassWithMembers").args[0]), "42");
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>().get_attribute("InnerClassWithMembers").args[1]), ",");
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>().get_attribute("InnerClassWithMembers").args[2]), "\"foo\"");
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>().get_attribute("InnerClassWithMembers").full_attribute), "InnerClassWithMembers(42,\"foo\")");
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>().get_attribute("InnerClassWithMembers").namespace_.name()), "");
EXPECT_EQ((tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>().get_attribute("InnerClassWithMembers").name.name()), "InnerClassWithMembers");


EXPECT_EQ(tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attributes().size(), 1);
EXPECT_EQ(tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attribute("Foo").args.size(), 0);
EXPECT_EQ(tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attribute("Foo").full_attribute, "Foo");
EXPECT_EQ(tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attribute("Foo").name.name(), "Foo");
EXPECT_EQ(tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attribute("Foo").namespace_.name(), "");
