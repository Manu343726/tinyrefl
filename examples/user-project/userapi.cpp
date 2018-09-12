#include "userapi.hpp"
#include <tinyrefl/api.hpp>
#include "userapi.hpp.tinyrefl"

using namespace userapi;

std::string EntryPoint::SessionData::to_string() const
{
    return tinyrefl::to_string(*this);
}

bool EntryPoint::SessionData::
     operator==(const EntryPoint::SessionData& other) const
{
    return tinyrefl::equal(*this, other);
}

bool EntryPoint::SessionData::
     operator!=(const EntryPoint::SessionData& other) const
{
    return tinyrefl::not_equal(*this, other);
}

EntryPoint::EntryPoint(const EntryPoint::SessionData& sessionData)
    : _sessionData{sessionData}
{
}

const EntryPoint::SessionData& EntryPoint::sessionData() const
{
    return _sessionData;
}
