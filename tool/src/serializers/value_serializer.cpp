#include <tinyrefl/tool/serializers/value_serializer.hpp>

using namespace tinyrefl::tool;

value_serializer::value_serializer(
    tinyrefl::tool::string_registry& string_registry,
    cppast::cpp_entity_index&        index)
    : tinyrefl::tool::serializer{string_registry},
      _type_serializer{string_registry, index},
      _entity_names{string_registry}
{
}

entity_names::fully_qualified value_serializer::pointer_must_be_fully_qualified(
    const cppast::cpp_entity_kind kind)
{
    switch(kind)
    {
    case cppast::cpp_entity_kind::function_t:
        return entity_names::fully_qualified::YES;
    default:
        return entity_names::fully_qualified::NO;
    }
}
