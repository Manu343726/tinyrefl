#ifndef TINYREFL_TOOL_STRING_REGISTRY_HPP_INCLUDED
#define TINYREFL_TOOL_STRING_REGISTRY_HPP_INCLUDED

#include <string>
#include <unordered_set>

namespace tinyrefl
{

namespace tool
{

class string_registry
{
public:
    const std::string& operator()(std::string string)
    {
        return *_registry.insert(std::move(string)).first;
    }

    using iterator = std::unordered_set<std::string>::const_iterator;

    iterator begin() const
    {
        return _registry.begin();
    }

    iterator end() const
    {
        return _registry.end();
    }

private:
    std::unordered_set<std::string> _registry;
};
} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_STRING_REGISTRY_HPP_INCLUDED
