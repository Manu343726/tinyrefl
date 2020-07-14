#ifndef TINYREFL_ENTITIES_MEMBER_FUNCTION_HPP_INCLUDED
#define TINYREFL_ENTITIES_MEMBER_FUNCTION_HPP_INCLUDED

#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/entities/invokable.hpp>

namespace tinyrefl
{

namespace entities
{

template<
    typename Name,
    typename FullName,
    typename Parent,
    typename SourceLocation,
    typename DisplayName,
    typename FullDisplayName,
    typename ReturnType,
    typename Parameters,
    typename ParameterNames,
    typename Pointer,
    typename Attributes>
struct member_function
    : public tinyrefl::entities::entity_with_attributes<
          Attributes,
          tinyrefl::entities::entity_kind::MEMBER_FUNCTION,
          Name,
          FullName,
          Parent,
          tinyrefl::entities::no_children,
          SourceLocation,
          DisplayName,
          FullDisplayName>,
      public tinyrefl::entities::
          invokable<Pointer, ReturnType, Parameters, ParameterNames>
{
    constexpr member_function() = default;

    template<typename Class, typename... Args>
    constexpr auto operator()(Class&& object, Args&&... args) const
    {
        return (std::forward<Class>(object).Pointer::value)(
            std::forward<Args>(args)...);
    }
};

} // namespace entities
} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_MEMBER_FUNCTION_HPP_INCLUDED
