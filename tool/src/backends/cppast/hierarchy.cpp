#include <cassert>
#include <tinyrefl/tool/backends/cppast/hierarchy.hpp>
#include <cppast/cpp_entity_kind.hpp>

using namespace tinyrefl::tool::cppast_backend;

const cppast::cpp_file&
    HierarchyReader::root(const cppast::cpp_entity& entity) const
{
    const auto parent = this->parent(entity);

    if(not parent)
    {
        assert(entity.kind() == cppast::cpp_entity_kind::file_t);
        return static_cast<const cppast::cpp_file&>(entity);
    }
    else
    {
        return root(parent.value());
    }
}

type_safe::optional_ref<const cppast::cpp_entity>
    HierarchyReader::parent(const cppast::cpp_entity& entity) const
{
    return entity.parent();
}
