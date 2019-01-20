#include "catch.hpp"
#include "example.hpp"
#include <tinyrefl/api.hpp>
#include <unordered_map>
#include <unordered_set>
#include "example.hpp.tinyrefl"

using namespace std::string_literals;

TEST_CASE("tinyrefl api")
{
    SECTION("class metadata")
    {
        using Metadata = $(my_namespace::MyClass);
        std::ostringstream ss;
        INFO(
            (tinyrefl::meta::foreach<Metadata::base_classes>(
                 [&ss](auto type, auto index) {
                     using Type = typename decltype(type)::type;
                     ss << "Base class(" << index
                        << "): " << ctti::nameof<Type>() << "\n";
                 }),
             ss.str()));
        REQUIRE(Metadata::base_classes::size == 2);
        REQUIRE(std::is_same<
                tinyrefl::meta::get_t<0, Metadata::base_classes>,
                my_namespace::BaseClass>());
        REQUIRE(std::is_same<
                tinyrefl::meta::get_t<1, Metadata::base_classes>,
                my_namespace::Foo>());

        REQUIRE(
            tinyrefl::meta::get_t<0, Metadata::members>::value ==
            &my_namespace::MyClass::f);
        REQUIRE(!tinyrefl::has_attribute<my_namespace::MyClass>("foo"));

        SECTION(
            "overloaded functions are exposed as different member functions")
        {
            static_assert(
                Metadata::member_functions::size == 6,
                "Expected 6 member functions (1 'f', 4 'overloaded', and 1 'functionReturningTemplateWithMultipleParameters'");
            REQUIRE(
                tinyrefl::meta::get_t<1, Metadata::member_functions>::value ==
                static_cast<void (my_namespace::MyClass::*)() const>(
                    &my_namespace::MyClass::overloaded));
            REQUIRE(
                tinyrefl::meta::get_t<2, Metadata::member_functions>::value ==
                static_cast<void (my_namespace::MyClass::*)()>(
                    &my_namespace::MyClass::overloaded));
            REQUIRE(
                tinyrefl::meta::get_t<3, Metadata::member_functions>::value ==
                static_cast<void (my_namespace::MyClass::*)(int) const>(
                    &my_namespace::MyClass::overloaded));
            REQUIRE(
                tinyrefl::meta::get_t<4, Metadata::member_functions>::value ==
                static_cast<void (my_namespace::MyClass::*)(int)>(
                    &my_namespace::MyClass::overloaded));
        }

        SECTION("User declared constructors are reflected")
        {
            static_assert(
                Metadata::constructors::size == 3,
                "Expected three user declared constructors");

            REQUIRE(
                tinyrefl::meta::get_t<0, Metadata::constructors>::name ==
                "MyClass()");
            REQUIRE(
                tinyrefl::meta::get_t<1, Metadata::constructors>::name ==
                "MyClass(int,int)");
            REQUIRE(
                tinyrefl::meta::get_t<2, Metadata::constructors>::name ==
                "MyClass(std::vector<std::string>)");
        }
    }

    SECTION("attributes")
    {
        REQUIRE(tinyrefl::meta::get_t<
                    0,
                    tinyrefl::metadata<my_namespace::MyClass>::constructors>()
                    .has_attribute("ctor"));
        REQUIRE(tinyrefl::has_attribute<tinyrefl::static_value<
                    void (my_namespace::MyClass::*)(int),
                    &my_namespace::MyClass::f>>("f"));
        REQUIRE(tinyrefl::has_attribute<TINYREFL_STATIC_VALUE(
                    &my_namespace::MyClass::str)>("str"));
        REQUIRE(tinyrefl::has_attribute<my_namespace::MyClass::Foo>("Foo"));
        REQUIRE(tinyrefl::has_attribute<my_namespace::MyClass::Enum>("Enum"));
    }

    SECTION("visit class")
    {
        auto test =
            [](const tinyrefl::entity                      expected_entity_kind,
               const std::unordered_map<std::string, int>& expected_results) {
                std::unordered_set<std::string> entities;

                tinyrefl::visit_class<my_namespace::MyClass>(
                    [&entities, expected_entity_kind](
                        const std::string& name,
                        auto /* depth */,
                        auto /* entity */,
                        auto entity_kind) {
                        if(entity_kind == expected_entity_kind)
                        {
                            entities.insert(name);
                        }
                    });

                for(const auto& key_value : expected_results)
                {
                    const auto& entity_name    = key_value.first;
                    const auto& expected_count = key_value.second;

                    INFO(
                        entity_name << " " << expected_entity_kind
                                    << " expected " << expected_count
                                    << " times");
                    CHECK(entities.count(entity_name) == expected_count);
                }
            };

        SECTION("member variables")
        {
            test(
                tinyrefl::entity::MEMBER_VARIABLE,
                {{"vector", 1},
                 {"str", 1},
                 {"f", 0},
                 {"baseMember", 0},
                 {"baseFunction", 0},
                 {"_private", 0},
                 {"Enum", 0},
                 {"Foo", 0},
                 {"my_namespace::BaseClass", 0},
                 {"my_namespace::Foo", 0},
                 {"InnerClassWithMembers", 0},
                 {"a", 0},
                 {"b", 0},
                 {"c", 0},
                 {"enum_value", 1},
                 {"innerClassInstance", 1}});
        }

        SECTION("member functions")
        {
            test(
                tinyrefl::entity::MEMBER_FUNCTION,
                {{"vector", 0},
                 {"str", 0},
                 {"f", 1},
                 {"baseMember", 0},
                 {"baseFunction", 0},
                 {"_private", 0},
                 {"Enum", 0},
                 {"Foo", 0},
                 {"my_namespace::BaseClass", 0},
                 {"my_namespace::Foo", 0},
                 {"InnerClassWithMembers", 0},
                 {"a", 0},
                 {"b", 0},
                 {"c", 0},
                 {"enum_value", 0},
                 {"innerClassInstance", 0}});
        }

        SECTION("member enums")
        {
            test(
                tinyrefl::entity::MEMBER_ENUM,
                {{"vector", 0},
                 {"str", 0},
                 {"f", 0},
                 {"baseMember", 0},
                 {"baseFunction", 0},
                 {"_private", 0},
                 {"Enum", 1},
                 {"BaseClass", 0},
                 {"Foo", 0},
                 {"my_namespace::BaseClass", 0},
                 {"my_namespace::Foo", 0},
                 {"InnerClassWithMembers", 0},
                 {"a", 0},
                 {"b", 0},
                 {"c", 0},
                 {"enum_value", 0},
                 {"innerClassInstance", 0}});
        }

        SECTION("member classes")
        {
            test(
                tinyrefl::entity::MEMBER_CLASS,
                {{"vector", 0},
                 {"str", 0},
                 {"f", 0},
                 {"baseMember", 0},
                 {"baseFunction", 0},
                 {"_private", 0},
                 {"Enum", 0},
                 {"BaseClass", 0},
                 {"Foo", 1},
                 {"my_namespace::BaseClass", 0},
                 {"my_namespace::Foo", 0},
                 {"InnerClassWithMembers", 1},
                 {"a", 0},
                 {"b", 0},
                 {"c", 0},
                 {"enum_value", 0},
                 {"innerClassInstance", 0}});
        }

        SECTION("base classes")
        {
            test(
                tinyrefl::entity::BASE_CLASS,
                {{"vector", 0},
                 {"str", 0},
                 {"f", 0},
                 {"baseMember", 0},
                 {"baseFunction", 0},
                 {"_private", 0},
                 {"Enum", 0},
                 {"Foo", 0},
                 {"my_namespace::BaseClass", 1},
                 {"my_namespace::Foo", 1},
                 {"InnerClassWithMembers", 0},
                 {"a", 0},
                 {"b", 0},
                 {"c", 0},
                 {"enum_value", 0},
                 {"innerClassInstance", 0}});
        }
    }

    SECTION("visit object")
    {
        auto test = [](auto                                   expected_kind,
                       const std::unordered_set<std::string>& expected) {
            std::unordered_set<std::string> members;
            my_namespace::MyClass           myObject;

            tinyrefl::visit_object(
                myObject,
                [&members, expected_kind](
                    const std::string& name,
                    auto /* depth */,
                    auto /* entity */,
                    decltype(expected_kind) kind) {
                    CHECK(kind == expected_kind.get());
                    members.insert(name);
                });

            INFO([members] {
                std::ostringstream ss;

                for(const auto& member : members)
                {
                    ss << " - got member \"" << member << "\"\n";
                }

                return ss.str();
            }());
            CHECK(members.size() == expected.size());

            for(const auto& member : expected)
            {
                INFO("Expected " << expected_kind << " \"" << member << "\"");
                CHECK(members.count(member) == 1);
            }
        };

        SECTION("visit member variables only")
        {
            test(
                TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)(),
                {"str", "innerClassInstance", "vector", "enum_value"});
        }

        SECTION("visit subobjects only")
        {
            test(
                TINYREFL_STATIC_VALUE(tinyrefl::entity::OBJECT)(),
                {"my_namespace::BaseClass", "my_namespace::Foo"});
        }

        SECTION("visit returns references to the object members")
        {
            my_namespace::MyClass myObject;

            auto addressof = [](auto& object) {
                return reinterpret_cast<void*>(std::addressof(object));
            };

            bool strVisited                = false;
            bool innerClassInstanceVisited = false;
            bool vectorVisited             = false;
            bool enumValueVisited          = false;
            bool somethingVisited          = false;

            tinyrefl::visit_object(
                myObject,
                [&](const std::string& name,
                    auto /* depth */,
                    auto& member,
                    TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
                    if(name == "str")
                    {
                        CHECK(addressof(member) == addressof(myObject.str));
                        strVisited = true;
                    }
                    else if(name == "innerClassInstance")
                    {
                        CHECK(
                            addressof(member) ==
                            addressof(myObject.innerClassInstance));
                        innerClassInstanceVisited = true;
                    }
                    else if(name == "vector")
                    {
                        CHECK(addressof(member) == addressof(myObject.vector));
                        vectorVisited = true;
                    }
                    else if(name == "enum_value")
                    {
                        CHECK(
                            addressof(member) ==
                            addressof(myObject.enum_value));
                        enumValueVisited = true;
                    }

                    somethingVisited = true;
                });

            CHECK(strVisited);
            CHECK(innerClassInstanceVisited);
            CHECK(vectorVisited);
            CHECK(enumValueVisited);
            CHECK(somethingVisited);
        }

        SECTION(
            "assigning values to members in visit changes members of visited object")
        {
            my_namespace::MyClass myObject;

            tinyrefl::visit_object(
                myObject,
                [](const std::string& /* name */,
                   auto /* depth */,
                   std::string& member,
                   TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
                    member = "a new string value";
                },
                [](const std::string& /* name */,
                   auto /* depth */,
                   std::vector<int>& member,
                   TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
                    member.assign(42, 42);
                },
                [](const std::string& /* name */,
                   auto /* depth */,
                   my_namespace::MyClass::InnerClassWithMembers& member,
                   TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
                    member.a = 42;
                    member.b = 42;
                    member.c = 42;
                },
                [](const std::string& /* name */,
                   auto /* depth */,
                   my_namespace::MyClass::Enum& member,
                   TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
                    member = my_namespace::MyClass::Enum::C;
                });

            CHECK(myObject.str == "a new string value");
            REQUIRE(myObject.vector.size() == 42);

            for(int e : myObject.vector)
            {
                CHECK(e == 42);
            }

            CHECK(myObject.innerClassInstance.a == 42);
            CHECK(myObject.innerClassInstance.b == 42);
            CHECK(myObject.innerClassInstance.c == 42);
            CHECK(myObject.enum_value == my_namespace::MyClass::Enum::C);
        }
    }

    SECTION("visit objects")
    {
        auto test = [](auto                                   expected_kind,
                       const std::unordered_set<std::string>& expected) {
            std::unordered_set<std::string> members;
            my_namespace::MyClass           lhs, rhs;

            tinyrefl::visit_objects(lhs, rhs)(
                [&members, expected_kind](
                    const std::string& name,
                    auto /* depth */,
                    auto /* entity */,
                    decltype(expected_kind) kind) {
                    CHECK(kind == expected_kind.get());
                    members.insert(name);
                });

            INFO([members] {
                std::ostringstream ss;

                for(const auto& member : members)
                {
                    ss << " - got member \"" << member << "\"\n";
                }

                return ss.str();
            }());
            CHECK(members.size() == expected.size());

            for(const auto& member : expected)
            {
                INFO("Expected " << expected_kind << " \"" << member << "\"");
                CHECK(members.count(member) == 1);
            }
        };

        SECTION("visit member variables only")
        {
            test(
                TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)(),
                {"str", "innerClassInstance", "vector", "enum_value"});
        }

        SECTION("visit subobjects only")
        {
            test(
                TINYREFL_STATIC_VALUE(tinyrefl::entity::OBJECT)(),
                {"my_namespace::BaseClass", "my_namespace::Foo"});
        }

        SECTION("visit returns references to the object members")
        {
            my_namespace::MyClass lhs, rhs;

            auto addressof = [](auto& object) {
                return reinterpret_cast<void*>(std::addressof(object));
            };

            bool strVisited                = false;
            bool innerClassInstanceVisited = false;
            bool vectorVisited             = false;
            bool enumValueVisited          = false;
            bool somethingVisited          = false;

            tinyrefl::visit_objects(
                static_cast<my_namespace::MyClass&>(lhs),
                static_cast<my_namespace::MyClass&>(
                    rhs))([&lhs,
                           &rhs,
                           addressof,
                           &strVisited,
                           &innerClassInstanceVisited,
                           &vectorVisited,
                           &enumValueVisited,
                           &somethingVisited](
                              const std::string& name,
                              auto /* depth */,
                              auto members,
                              TINYREFL_STATIC_VALUE(
                                  tinyrefl::entity::MEMBER_VARIABLE)) {
                auto& lhsMember = std::get<0>(members);
                auto& rhsMember = std::get<1>(members);

                if(name == "str")
                {
                    CHECK(addressof(lhsMember) == addressof(lhs.str));
                    CHECK(addressof(rhsMember) == addressof(rhs.str));
                    strVisited = true;
                }
                else if(name == "innerClassInstance")
                {
                    CHECK(
                        addressof(lhsMember) ==
                        addressof(lhs.innerClassInstance));
                    CHECK(
                        addressof(rhsMember) ==
                        addressof(rhs.innerClassInstance));
                    innerClassInstanceVisited = true;
                }
                else if(name == "vector")
                {
                    CHECK(addressof(lhsMember) == addressof(lhs.vector));
                    CHECK(addressof(rhsMember) == addressof(rhs.vector));
                    vectorVisited = true;
                }
                else if(name == "enum_value")
                {
                    CHECK(addressof(lhsMember) == addressof(lhs.enum_value));
                    CHECK(addressof(rhsMember) == addressof(rhs.enum_value));
                    enumValueVisited = true;
                }

                somethingVisited = true;
            });

            CHECK(strVisited);
            CHECK(innerClassInstanceVisited);
            CHECK(vectorVisited);
            CHECK(enumValueVisited);
            CHECK(somethingVisited);
        }

        SECTION("visit returns const references to the const object members")
        {
            my_namespace::MyClass lhs, rhs;

            auto addressof = [](const auto& object) {
                return reinterpret_cast<const void*>(std::addressof(object));
            };

            bool strVisited                = false;
            bool innerClassInstanceVisited = false;
            bool vectorVisited             = false;
            bool enumValueVisited          = false;
            bool somethingVisited          = false;

            tinyrefl::visit_objects(
                static_cast<const my_namespace::MyClass&>(lhs),
                static_cast<const my_namespace::MyClass&>(
                    rhs))([&lhs,
                           &rhs,
                           addressof,
                           &strVisited,
                           &innerClassInstanceVisited,
                           &vectorVisited,
                           &enumValueVisited,
                           &somethingVisited](
                              const std::string& name,
                              auto /* depth */,
                              auto members,
                              TINYREFL_STATIC_VALUE(
                                  tinyrefl::entity::MEMBER_VARIABLE)) {
                const auto& lhsMember = std::get<0>(members);
                const auto& rhsMember = std::get<1>(members);

                if(name == "str")
                {
                    CHECK(addressof(lhsMember) == addressof(lhs.str));
                    CHECK(addressof(rhsMember) == addressof(rhs.str));
                    strVisited = true;
                }
                else if(name == "innerClassInstance")
                {
                    CHECK(
                        addressof(lhsMember) ==
                        addressof(lhs.innerClassInstance));
                    CHECK(
                        addressof(rhsMember) ==
                        addressof(rhs.innerClassInstance));
                    innerClassInstanceVisited = true;
                }
                else if(name == "vector")
                {
                    CHECK(addressof(lhsMember) == addressof(lhs.vector));
                    CHECK(addressof(rhsMember) == addressof(rhs.vector));
                    vectorVisited = true;
                }
                else if(name == "enum_value")
                {
                    CHECK(addressof(lhsMember) == addressof(lhs.enum_value));
                    CHECK(addressof(rhsMember) == addressof(rhs.enum_value));
                    enumValueVisited = true;
                }

                somethingVisited = true;
            });

            CHECK(strVisited);
            CHECK(innerClassInstanceVisited);
            CHECK(vectorVisited);
            CHECK(enumValueVisited);
            CHECK(somethingVisited);
        }

        SECTION(
            "assigning values to members in visit changes members of visited object")
        {
            my_namespace::MyClass lhs, rhs;

            tinyrefl::visit_objects(lhs, rhs)(
                [](const std::string& /* name */,
                   auto /* depth */,
                   std::tuple<std::string&, std::string&> members,
                   TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
                    std::get<0>(members) = "a new string value";
                    std::get<1>(members) = "a new string value";
                },
                [](const std::string& /* name */,
                   auto /* depth */,
                   std::tuple<std::vector<int>&, std::vector<int>&> members,
                   TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
                    std::get<0>(members).assign(42, 42);
                    std::get<1>(members).assign(42, 42);
                },
                [](const std::string& /* name */,
                   auto /* depth */,
                   std::tuple<
                       my_namespace::MyClass::InnerClassWithMembers&,
                       my_namespace::MyClass::InnerClassWithMembers&> members,
                   TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
                    std::get<0>(members).a = 42;
                    std::get<0>(members).b = 42;
                    std::get<0>(members).c = 42;
                    std::get<1>(members).a = 42;
                    std::get<1>(members).b = 42;
                    std::get<1>(members).c = 42;
                },
                [](const std::string& /* name */,
                   auto /* depth */,
                   std::tuple<
                       my_namespace::MyClass::Enum&,
                       my_namespace::MyClass::Enum&> members,
                   TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)) {
                    std::get<0>(members) = my_namespace::MyClass::Enum::C;
                    std::get<1>(members) = my_namespace::MyClass::Enum::D;
                });

            CHECK(lhs.str == "a new string value");
            CHECK(rhs.str == "a new string value");

            CHECK(lhs.innerClassInstance.a == 42);
            CHECK(lhs.innerClassInstance.b == 42);
            CHECK(lhs.innerClassInstance.c == 42);
            CHECK(rhs.innerClassInstance.a == 42);
            CHECK(rhs.innerClassInstance.b == 42);
            CHECK(rhs.innerClassInstance.c == 42);

            REQUIRE(lhs.vector.size() == 42);
            REQUIRE(rhs.vector.size() == 42);

            for(int e : lhs.vector)
            {
                CHECK(e == 42);
            }
            for(int e : rhs.vector)
            {
                CHECK(e == 42);
            }

            CHECK(lhs.enum_value == my_namespace::MyClass::Enum::C);
            CHECK(rhs.enum_value == my_namespace::MyClass::Enum::D);
        }
    }

    SECTION("visit members")
    {
        auto test = [](const std::unordered_set<std::string>& expected) {
            std::unordered_set<std::string> members;
            my_namespace::MyClass           myObject;

            tinyrefl::visit_member_variables(
                myObject,
                [&members](const std::string& name, auto /* entity */) {
                    members.insert(name);
                });

            INFO([members] {
                std::ostringstream ss;

                for(const auto& member : members)
                {
                    ss << " - got member \"" << member << "\"\n";
                }

                return ss.str();
            }());
            CHECK(members.size() == expected.size());

            for(const auto& member : expected)
            {
                INFO(
                    "Expected "
                    << " \"" << member << "\"");
                CHECK(members.count(member) == 1);
            }
        };

        SECTION("visit member variables only")
        {
            test({"str", "innerClassInstance", "vector", "enum_value"});
        }

        SECTION("visit returns references to the object members")
        {
            my_namespace::MyClass myObject;

            auto addressof = [](auto& object) {
                return reinterpret_cast<void*>(std::addressof(object));
            };

            tinyrefl::visit_member_variables(
                myObject,
                [&myObject, addressof](const std::string& name, auto& member) {
                    if(name == "str")
                    {
                        CHECK(addressof(member) == addressof(myObject.str));
                    }
                    else if(name == "innerClassInstance")
                    {
                        CHECK(
                            addressof(member) ==
                            addressof(myObject.innerClassInstance));
                    }
                    else if(name == "vector")
                    {
                        CHECK(addressof(member) == addressof(myObject.vector));
                    }
                    else if(name == "enum_value")
                    {
                        CHECK(
                            addressof(member) ==
                            addressof(myObject.enum_value));
                    }
                });
        }

        SECTION(
            "assigning values to members in visit changes members of visited object")
        {
            my_namespace::MyClass myObject;

            tinyrefl::visit_member_variables(
                myObject,
                [](const std::string& /* name */, std::string& member) {
                    member = "a new string value";
                },
                [](const std::string& /* name */, std::vector<int>& member) {
                    member.assign(42, 42);
                },
                [](const std::string& /* name */,
                   my_namespace::MyClass::InnerClassWithMembers& member) {
                    member.a = 42;
                    member.b = 42;
                    member.c = 42;
                },
                [](const std::string& /* name */,
                   my_namespace::MyClass::Enum& value) {
                    value = my_namespace::MyClass::Enum::D;
                });

            CHECK(myObject.str == "a new string value");
            REQUIRE(myObject.vector.size() == 42);

            for(int e : myObject.vector)
            {
                CHECK(e == 42);
            }

            CHECK(myObject.innerClassInstance.a == 42);
            CHECK(myObject.innerClassInstance.b == 42);
            CHECK(myObject.innerClassInstance.c == 42);

            CHECK(myObject.enum_value == my_namespace::MyClass::Enum::D);
        }
    }

    SECTION("tuple converters")
    {
        auto addressof = [](const auto& object) {
            return reinterpret_cast<const void*>(std::addressof(object));
        };

        SECTION("make_tuple")
        {
            my_namespace::MyClass myObject;
            auto                  tuple = tinyrefl::make_tuple(myObject);

            static_assert(
                std::tuple_size<decltype(tuple)>::value == 3,
                "Wrong tuple serialization");
            CHECK(addressof(std::get<0>(tuple)) == addressof(myObject.str));
            CHECK(
                addressof(std::get<1>(tuple)) ==
                addressof(myObject.innerClassInstance));
            CHECK(
                addressof(std::get<2>(tuple)) ==
                addressof(myObject.enum_value));
        }

        SECTION("make_object")
        {
            const auto tuple = std::make_tuple(
                "foo str",
                my_namespace::MyClass::InnerClassWithMembers{1, 2, 3},
                my_namespace::MyClass::Enum::A);
            my_namespace::MyClass object =
                tinyrefl::make_object<my_namespace::MyClass>(tuple);

            CHECK(object.str == "foo str");
            CHECK(object.innerClassInstance.a == 1);
            CHECK(object.innerClassInstance.b == 2);
            CHECK(object.innerClassInstance.c == 3);
            CHECK(object.enum_value == my_namespace::MyClass::Enum::A);
        }
    }

    SECTION("enum converters")
    {
        using Enum = my_namespace::MyClass::Enum;

        SECTION("enum_cast(integer)")
        {
            CHECK(tinyrefl::enum_cast<Enum>(0) == Enum::A);
            CHECK(tinyrefl::enum_cast<Enum>(1) == Enum::B);
            CHECK(tinyrefl::enum_cast<Enum>(2) == Enum::C);
            CHECK(tinyrefl::enum_cast<Enum>(42) == Enum::D);
        }

        SECTION("enum_cast(string)")
        {
            CHECK(tinyrefl::enum_cast<Enum>("A") == Enum::A);
            CHECK(tinyrefl::enum_cast<Enum>("B") == Enum::B);
            CHECK(tinyrefl::enum_cast<Enum>("C") == Enum::C);
            CHECK(tinyrefl::enum_cast<Enum>("D") == Enum::D);
        }

        SECTION("enum_cast(std::string)")
        {
            CHECK(tinyrefl::enum_cast<Enum>("A"s) == Enum::A);
            CHECK(tinyrefl::enum_cast<Enum>("B"s) == Enum::B);
            CHECK(tinyrefl::enum_cast<Enum>("C"s) == Enum::C);
            CHECK(tinyrefl::enum_cast<Enum>("D"s) == Enum::D);
        }

        SECTION("to_string()")
        {
            CHECK(tinyrefl::to_string(Enum::A) == "A");
            CHECK(tinyrefl::to_string(Enum::B) == "B");
            CHECK(tinyrefl::to_string(Enum::C) == "C");
            CHECK(tinyrefl::to_string(Enum::D) == "D");
        }
    }

    SECTION("JSON serialization")
    {
        my_namespace::MyClass myObject;
        myObject.str                  = "foo";
        myObject.innerClassInstance.a = 1;
        myObject.innerClassInstance.b = 2;
        myObject.innerClassInstance.c = 3;
        myObject.enum_value           = my_namespace::MyClass::Enum::A;
        myObject.vector               = {1, 2, 3, 4};

        SECTION("to_json")
        {
            auto json = tinyrefl::to_json(myObject);
            INFO(json);

            REQUIRE(json.is_object());

            REQUIRE(json.count("str") == 1);
            REQUIRE(json["str"].is_string());
            CHECK(json["str"] == "foo");

            REQUIRE(json.count("innerClassInstance") == 1);
            REQUIRE(json["innerClassInstance"].is_object());

            REQUIRE(json["innerClassInstance"].count("a") == 1);
            REQUIRE(json["innerClassInstance"]["a"].is_number_integer());
            CHECK(json["innerClassInstance"]["a"] == 1);

            REQUIRE(json["innerClassInstance"].count("b") == 1);
            REQUIRE(json["innerClassInstance"]["b"].is_number_integer());
            CHECK(json["innerClassInstance"]["b"] == 2);

            REQUIRE(json["innerClassInstance"].count("c") == 1);
            REQUIRE(json["innerClassInstance"]["c"].is_number_integer());
            CHECK(json["innerClassInstance"]["c"] == 3);

            REQUIRE(json.count("vector") == 1);
            REQUIRE(json["vector"].is_array());

            REQUIRE(json.count("enum_value") == 1);
            REQUIRE(json["enum_value"].is_string());
            CHECK(json["enum_value"] == "A");
        }

        SECTION("from_json")
        {
            auto object = tinyrefl::from_json<my_namespace::MyClass>(
                {{"str", "foo"},
                 {"innerClassInstance", {{"a", 1}, {"b", 2}, {"c", 3}}},
                 {"vector", {1, 2, 3, 4}},
                 {"enum_value", "A"}});

            CHECK(object.str == "foo");
            CHECK(object.innerClassInstance.a == 1);
            CHECK(object.innerClassInstance.b == 2);
            CHECK(object.innerClassInstance.c == 3);
            CHECK(object.vector == std::vector<int>{1, 2, 3, 4});
            CHECK(object.enum_value == my_namespace::MyClass::Enum::A);
        }
    }

    SECTION("comparison operators")
    {
        my_namespace::MyClass a, b, c, d;

        a.str                  = "foo";
        a.innerClassInstance.a = 1;
        a.innerClassInstance.b = 2;
        a.innerClassInstance.c = 3;
        a.enum_value           = my_namespace::MyClass::Enum::A;
        a.vector               = {1, 2, 3, 4};

        d = c = b = a;

        d.enum_value = my_namespace::MyClass::Enum::B;

        CHECK(tinyrefl::equal(a, b));
        CHECK(tinyrefl::equal(b, a));
        CHECK(tinyrefl::equal(a, c));
        CHECK(tinyrefl::equal(c, a));
        CHECK(tinyrefl::not_equal(a, d));
    }
}
