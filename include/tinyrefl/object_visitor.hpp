#ifndef TINYREFL_OBJECT_VISITOR_HPP
#define TINYREFL_OBJECT_VISITOR_HPP

#include <tinyrefl/visitor.hpp>

namespace tinyrefl
{

namespace impl
{

template<typename Class, typename Visitor>
struct constexpr_member_variable_visitor
{
    constexpr constexpr_member_variable_visitor(
        Class& object, Visitor&& visitor)
        : object{&object}, visitor{std::move(visitor)}
    {
    }

    template<typename MemberVariable>
    constexpr void operator()(
        const MemberVariable&,
        tinyrefl::entities::kind_constant<
            tinyrefl::entities::entity_kind::MEMBER_VARIABLE>) const
    {
        visitor(MemberVariable{}.name(), MemberVariable{}.get(*object));
    }

    Class*  object;
    Visitor visitor;
};

template<typename Class, typename Visitor>
constexpr constexpr_member_variable_visitor<
    std::remove_reference_t<Class>,
    std::decay_t<Visitor>>
    make_constexpr_member_variable_visitor(Class& object, Visitor&& visitor)
{
    return {object, std::forward<Visitor>(visitor)};
}

template<typename Class, typename MemberFunction>
struct bound_member_function
{
    constexpr bound_member_function(Class& object) : object{&object} {}

    template<typename... Args>
    constexpr auto operator()(Args&&... args)
    {
        return MemberFunction{}(*object, std::forward<Args>(args)...);
    }

    Class* object;
};

template<typename MemberFunction, typename Class>
constexpr auto make_bound_member_function(Class& object)
{
    return bound_member_function<Class, MemberFunction>{object};
}

template<typename Class, typename Visitor>
struct constexpr_member_function_visitor
{
    constexpr constexpr_member_function_visitor(
        Class& object, Visitor&& visitor)
        : object{&object}, visitor{std::move(visitor)}
    {
    }

    template<typename MemberFunction>
    constexpr void operator()(
        const MemberFunction&,
        tinyrefl::entities::kind_constant<
            tinyrefl::entities::entity_kind::MEMBER_FUNCTION>) const
    {
        visitor(
            MemberFunction{}.display_name(),
            make_bound_member_function<MemberFunction>(*object));
    }

    Class*  object;
    Visitor visitor;
};

template<typename Class, typename Visitor>
constexpr constexpr_member_function_visitor<
    std::remove_reference_t<Class>,
    std::decay_t<Visitor>>
    make_constexpr_member_function_visitor(Class& object, Visitor&& visitor)
{
    return {object, std::forward<Visitor>(visitor)};
}

} // namespace impl

template<typename Class, typename... Visitors>
constexpr void visit_member_variables(Class& object, Visitors&&... visitors)
{
    tinyrefl::visit<std::decay_t<Class>>(
        tinyrefl::impl::make_constexpr_member_variable_visitor(
            object, std::forward<Visitors>(visitors))...);
}


template<typename Class, typename... Visitors>
constexpr void visit_member_functions(Class& object, Visitors&&... visitors)
{
    tinyrefl::visit<std::decay_t<Class>>(
        tinyrefl::impl::make_constexpr_member_function_visitor(
            object, std::forward<Visitors>(visitors))...);
}
} // namespace tinyrefl

#endif // TINYREFL_OBJECT_VISITOR_HPP
