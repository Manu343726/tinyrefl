#ifndef TINYREFL_ENTITIES_MEMBER_VARIABLE_HPP_INCLUDED
#define TINYREFL_ENTITIES_MEMBER_VARIABLE_HPP_INCLUDED

#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/entities/pointer.hpp>
#include <tinyrefl/utils/invokable_traits.hpp>

namespace tinyrefl
{

namespace entities
{

template<
    typename Name,
    typename FullName,
    typename Parent,
    typename SourceLocation,
    typename Pointer,
    typename Attributes>
struct member_variable : public tinyrefl::entities::entity_with_attributes<
                             Attributes,
                             tinyrefl::entities::entity_kind::MEMBER_VARIABLE,
                             Name,
                             FullName,
                             Parent,
                             tinyrefl::entities::no_children,
                             SourceLocation>,
                         public tinyrefl::entities::pointer<Pointer>
{
    using class_type = tinyrefl::invokable_traits::class_type<Pointer>;

    constexpr member_variable() = default;

    constexpr decltype(auto) get(const class_type& obj) const
    {
        return obj.*Pointer::value;
    }

    constexpr decltype(auto) get(class_type& obj) const
    {
        return obj.*Pointer::value;
    }
};
} // namespace entities
} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_MEMBER_VARIABLE_HPP_INCLUDED
