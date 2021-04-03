#include <tinyrefl/tool/backends/legacy/serializers/value_serializer.hpp>

using namespace tinyrefl::tool;

value_serializer::value_serializer(
    tinyrefl::tool::string_registry& string_registry,
    cppast::cpp_entity_index&        index)
    : tinyrefl::tool::serializer{string_registry},
      _type_serializer{string_registry, index},
      _entity_names{string_registry}
{
}
