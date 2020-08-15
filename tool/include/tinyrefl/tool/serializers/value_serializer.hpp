#ifndef TINYREFL_TOOL_SERIALIZERS_VALUE_SERIALIZER_HPP_INCLUDED
#define TINYREFL_TOOL_SERIALIZERS_VALUE_SERIALIZER_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <string>
#include <tinyrefl/tool/entity_names.hpp>
#include <tinyrefl/tool/serializer.hpp>
#include <tinyrefl/tool/serializers/type_serializer.hpp>
#include <type_traits>

namespace tinyrefl
{

namespace tool
{

class value_serializer : public tinyrefl::tool::serializer
{
public:
    value_serializer(
        tinyrefl::tool::string_registry& string_registry,
        cppast::cpp_entity_index&        index);

    template<typename Entity>
    auto value(const Entity& entity) -> std::enable_if_t<
        std::is_base_of<cppast::cpp_entity, Entity>::value,
        std::string>
    {
        return fmt::format(
            "TINYREFL_VALUE(({}), ({}))",
            _type_serializer.type_reference(entity),
            _entity_names.full_name(entity));
    }

    template<typename Entity>
    auto pointer(const Entity& entity) -> std::enable_if_t<
        std::is_base_of<cppast::cpp_entity, Entity>::value,
        std::string>
    {
        return fmt::format(
            "TINYREFL_VALUE(({}), (&{}))",
            _type_serializer.type_reference(
                entity,
                [kind = entity.kind()](const std::string& type) {
                    if(kind == cppast::cpp_entity_kind::variable_t)
                    {
                        return tinyrefl::tool::type_serializer::add_pointer(
                            type);
                    }
                    else
                    {
                        return type;
                    }
                }),
            _entity_names.full_name(
                entity, pointer_must_be_fully_qualified(entity.kind())));
    }

private:
    tinyrefl::tool::type_serializer _type_serializer;
    tinyrefl::tool::entity_names    _entity_names;

    static entity_names::fully_qualified
        pointer_must_be_fully_qualified(const cppast::cpp_entity_kind kind);
};
} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_SERIALIZERS_VALUE_SERIALIZER_HPP_INCLUDED
