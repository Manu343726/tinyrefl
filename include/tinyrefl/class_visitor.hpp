#ifndef TINYREFL_CLASS_VISITOR_HPP_INCLUDED
#define TINYREFL_CLASS_VISITOR_HPP_INCLUDED

#include <tinyrefl/visitor.hpp>
#include <type_traits>

namespace tinyrefl
{

template<typename Entity, typename... Visitors>
constexpr void visit_class(Entity e, Visitors&&... visitors)
{
    constexpr std::decay_t<decltype(e)> entity;
    static_assert(
        entity.kind() == tinyrefl::entities::entity_kind::CLASS or
            entity.kind() == tinyrefl::entities::entity_kind::BASE_CLASS,
        "tinyrefl::visit_class() can only invoked with class types");

    const auto visitor =
        tinyrefl::overloaded_function(std::forward<Visitors>(visitors)...);
    tinyrefl::visit(entity, visitor);
    tinyrefl::visit(entity.children(), visitor);

    tinyrefl::meta::foreach(
        entity.bases(), [visitor] TINYREFL_CONSTEXPR_LAMBDA(auto e) {
            constexpr decltype(e) base_class;
            visit_class(base_class, visitor);
        });
}

template<typename Class, typename... Visitors>
constexpr void visit_class(Visitors&&... visitors)
{
    static_assert(
        std::is_class<Class>::value,
        "tinyrefl::visit_class() can only invoked with class types");
    visit_class(
        tinyrefl::metadata<Class>(), std::forward<Visitors>(visitors)...);
}

} // namespace tinyrefl

#endif // TINYREFL_CLASS_VISITOR_HPP_INCLUDED
