#include <tinyrefl/tool/unique_id.hpp>

namespace tinyrefl
{

namespace tool
{

std::uint64_t uniqueId(const tinyrefl::string& fullDisplayName)
{
    return fullDisplayName.hash();
}

} // namespace tool

} // namespace tinyrefl
