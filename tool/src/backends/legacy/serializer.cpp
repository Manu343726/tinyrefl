#include <cassert>
#include <cppast/cpp_entity_kind.hpp>
#include <fmt/format.h>
#include <tinyrefl/tool/backends/legacy/serializer.hpp>

using namespace tinyrefl::tool;

serializer::serializer(tinyrefl::tool::string_registry& string_registry)
    : _string_registry(string_registry)
{
}

const std::string& serializer::string(std::string string)
{
    return _string_registry(std::move(string));
}

std::string serializer::string_constant(std::string string)
{
    return fmt::format("TINYREFL_STRING({})", this->string(std::move(string)));
}

const cppast::cpp_file&
    serializer::entity_root(const cppast::cpp_entity& entity) const
{
    if(!entity.parent().has_value())
    {
        assert(entity.kind() == cppast::cpp_entity_kind::file_t);
        return static_cast<const cppast::cpp_file&>(entity);
    }
    else
    {
        return entity_root(entity.parent().value());
    }
}
