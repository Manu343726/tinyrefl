#ifndef TINYREFL_TOOL_MODEL_CPPAST_COMMON_HPP
#define TINYREFL_TOOL_MODEL_CPPAST_COMMON_HPP

#include <algorithm>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_index.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/visitor.hpp>
#include <iterator>
#include <tinyrefl/tool/detail/assert.hpp>
#include <tinyrefl/tool/detail/logger.hpp>
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
    entity_ref() = delete;

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
        DEBUG_ASSERT(!_destroyed, tinyrefl::tool::detail::assert_handler{});
        DEBUG_ASSERT(
            _index != nullptr, tinyrefl::tool::detail::assert_handler{});
        return *_index;
    }

    const T& entity() const
    {
        DEBUG_ASSERT(!_destroyed, tinyrefl::tool::detail::assert_handler{});
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

    ~entity_ref()
    {
        _entity    = nullptr;
        _index     = nullptr;
        _destroyed = true;
    }

private:
    cppast::cpp_access_specifier_kind _access    = cppast::cpp_public;
    const T*                          _entity    = nullptr;
    const cppast::cpp_entity_index*   _index     = nullptr;
    bool                              _destroyed = false;
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
std::string full_decorated_name(const cppast::cpp_entity& entity);
bool entity_has_comment(const cppast::cpp_entity& entity);
std::string entity_comment(const cppast::cpp_entity& entity);

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

namespace entity_filters
{
inline bool always_true(
    const cppast::cpp_entity& root, const cppast::cpp_entity& entity)
{
    return true;
}
} // namespace entity_filters

template<typename Entity, typename Predicate>
entity_refs<Entity> child_entities(
    const cppast::cpp_entity_index& index,
    const cppast::cpp_entity&       root,
    Predicate                       predicate)
{
    entity_refs<Entity> result;

    cppast::visit(
        root,
        [&result, &root, &index, predicate](
            const cppast::cpp_entity&   entity,
            const cppast::visitor_info& info) {
            const bool has_expected_kind = entity.kind() == Entity::kind();
            const bool has_complete_name =
                tinyrefl::tool::model::has_complete_name(entity);
            const bool predicate_passed = predicate(root, entity);

            if(info.is_new_entity())
            {
                if(has_expected_kind && has_complete_name && predicate_passed)
                {
                    result.emplace_back(
                        index, static_cast<const Entity&>(entity), info.access);
                }
                else
                {
                    tinyrefl::tool::detail::log().trace(
                        "members({}, kind: {}): child \"{}\" ignored. Reason: [{}] [{}] [{}]",
                        full_qualified_name(root),
                        cppast::to_string(Entity::kind()),
                        full_qualified_name(entity),
                        (!has_expected_kind
                             ? fmt::format(
                                   "not a {}",
                                   cppast::to_string(Entity::kind()))
                             : ""),
                        (!has_complete_name ? "invalid full name" : ""),
                        (!predicate_passed ? "custom filter not passed" : ""));
                }
            }
        });

    return result;
}

bool is_direct_child_of(
    const cppast::cpp_entity& entity, const cppast::cpp_entity& parent);

template<typename Entity, typename Predicate>
entity_refs<Entity> direct_child_entities(
    const cppast::cpp_entity_index& index,
    const cppast::cpp_entity&       root,
    Predicate                       predicate)
{
    return child_entities<Entity>(
        index,
        root,
        [predicate](
            const cppast::cpp_entity& root, const cppast::cpp_entity& entity) {
            return is_direct_child_of(entity, root) && predicate(root, entity);
        });
}

template<typename Entity>
entity_refs<Entity> child_entities(
    const cppast::cpp_entity_index& index, const cppast::cpp_entity& root)
{
    return child_entities<Entity>(index, root, entity_filters::always_true);
}

template<typename Entity>
entity_refs<Entity> direct_child_entities(
    const cppast::cpp_entity_index& index, const cppast::cpp_entity& root)
{
    return direct_child_entities<Entity>(
        index, root, entity_filters::always_true);
}

bool equal_entities(
    const cppast::cpp_entity& lhs, const cppast::cpp_entity& rhs);

template<typename Lhs, typename Rhs>
bool operator==(const entity_ref<Lhs>& lhs, const entity_ref<Rhs>& rhs)
{
    return equal_entities(lhs.entity(), rhs.entity());
}
} // namespace model
} // namespace tool
} // namespace tinyrefl

#endif // TINYREFL_TOOL_MODEL_CPPAST_COMMON_HPP
