#ifndef TINYREFL_ENTITIES_INVOKABLE_HPP_INCLUDED
#define TINYREFL_ENTITIES_INVOKABLE_HPP_INCLUDED

#include <tinyrefl/entities/function_signature.hpp>
#include <tinyrefl/entities/pointer.hpp>
#include <tinyrefl/entities/type.hpp>
#include <tinyrefl/utils/invokable_traits.hpp>
#include <tinyrefl/utils/meta.hpp>
#include <type_traits>

namespace tinyrefl
{

namespace entities
{

namespace detail
{

template<typename Signature, typename FunctionPointerSignature>
struct check_invokable_signature
{
    static_assert(
        std::is_same<FunctionPointerSignature, Signature>::value,
        "Given pointer does not match the reflected signature");
};

template<
    typename Pointer,
    typename ReturnType,
    typename Arguments_,
    typename ArgNames>
struct invokable_base;

template<
    typename Pointer,
    typename ReturnType,
    typename... Arguments_,
    typename ArgNames>
struct invokable_base<
    Pointer,
    ReturnType,
    tinyrefl::meta::list<Arguments_...>,
    ArgNames> : public tinyrefl::entities::pointer<Pointer>
{
    using Signature = tinyrefl::entities::
        function_signature<ArgNames, ReturnType(Arguments_...)>;

    constexpr Signature signature() const
    {
        return {};
    }

private:
    struct _check : check_invokable_signature<
                        ReturnType(Arguments_...),
                        tinyrefl::invokable_traits::signature<Pointer>>
    {
    };
};

} // namespace detail

template<
    typename Pointer,
    typename ReturnType,
    typename Signature,
    typename ArgNames,
    typename ClassType = tinyrefl::invokable_traits::class_type<Pointer>,
    bool RequiresObjectForCall =
        tinyrefl::invokable_traits::requires_object<Pointer>>
struct invokable : tinyrefl::entities::detail::
                       invokable_base<Pointer, ReturnType, Signature, ArgNames>
{
    static_assert(!std::is_same<ClassType, void>::value, "");
    static_assert(RequiresObjectForCall, "");

    // operator() is overloaded to take into account the constness of the given
    // target object and the method.
    //
    // tinyrefl::invokable_traits::is_const<> calls take the Pointer type as
    // parameter, but obfuscated behind a comma operator so the call depends on
    // the method call arguments and SFINAE applies.
    //
    // Yeah, C++ sucks

    template<typename... Args>
    constexpr auto operator()(const ClassType& obj, Args&&... args) const
        -> std::enable_if_t<
            tinyrefl::invokable_traits::is_const<
                decltype((tinyrefl::meta::list<Args...>{}, Pointer{}))>,
            tinyrefl::invokable_traits::return_type<Pointer>>
    {
        return (obj.*Pointer::value)(std::forward<Args>(args)...);
    }

    template<typename... Args>
    constexpr auto operator()(const ClassType& obj, Args&&... args) const
        -> std::enable_if_t<
            !tinyrefl::invokable_traits::is_const<
                decltype((tinyrefl::meta::list<Args...>{}, Pointer{}))>,
            tinyrefl::invokable_traits::return_type<Pointer>>
    {
        return (obj.*Pointer::value)(std::forward<Args>(args)...);
    }

    template<typename... Args>
    constexpr auto operator()(ClassType& obj, Args&&... args) const
        -> std::enable_if_t<
            !tinyrefl::invokable_traits::is_const<
                decltype((tinyrefl::meta::list<Args...>{}, Pointer{}))>,
            tinyrefl::invokable_traits::return_type<Pointer>>
    {
        return (obj.*Pointer::value)(std::forward<Args>(args)...);
    }

    constexpr auto bind(const ClassType& obj) const
    {
        return [&obj](auto&&... args) {
            return invokable{}(std::forward<decltype(args)>(args)...);
        };
    }

    constexpr auto bind(ClassType& obj) const
    {
        return [&obj](auto&&... args) {
            return invokable{}(std::forward<decltype(args)>(args)...);
        };
    }
};

template<
    typename Pointer,
    typename ReturnType,
    typename Signature,
    typename ArgNames,
    typename ClassType>
struct invokable<Pointer, ReturnType, Signature, ArgNames, ClassType, false>
    : public tinyrefl::entities::detail::
          invokable_base<Pointer, ReturnType, Signature, ArgNames>
{
    template<typename... Args>
    constexpr tinyrefl::invokable_traits::return_type<Pointer>
        operator()(Args&&... args) const
    {
        return (*Pointer::value)(std::forward<Args>(args)...);
    }
};

} // namespace entities
} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_INVOKABLE_HPP_INCLUDED
