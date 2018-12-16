#include <tinyrefl/api.hpp>
#include <tinyrefl/tool/model/model.hpp>
#include <tinyrefl/tool/model/model.hpp.tinyrefl>

using namespace tinyrefl::tool::model;

static_assert(tinyrefl::has_metadata<enum_value>(), "");
static_assert(tinyrefl::has_metadata<enum_>(), "");

std::string enum_value::to_string() const
{
    return tinyrefl::to_string(*this);
}

std::string enum_::to_string() const
{
    return tinyrefl::to_string(*this);
}
