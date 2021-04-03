#ifndef TINYREFL_TOOL_UNIQUEID_HPP_INCLUDED
#define TINYREFL_TOOL_UNIQUEID_HPP_INCLUDED

#include <cstdint>
#include <tinyrefl/types/string.hpp>

namespace tinyrefl
{

namespace tool
{

std::uint64_t uniqueId(const tinyrefl::string& fullDisplayName);

}

} // namespace tinyrefl

#endif // TINYREFL_TOOL_UNIQUEID_HPP_INCLUDED
