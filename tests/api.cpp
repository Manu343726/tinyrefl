#include "catch.hpp"
#include "example.hpp"
#include <tinyrefl/api.hpp>
#include "example.hpp.tinyrefl"
#include <unordered_set>
#include <unordered_map>
#include <iostream>

TEST_CASE("tinyrefl api")
{
    SECTION("class metadata")
    {
        using metadata = $(my_namespace::MyClass);
        std::ostringstream ss;
        INFO((tinyrefl::meta::foreach<metadata::base_classes>([&ss](auto type, auto index)
        {
            using Type = typename decltype(type)::type;
            ss << "Base class(" << index << "): " << ctti::nameof<Type>() << "\n";
        }), ss.str()));
        REQUIRE(metadata::base_classes::size == 2);
        REQUIRE(std::is_same<tinyrefl::meta::get_t<0, metadata::base_classes>, my_namespace::BaseClass>());
        REQUIRE(std::is_same<tinyrefl::meta::get_t<1, metadata::base_classes>, my_namespace::Foo>());

        REQUIRE(tinyrefl::meta::get_t<0, metadata::members>::value == &my_namespace::MyClass::f);
    }

    SECTION("visit class")
    {
        auto test = [](const tinyrefl::entity expected_entity_kind, const std::unordered_map<std::string, int>& expected_results)
        {
            std::unordered_set<std::string> entities;

            tinyrefl::visit_class<my_namespace::MyClass>(
                [&entities, expected_entity_kind](const std::string& name, auto depth, auto entity, auto entity_kind)
            {
            std::cout << "-- " << depth << " -- On " << entity_kind << " " << name << "\n";

                if(entity_kind == expected_entity_kind)
                {
                    entities.insert(name);
                }
            });

            std::cout << "\n\n";

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
            });
        }
    }
}
