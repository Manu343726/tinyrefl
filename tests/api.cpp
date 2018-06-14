#include "catch.hpp"
#include "example.hpp"
#include <tinyrefl/api.hpp>
#include "example.hpp.tinyrefl"
#include <unordered_set>
#include <unordered_map>

TEST_CASE("tinyrefl api")
{
    SECTION("class metadata")
    {
        using Metadata = $(my_namespace::MyClass);
        std::ostringstream ss;
        INFO((tinyrefl::meta::foreach<Metadata::base_classes>([&ss](auto type, auto index)
        {
            using Type = typename decltype(type)::type;
            ss << "Base class(" << index << "): " << ctti::nameof<Type>() << "\n";
        }), ss.str()));
        REQUIRE(Metadata::base_classes::size == 2);
        REQUIRE(std::is_same<tinyrefl::meta::get_t<0, Metadata::base_classes>, my_namespace::BaseClass>());
        REQUIRE(std::is_same<tinyrefl::meta::get_t<1, Metadata::base_classes>, my_namespace::Foo>());

        REQUIRE(tinyrefl::meta::get_t<0, Metadata::members>::value == &my_namespace::MyClass::f);
        REQUIRE(!tinyrefl::has_attribute<my_namespace::MyClass>("foo"));
    }

    SECTION("visit class")
    {
        auto test = [](const tinyrefl::entity expected_entity_kind, const std::unordered_map<std::string, int>& expected_results)
        {
            std::unordered_set<std::string> entities;

            tinyrefl::visit_class<my_namespace::MyClass>(
                [&entities, expected_entity_kind](const std::string& name, auto /* depth */, auto /* entity */, auto entity_kind)
            {
                if(entity_kind == expected_entity_kind)
                {
                    entities.insert(name);
                }
            });

            for(const auto& key_value : expected_results)
            {
                const auto& entity_name = key_value.first;
                const auto& expected_count = key_value.second;

                INFO(entity_name << " " << expected_entity_kind << " expected " << expected_count << " times");
                CHECK(entities.count(entity_name) == expected_count);
            }
        };

        SECTION("member variables")
        {
            test(tinyrefl::entity::MEMBER_VARIABLE, {
                {"vector",       1},
                {"str",          1},
                {"f",            0},
                {"baseMember",   0},
                {"baseFunction", 0},
                {"_private",     0},
                {"Enum",         0},
                {"Foo",          0},
                {"my_namespace::BaseClass", 0},
                {"my_namespace::Foo", 0},
                {"InnerClassWithMembers", 0},
                {"a", 0},
                {"b", 0},
                {"c", 0},
                {"innerClassInstance", 1}
            });
        }

        SECTION("member functions")
        {
            test(tinyrefl::entity::MEMBER_FUNCTION, {
                {"vector",       0},
                {"str",          0},
                {"f",            1},
                {"baseMember",   0},
                {"baseFunction", 0},
                {"_private",     0},
                {"Enum",         0},
                {"Foo",          0},
                {"my_namespace::BaseClass", 0},
                {"my_namespace::Foo", 0},
                {"InnerClassWithMembers", 0},
                {"a", 0},
                {"b", 0},
                {"c", 0},
                {"innerClassInstance", 0}
            });
        }

        SECTION("member enums")
        {
            test(tinyrefl::entity::MEMBER_ENUM, {
                {"vector",       0},
                {"str",          0},
                {"f",            0},
                {"baseMember",   0},
                {"baseFunction", 0},
                {"_private",     0},
                {"Enum",         1},
                {"BaseClass",    0},
                {"Foo",          0},
                {"my_namespace::BaseClass", 0},
                {"my_namespace::Foo", 0},
                {"InnerClassWithMembers", 0},
                {"a", 0},
                {"b", 0},
                {"c", 0},
                {"innerClassInstance", 0}
            });
        }

        SECTION("member classes")
        {
            test(tinyrefl::entity::MEMBER_CLASS, {
                {"vector",       0},
                {"str",          0},
                {"f",            0},
                {"baseMember",   0},
                {"baseFunction", 0},
                {"_private",     0},
                {"Enum",         0},
                {"BaseClass",    0},
                {"Foo",          1},
                {"my_namespace::BaseClass", 0},
                {"my_namespace::Foo", 0},
                {"InnerClassWithMembers", 1},
                {"a", 0},
                {"b", 0},
                {"c", 0},
                {"innerClassInstance", 0}
            });
        }

        SECTION("base classes")
        {
            test(tinyrefl::entity::BASE_CLASS, {
                {"vector",       0},
                {"str",          0},
                {"f",            0},
                {"baseMember",   0},
                {"baseFunction", 0},
                {"_private",     0},
                {"Enum",         0},
                {"Foo",          0},
                {"my_namespace::BaseClass", 1},
                {"my_namespace::Foo", 1},
                {"InnerClassWithMembers", 0},
                {"a", 0},
                {"b", 0},
                {"c", 0},
                {"innerClassInstance", 0}
            });
        }
    }

    SECTION("visit object")
    {
        auto test = [](auto expected_kind, const std::unordered_set<std::string>& expected)
        {
            std::unordered_set<std::string> members;
            my_namespace::MyClass myObject;

            tinyrefl::visit_object(myObject,
                [&members, expected_kind](const std::string& name, auto /* depth */, auto /* entity */, decltype(expected_kind) kind)
            {
                CHECK((kind == expected_kind.get()));
                members.insert(name);
            });

            INFO([members]
            {
                std::ostringstream ss;

                for(const auto& member : members)
                {
                    ss << " - got member \"" << member << "\"\n";
                }

                return ss.str();
            }());
            CHECK((members.size() == expected.size()));

            for(const auto& member : expected)
            {
                INFO("Expected " << expected_kind << " \"" << member << "\"");
                CHECK((members.count(member) == 1));
            }
        };

        SECTION("visit member variables only")
        {
            test(CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)(), {
                "str", "innerClassInstance", "vector"
            });
        }

        SECTION("visit subobjects only")
        {
            test(CTTI_STATIC_VALUE(tinyrefl::entity::OBJECT)(), {
                "my_namespace::BaseClass", "my_namespace::Foo"
            });
        }

        SECTION("visit returns references to the object members")
        {
            my_namespace::MyClass myObject;

            auto addressof = [](auto& object)
            {
                return reinterpret_cast<void*>(std::addressof(object));
            };

            tinyrefl::visit_object(myObject, [&myObject, addressof](const std::string& name, auto /* depth */, auto& member, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
            {
                if(name == "str")
                {
                    CHECK((addressof(member) == addressof(myObject.str)));
                }
                else if(name == "innerClassInstance")
                {
                    CHECK((addressof(member) == addressof(myObject.innerClassInstance)));
                }
                else if(name == "vector")
                {
                    CHECK((addressof(member) == addressof(myObject.vector)));
                }
            });
        }

        SECTION("assigning values to members in visit changes members of visited object")
        {
            my_namespace::MyClass myObject;

            tinyrefl::visit_object(myObject,
                [](const std::string& /* name */, auto /* depth */, std::string& member, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
            {
                member = "a new string value";
            },
                [](const std::string& /* name */, auto /* depth */, std::vector<int>& member, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
            {
                member.assign(42, 42);
            },
                [](const std::string& /* name */, auto /* depth */, my_namespace::MyClass::InnerClassWithMembers& member, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
            {
                member.a = 42;
                member.b = 42;
                member.c = 42;
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
        }
    }

    SECTION("visit objects")
    {
        auto test = [](auto expected_kind, const std::unordered_set<std::string>& expected)
        {
            std::unordered_set<std::string> members;
            my_namespace::MyClass lhs, rhs;

            tinyrefl::visit_objects(lhs, rhs)(
                [&members, expected_kind](const std::string& name, auto /* depth */, auto /* entity */, decltype(expected_kind) kind)
            {
                CHECK((kind == expected_kind.get()));
                members.insert(name);
            });

            INFO([members]
            {
                std::ostringstream ss;

                for(const auto& member : members)
                {
                    ss << " - got member \"" << member << "\"\n";
                }

                return ss.str();
            }());
            CHECK((members.size() == expected.size()));

            for(const auto& member : expected)
            {
                INFO("Expected " << expected_kind << " \"" << member << "\"");
                CHECK((members.count(member) == 1));
            }
        };

        SECTION("visit member variables only")
        {
            test(CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE)(), {
                "str", "innerClassInstance", "vector"
            });
        }

        SECTION("visit subobjects only")
        {
            test(CTTI_STATIC_VALUE(tinyrefl::entity::OBJECT)(), {
                "my_namespace::BaseClass", "my_namespace::Foo"
            });
        }

        SECTION("visit returns references to the object members")
        {
            my_namespace::MyClass lhs, rhs;

            auto addressof = [](auto& object)
            {
                return reinterpret_cast<void*>(std::addressof(object));
            };

            bool strVisited = false;
            bool innerClassInstanceVisited = false;
            bool vectorVisited = false;
            bool somethingVisited = false;

            tinyrefl::visit_objects(static_cast<my_namespace::MyClass&>(lhs), static_cast<my_namespace::MyClass&>(rhs))
                ([&lhs, &rhs, addressof, &strVisited, &innerClassInstanceVisited, &vectorVisited, &somethingVisited](const std::string& name, auto /* depth */, auto members, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
            {
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
                    CHECK(addressof(lhsMember) == addressof(lhs.innerClassInstance));
                    CHECK(addressof(rhsMember) == addressof(rhs.innerClassInstance));
                    innerClassInstanceVisited = true;
                }
                else if(name == "vector")
                {
                    CHECK(addressof(lhsMember) == addressof(lhs.vector));
                    CHECK(addressof(rhsMember) == addressof(rhs.vector));
                    vectorVisited = true;
                }

                somethingVisited = true;
            });

            CHECK(strVisited);
            CHECK(innerClassInstanceVisited);
            CHECK(vectorVisited);
            CHECK(somethingVisited);
        }

        SECTION("visit returns const references to the const object members")
        {
            my_namespace::MyClass lhs, rhs;

            auto addressof = [](const auto& object)
            {
                return reinterpret_cast<const void*>(std::addressof(object));
            };

            bool strVisited = false;
            bool innerClassInstanceVisited = false;
            bool vectorVisited = false;
            bool somethingVisited = false;

            tinyrefl::visit_objects(static_cast<const my_namespace::MyClass&>(lhs), static_cast<const my_namespace::MyClass&>(rhs))
                ([&lhs, &rhs, addressof, &strVisited, &innerClassInstanceVisited, &vectorVisited, &somethingVisited](const std::string& name, auto /* depth */, auto members, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
            {
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
                    CHECK(addressof(lhsMember) == addressof(lhs.innerClassInstance));
                    CHECK(addressof(rhsMember) == addressof(rhs.innerClassInstance));
                    innerClassInstanceVisited = true;
                }
                else if(name == "vector")
                {
                    CHECK(addressof(lhsMember) == addressof(lhs.vector));
                    CHECK(addressof(rhsMember) == addressof(rhs.vector));
                    vectorVisited = true;
                }

                somethingVisited = true;
            });

            CHECK(strVisited);
            CHECK(innerClassInstanceVisited);
            CHECK(vectorVisited);
            CHECK(somethingVisited);
        }

        SECTION("assigning values to members in visit changes members of visited object")
        {
            my_namespace::MyClass lhs, rhs;

            tinyrefl::visit_objects(lhs, rhs)(
                [](const std::string& /* name */, auto /* depth */, std::tuple<std::string&, std::string&> members, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
            {
                std::get<0>(members) = "a new string value";
                std::get<1>(members) = "a new string value";
            },
                [](const std::string& /* name */, auto /* depth */, std::tuple<std::vector<int>&, std::vector<int>&> members, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
            {
                std::get<0>(members).assign(42, 42);
                std::get<1>(members).assign(42, 42);
            },
                [](const std::string& /* name */, auto /* depth */, std::tuple<my_namespace::MyClass::InnerClassWithMembers&, my_namespace::MyClass::InnerClassWithMembers&> members, CTTI_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
            {
                std::get<0>(members).a = 42;
                std::get<0>(members).b = 42;
                std::get<0>(members).c = 42;
                std::get<1>(members).a = 42;
                std::get<1>(members).b = 42;
                std::get<1>(members).c = 42;
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
        }
    }
}
