#ifndef TINYREFL_TOOL_MODEL_CPPAST_COMMON_HPP
#define TINYREFL_TOOL_MODEL_CPPAST_COMMON_HPP

#include <algorithm>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_index.hpp>
#include <iterator>
#include <tinyrefl/tool/detail/assert.hpp>
#include <type_safe/reference.hpp>
#include <vector>

namespace tinyrefl
{

namespace tool
{

namespace model
{

template<typename T>
struct entity_ref
{
    entity_ref() = default;

    entity_ref(
        const cppast::cpp_entity_index&         index,
        const T&                                entity,
        const cppast::cpp_access_specifier_kind access =
            cppast::cpp_access_specifier_kind::cpp_public)
        : entity_ref{index, type_safe::cref(entity), access}
    {
    }

    entity_ref(
        const cppast::cpp_entity_index&         index,
        const type_safe::object_ref<const T>&   entity,
        const cppast::cpp_access_specifier_kind access =
            cppast::cpp_access_specifier_kind::cpp_public)
        : _access{access}, _entity{&entity.get()}, _index{&index}
    {
    }

    const T* operator->() const
    {
        return &entity();
    }

    operator const T&() const
    {
        return entity();
    }

    bool is_public() const
    {
        return access() == cppast::cpp_access_specifier_kind::cpp_public;
    }

    bool has_parent() const
    {
        return entity().parent().has_value();
    }

    cppast::cpp_access_specifier_kind access() const
    {
        return _access;
    }

    const cppast::cpp_entity_index& index() const
    {
        DEBUG_ASSERT(
            _index != nullptr, tinyrefl::tool::detail::assert_handler{});
        return *_index;
    }

    const T& entity() const
    {
        DEBUG_ASSERT(
            _entity != nullptr, tinyrefl::tool::detail::assert_handler{});
        return *_entity;
    }

    template<typename Parent>
    entity_ref<Parent> parent_ref() const
    {
        DEBUG_ASSERT(has_parent(), tinyrefl::tool::detail::assert_handler{});
        DEBUG_ASSERT(
            entity().parent().value().kind() == Parent::kind(),
            tinyrefl::tool::detail::assert_handler{});

        return {index(), static_cast<const Parent&>(entity().parent().value())};
    }

private:
    cppast::cpp_access_specifier_kind _access = cppast::cpp_public;
    const T*                          _entity = nullptr;
    const cppast::cpp_entity_index*   _index  = nullptr;
};

template<typename T>
entity_ref<T> make_entity_ref(
    const cppast::cpp_entity_index&         index,
    const T&                                entity,
    const cppast::cpp_access_specifier_kind access =
        cppast::cpp_access_specifier_kind::cpp_public)
{
    return {index, entity, access};
}

template<typename T>
using entity_refs = std::vector<entity_ref<T>>;

bool has_complete_name(const cppast::cpp_entity& entity);
std::string full_qualified_name(const cppast::cpp_entity& entity);

template<typename T>
entity_refs<T> public_entities_only(const entity_refs<T>& entities)
{
    entity_refs<T> result;

    std::copy_if(
        entities.begin(),
        entities.end(),
        std::back_inserter(result),
        [](const entity_ref<T>& entity) { return entity.is_public(); });

    return result;
}
}
}
}

#endif // TINYREFL_TOOL_MODEL_CPPAST_COMMON_HPP
