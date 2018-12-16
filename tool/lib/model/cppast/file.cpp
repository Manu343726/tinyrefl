#include <tinyrefl/tool/detail/assert.hpp>
#include <tinyrefl/tool/model/cppast/file.hpp>


namespace tinyrefl
{

namespace tool
{

namespace model
{

template<typename Entity>
entity_refs<Entity> all_entities_with_parent_of_kind(
    const cppast::cpp_entity_index& index,
    const cppast::cpp_file&         file,
    const cppast::cpp_entity_kind   parent_kind)
{
    return child_entities<Entity>(
        index,
        file,
        [parent_kind](
            const cppast::cpp_entity& root, const cppast::cpp_entity& entity) {
            DEBUG_ASSERT(
                entity.parent().has_value(),
                tinyrefl::tool::detail::assert_handler{});

            return entity.parent().value().kind() == parent_kind;
        });
}

entity_refs<cppast::cpp_class> all_namespace_level_classes(
    const cppast::cpp_entity_index& index, const cppast::cpp_file& file)
{
    return all_entities_with_parent_of_kind<cppast::cpp_class>(
        index, file, cppast::cpp_entity_kind::namespace_t);
}

entity_refs<cppast::cpp_enum> all_namespace_level_enums(
    const cppast::cpp_entity_index& index, const cppast::cpp_file& file)
{
    return all_entities_with_parent_of_kind<cppast::cpp_enum>(
        index, file, cppast::cpp_entity_kind::namespace_t);
}

entity_refs<cppast::cpp_class> all_classes(
    const cppast::cpp_entity_index& index, const cppast::cpp_file& file)
{
    return child_entities<cppast::cpp_class>(index, file);
}

entity_refs<cppast::cpp_enum> all_enums(
    const cppast::cpp_entity_index& index, const cppast::cpp_file& file)
{
    return child_entities<cppast::cpp_enum>(index, file);
}
}
}
}
