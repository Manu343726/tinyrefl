#include "../example.hpp"
#include "members.hpp"
#include "template_using_reflection.hpp"
#include <tinyrefl/utils/enum_value_attributes.hpp>
#include "../example.hpp.tinyrefl"
#include "members.hpp.tinyrefl"
#include <tinyrefl/entities.hpp>
#include "static_test.hpp"

#if TINYREFL_GENERATED_FILE_COUNT != 2
#error "Expected two tinyrefl codegen headers"
#endif

using check = tinyrefl::test::AssertMetadataAvailableForTemplateParam<foo::Foo>;
EXPECT_TRUE(std::is_pointer<check*>::value);
EXPECT_FALSE(tinyrefl::entities::has_attribute(tinyrefl::metadata<foo::Foo>(), "attribute"));

#ifdef __clang__
template<typename T>
constexpr auto overload_on_attributes()
    -> tinyrefl::meta::enable_if_t<tinyrefl::entities::has_attribute(tinyrefl::metadata<T>(), "return_42"), int>
{
    return 42;
}

template<typename T>
constexpr auto overload_on_attributes() -> tinyrefl::meta::
    enable_if_t<!tinyrefl::entities::has_attribute(tinyrefl::metadata<T>(), "return_42"), int>
{
    return 0;
}

EXPECT_EQ(overload_on_attributes<foo::Foo>(), 0);
#endif // __clang__

#ifdef TINYREFL_OLD_API
EXPECT_TRUE((std::is_same<
             decltype(tinyrefl::select_overload<>(&foo::Foo::f)),
             void (foo::Foo::*)()>::value));
EXPECT_TRUE((std::is_same<
             decltype(tinyrefl::select_overload<int>(&foo::Foo::f)),
             void (foo::Foo::*)(int)>::value));

EXPECT_TRUE(tinyrefl::meta::get_t<
                0,
                tinyrefl::metadata<my_namespace::MyClass>::constructors>()
                .has_attribute("ctor"));
EXPECT_EQ(
    (tinyrefl::meta::
         get_t<0, tinyrefl::metadata<my_namespace::MyClass>::constructors>()
             .get_attribute("ctor")
             .args.size()),
    0);
EXPECT_EQ(
    (tinyrefl::meta::
         get_t<0, tinyrefl::metadata<my_namespace::MyClass>::constructors>()
             .get_attribute("ctor")
             .full_attribute),
    "ctor");
EXPECT_EQ(
    (tinyrefl::meta::
         get_t<0, tinyrefl::metadata<my_namespace::MyClass>::constructors>()
             .get_attribute("ctor")
             .name.name()),
    "ctor");
EXPECT_EQ(
    (tinyrefl::meta::
         get_t<0, tinyrefl::metadata<my_namespace::MyClass>::constructors>()
             .get_attribute("ctor")
             .namespace_.name()),
    "");
EXPECT_TRUE(tinyrefl::entities::has_attribute<tinyrefl::static_value<
                void (my_namespace::MyClass::*)(int),
                &my_namespace::MyClass::f>>("f"));
EXPECT_TRUE(
    (tinyrefl::metadata<tinyrefl::static_value<
         void (my_namespace::MyClass::*)(int),
         &my_namespace::MyClass::f>>()
         .get_attribute("f")
         .args.size()),
    1);
EXPECT_TRUE(
    (tinyrefl::metadata<tinyrefl::static_value<
         void (my_namespace::MyClass::*)(int),
         &my_namespace::MyClass::f>>()
         .get_attribute("f")
         .args[0]),
    "");

EXPECT_TRUE(
    tinyrefl::entities::has_attribute<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>(
        "str"));
EXPECT_EQ(
    (tinyrefl::metadata<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>()
         .get_attribute("str")
         .args.size()),
    0);
EXPECT_EQ(
    (tinyrefl::metadata<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>()
         .get_attribute("str")
         .full_attribute),
    "str");
EXPECT_EQ(
    (tinyrefl::metadata<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>()
         .get_attribute("str")
         .namespace_.name()),
    "");
EXPECT_EQ(
    (tinyrefl::metadata<TINYREFL_STATIC_VALUE(&my_namespace::MyClass::str)>()
         .get_attribute("str")
         .name.name()),
    "str");
EXPECT_TRUE(tinyrefl::entities::has_attribute<my_namespace::MyClass::Foo>("Foo"));
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::Foo>()
         .get_attribute("Foo")
         .args.size()),
    0);
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::Foo>()
         .get_attribute("Foo")
         .full_attribute),
    "Foo");
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::Foo>()
         .get_attribute("Foo")
         .namespace_.name()),
    "");
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::Foo>()
         .get_attribute("Foo")
         .name.name()),
    "Foo");
EXPECT_TRUE(tinyrefl::entities::has_attribute<my_namespace::MyClass::Enum>("Enum"));
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::Enum>()
         .get_attribute("Enum")
         .args.size()),
    0);
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::Enum>()
         .get_attribute("Enum")
         .full_attribute),
    "Enum");
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::Enum>()
         .get_attribute("Enum")
         .namespace_.name()),
    "");
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::Enum>()
         .get_attribute("Enum")
         .name.name()),
    "Enum");

#if TINYREFL_HAS_ENUM_VALUE_ATTRIBUTES
EXPECT_TRUE(tinyrefl::entities::has_attribute<
            TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>("A"));
EXPECT_EQ(
    (tinyrefl::metadata<TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>()
         .get_attribute("A")
         .args.size()),
    0);
EXPECT_EQ(
    (tinyrefl::metadata<TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>()
         .get_attribute("A")
         .full_attribute),
    "A");
EXPECT_EQ(
    (tinyrefl::metadata<TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>()
         .get_attribute("A")
         .namespace_.name()),
    "");
