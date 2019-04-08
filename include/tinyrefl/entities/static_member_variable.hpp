#ifndef TINYREFL_ENTITIES_STATIC_MEMBER_VARIABLE_HPP_INCLUDED
#define TINYREFL_ENTITIES_STATIC_MEMBER_VARIABLE_HPP_INCLUDED

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
struct static_member_variable
    : public tinyrefl::entities::entity_with_attributes<
          Attributes,
          tinyrefl::entities::entity_kind::STATIC_MEMBER_VARIABLE,
          Name,
          FullName,
          Parent,
          tinyrefl::entities::no_children,
          SourceLocation>,
      public tinyrefl::entities::pointer<Pointer>
{
    constexpr static_member_variable() = default;
};
} // namespace entities
} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_STATIC_MEMBER_VARIABLE_HPP_INCLUDED
