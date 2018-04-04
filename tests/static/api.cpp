#include "template_using_reflection.hpp"
#include "members.hpp"
#include "members.hpp.tinyrefl"

using check = tinyrefl::test::AssertMetadataAvailableForTemplateParam<foo::Foo>;
constexpr check triggerCheck;
