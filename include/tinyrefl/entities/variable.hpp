#ifndef TINYREFL_ENTITIES_VARIABLE_HPP_INCLUDED
#define TINYREFL_ENTITIES_VARIABLE_HPP_INCLUDED

#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/entities/function_arguments.hpp>
#include <tinyrefl/entities/pointer.hpp>

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
struct variable : public tinyrefl::entities::entity_with_attributes<
                      Attributes,
                      tinyrefl::entities::entity_kind::VARIABLE,
                      Name,
                      FullName,
                      Parent,
                      tinyrefl::entities::no_children,
                      SourceLocation>,
                  public tinyrefl::entities::pointer<Pointer>
{
    constexpr variable() = default;

    using value_type = tinyrefl::invokable_traits::return_type<Pointer>;
};
} // namespace entities
} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_VARIABLE_HPP_INCLUDED
