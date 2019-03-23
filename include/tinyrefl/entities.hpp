#ifndef TINYREFL_ENTITIES_HPP_INCLUDED
#define TINYREFL_ENTITIES_HPP_INCLUDED

#include <type_traits>

#if !defined(TINYREFL_API_HPP) || !defined(TINYREFL_ENTITIES)
#error \
    "To use this header, first include your reflected headers, then <tinyrefl/api.hpp>, then your reflected headers generated code (.tinyrefl header files), and then this header"
#endif

namespace tinyrefl
{

using entities = tinyrefl::meta::fmap_t<
    tinyrefl::meta::defer<tinyrefl::backend::metadata_of_entity_name>,
    TINYREFL_ENTITIES>;

template<typename Entity>
constexpr ctti::detail::cstring display_name(Entity entity = Entity{})
{
    static_assert(std::is_class<Entity>::value, "");
    return tinyrefl::backend::display_name<Entity>();
}

template<typename Entity>
constexpr ctti::detail::cstring full_display_name(Entity entity = Entity{})
{
    static_assert(std::is_class<Entity>::value, "");
    return tinyrefl::backend::full_display_name<Entity>();
}

template<typename... Visitors>
void visit_entities(Visitors... visitors)
{
    tinyrefl::meta::foreach<tinyrefl::entities>(
        [visitor = tinyrefl::overloaded_function_default(visitors...)](
            auto type, auto index) {
            using Entity = typename decltype(type)::type;

            static_assert(std::is_class<Entity>::value, "");

            visitor(
                tinyrefl::display_name<Entity>(),
                index,
                Entity{},
                kind_tag_value<Entity::kind>);
        });
}

template<tinyrefl::entity Kind, typename Visitor>
void visit_entities(Visitor visitor)
{
    visit_entities(
        [visitor](
            auto display_name, auto /* index */, auto entity, kind_tag<Kind>) {
            visitor(display_name, entity);
        });
}

template<typename Visitor>
void visit_classes(Visitor visitor)
{
    visit_entities<tinyrefl::entity::CLASS>(visitor);
}

template<typename Visitor>
void visit_enums(Visitor visitor)
{
    visit_entities<tinyrefl::entity::ENUM>(visitor);
}
} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_HPP_INCLUDED
