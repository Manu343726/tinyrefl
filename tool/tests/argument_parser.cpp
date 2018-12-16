#include <catch.hpp>
#include <tinyrefl/tool/argument_parser.hpp>

using namespace tinyrefl::tool;

template<std::size_t N>
config parse(const char*(&&args)[N])
{
    return parse_arguments(N, args);
}

TEST_CASE("tool::argument_parser")
{
    SECTION("input file missing returns error")
    {
        CHECK_THROWS_AS(parse({""}), std::invalid_argument);
    }

    SECTION("C++ standard missing returns error")
    {
        CHECK_THROWS_AS(parse({"foo.hpp"}), std::invalid_argument);
    }

    SECTION("Unknown C++ standard returns error")
    {
        CHECK_THROWS_AS(
            parse({"foo.hpp", "-std=c++12"}), std::invalid_argument);
    }

    SECTION("Default output file is same file with .tinyrefl extension")
    {
        CHECK(
            parse({"foo.hpp", "-std=c++11"}).output.full_path ==
            "foo.hpp.tinyrefl");
    }
}
