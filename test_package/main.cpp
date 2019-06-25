#include "example.hpp"
#include <iostream>
#include <tinyrefl/api.hpp>
#include "example.hpp.tinyrefl"

static_assert(!tinyrefl::metadata<example::Example>().has_attribute("foo"), "");
static_assert(
    tinyrefl::metadata<example::Example>().attribute("foobar").namespace_() ==
        "",
    "");

int main()
{
    const auto json = tinyrefl::to_json(example::Example{});
    std::cout << json.dump() << "\n";

    const auto example = tinyrefl::from_json<example::Example>(json);
    std::cout << tinyrefl::to_string(example) << "\n";
}
