#ifndef TINYREFL_ENTITIES_CONSTRUCTOR_HPP_INCLUDED
#define TINYREFL_ENTITIES_CONSTRUCTOR_HPP_INCLUDED

#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/entities/invokable.hpp>
#include <tinyrefl/types/static_value.hpp>

namespace tinyrefl
{

namespace detail
{

template<typename Class, typename... Args>
constexpr Class constructor_trampoline(Args&&... args)
{
    return Class{std::forward<Args>(args)...};
}

template<typename Class, typename... Args>
using constructor_trampoline_t = Class (*)(Args...);

template<typename Class, typename Args>
struct constructor_trampoline_for_impl;

template<typename Class, typename... Args>
struct constructor_trampoline_for_impl<Class, tinyrefl::meta::list<Args...>>
{
    static constexpr Class call(Args... args)
    {
        return constructor_trampoline<Class>(std::move(args)...);
    }

    using type = tinyrefl::static_value<
        constructor_trampoline_t<Class, Args...>,
        &constructor_trampoline_for_impl::call>;
};

template<typename Class, typename Args>
using constructor_trampoline_for =
    typename constructor_trampoline_for_impl<Class, Args>::type;

} // namespace detail

namespace entities
{

template<
    typename Name,
    typename FullName,
    typename Parent,
    typename SourceLocation,
    typename DisplayName,
    typename FullDisplayName,
    typename Class,
    typename Signature,
    typename ArgNames,
    typename Attributes>
struct constructor
    : public tinyrefl::entities::entity_with_attributes<
          Attributes,
          tinyrefl::entities::entity_kind::CONSTRUCTOR,
          Name,
          FullName,
          Parent,
          tinyrefl::entities::no_children,
          SourceLocation,
          DisplayName,
          FullDisplayName>,
      public tinyrefl::entities::invokable<
          tinyrefl::detail::constructor_trampoline_for<Class, Signature>,
          Class,
          Signature,
          ArgNames>
{
};
} // namespace entities
} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_CONSTRUCTOR_HPP_INCLUDED
