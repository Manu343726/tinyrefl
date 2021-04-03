#ifndef TINYREFL_TOOL_SERIALIZER_HPP_INCLUDED
#define TINYREFL_TOOL_SERIALIZER_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_file.hpp>

#include <tinyrefl/tool/backends/legacy/string_registry.hpp>

namespace tinyrefl
{

namespace tool
{

class serializer
{
public:
    serializer(tinyrefl::tool::string_registry& string_registry);

protected:
    const std::string&      string(std::string string);
    std::string             string_constant(std::string string);
    const cppast::cpp_file& entity_root(const cppast::cpp_entity& entity) const;

private:
    tinyrefl::tool::string_registry& _string_registry;
};

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_SERIALIZER_HPP_INCLUDED