EXPECT_EQ(
    (tinyrefl::metadata<TINYREFL_STATIC_VALUE(my_namespace::MyClass::Enum::A)>()
         .get_attribute("A")
         .name.name()),
    "A");
#endif // TINYREFL_HAS_ENUM_VALUE_ATTRIBUTES

EXPECT_TRUE(
    tinyrefl::entities::has_attribute<my_namespace::MyClass::InnerClassWithMembers>(
        "InnerClassWithMembers"));
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>()
         .get_attribute("InnerClassWithMembers")
         .args.size()),
    3);
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>()
         .get_attribute("InnerClassWithMembers")
         .args[0]),
    "42");

#ifdef TINYREFL_HAS_CONSTEXPR_ARRAY_VIEW_SUBSCRIPT
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>()
         .get_attribute("InnerClassWithMembers")
         .args[1]),
    ",");
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>()
         .get_attribute("InnerClassWithMembers")
         .args[2]),
    "\"foo\"");
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>()
         .get_attribute("InnerClassWithMembers")
         .full_attribute),
    "InnerClassWithMembers(42,\"foo\")");
#endif // TINYREFL_HAS_CONSTEXPR_ARRAY_VIEW_SUBSCRIPT
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>()
         .get_attribute("InnerClassWithMembers")
         .namespace_.name()),
    "");
EXPECT_EQ(
    (tinyrefl::metadata<my_namespace::MyClass::InnerClassWithMembers>()
         .get_attribute("InnerClassWithMembers")
         .name.name()),
    "InnerClassWithMembers");


EXPECT_EQ(
    tinyrefl::metadata<my_namespace::MyClass::Foo>().get_attributes().size(),
    1);
EXPECT_EQ(
    tinyrefl::metadata<my_namespace::MyClass::Foo>()
        .get_attribute("Foo")
        .args.size(),
    0);
EXPECT_EQ(
    tinyrefl::metadata<my_namespace::MyClass::Foo>()
        .get_attribute("Foo")
        .full_attribute,
    "Foo");
EXPECT_EQ(
    tinyrefl::metadata<my_namespace::MyClass::Foo>()
        .get_attribute("Foo")
        .name.name(),
    "Foo");
EXPECT_EQ(
    tinyrefl::metadata<my_namespace::MyClass::Foo>()
        .get_attribute("Foo")
        .namespace_.name(),
    "");

using namespace tinyrefl::literals;

EXPECT_TRUE(tinyrefl::has_entity_metadata<"my_namespace::MyClass::Foo"_id>());

EXPECT_TRUE((std::is_same<
             tinyrefl::entity_metadata<"my_namespace::MyClass::Foo"_id>,
             tinyrefl::metadata<my_namespace::MyClass::Foo>>::value));

EXPECT_TRUE(
    tinyrefl::has_entity_metadata<"my_namespace::MyClass::overloaded()"_id>());

EXPECT_TRUE((std::is_same<
             tinyrefl::entity_metadata<"my_namespace::MyClass::Foo"_id>,
             tinyrefl::metadata<my_namespace::MyClass::Foo>>::value));

EXPECT_TRUE(tinyrefl::has_entity_metadata<
            "my_namespace::MyClass::overloaded() const"_id>());
EXPECT_TRUE(tinyrefl::has_entity_metadata<
            "my_namespace::MyClass::overloaded(int)"_id>());
EXPECT_TRUE(tinyrefl::has_entity_metadata<
            "my_namespace::MyClass::overloaded(int) const"_id>());

EXPECT_TRUE(
    (std::is_same<
        tinyrefl::entity_metadata<"my_namespace::MyClass::overloaded()"_id>,
        tinyrefl::metadata<::ctti::static_value<
            void (my_namespace::MyClass::*)(),
            &my_namespace::MyClass::overloaded>>>::value));

EXPECT_TRUE((std::is_same<
             tinyrefl::entity_metadata<
                 "my_namespace::MyClass::overloaded() const"_id>,
             tinyrefl::metadata<::ctti::static_value<
                 void (my_namespace::MyClass::*)() const,
                 &my_namespace::MyClass::overloaded>>>::value));

EXPECT_TRUE(
    (std::is_same<
        tinyrefl::entity_metadata<"my_namespace::MyClass::overloaded(int)"_id>,
        tinyrefl::metadata<::ctti::static_value<
            void (my_namespace::MyClass::*)(int),
            &my_namespace::MyClass::overloaded>>>::value));

EXPECT_TRUE((std::is_same<
             tinyrefl::entity_metadata<
                 "my_namespace::MyClass::overloaded(int) const"_id>,
             tinyrefl::metadata<::ctti::static_value<
                 void (my_namespace::MyClass::*)(int) const,
                 &my_namespace::MyClass::overloaded>>>::value));

EXPECT_EQ(
    tinyrefl::entity_metadata<
        "my_namespace::MyClass::overloaded(int)"_id>::arg_names.size(),
    2); // 1 arg plus extra dummy array item

EXPECT_EQ(
    tinyrefl::entity_metadata<
        "my_namespace::MyClass::overloaded(int)"_id>::arg_names[0],
    "firstArg");

EXPECT_TRUE(tinyrefl::has_entity_metadata<"my_namespace::MyClass::Enum"_id>());
EXPECT_TRUE(std::is_same<
            tinyrefl::entity_metadata<"my_namespace::MyClass::Enum"_id>,
            tinyrefl::metadata<my_namespace::MyClass::Enum>>::value);

EXPECT_TRUE(
    std::is_same<
        tinyrefl::meta::
            filter_t<tinyrefl::meta::defer<std::is_class>, tinyrefl::entities>,
        tinyrefl::entities>::value);
#endif // TINYREFL_OLD_API
