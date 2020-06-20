#ifndef TINYREFL_OBJECT_VISITOR_HPP
#define TINYREFL_OBJECT_VISITOR_HPP

#include <tinyrefl/class_visitor.hpp>
#include <tinyrefl/visitor.hpp>

namespace tinyrefl
{

namespace impl
{

template<typename Visitor, typename... Class>
struct constexpr_member_variable_visitor
{
    constexpr constexpr_member_variable_visitor(
        Class&... objects, Visitor&& visitor)
        : objects{&objects...}, visitor{std::move(visitor)}
    {
    }

    template<typename MemberVariable>
    constexpr auto operator()(const MemberVariable&) const -> std::enable_if_t<
        MemberVariable().kind() ==
            tinyrefl::entities::entity_kind::MEMBER_VARIABLE,
        decltype(std::declval<Visitor>()(
            MemberVariable{}.name(),
            MemberVariable{}.get(std::declval<Class&>())...))>
    {
        call(MemberVariable{}, std::index_sequence_for<Class...>{});
    }

    std::tuple<Class*...> objects;
    Visitor               visitor;

private:
    template<typename MemberVariable, std::size_t... Is>
    constexpr auto call(const MemberVariable&, std::index_sequence<Is...>) const
        -> decltype(std::declval<Visitor>()(
            MemberVariable{}.name(),
            MemberVariable{}.get(std::declval<Class&>())...))
    {
        visitor(
            MemberVariable{}.name(),
            MemberVariable{}.get(*std::get<Is>(objects))...);
    }
};

template<typename... Class, typename Visitor>
constexpr constexpr_member_variable_visitor<
    std::decay_t<Visitor>,
    std::remove_reference_t<Class>...>
    make_constexpr_member_variable_visitor(Visitor&& visitor, Class&... objects)
{
    return {objects..., std::forward<Visitor>(visitor)};
}

template<typename... Class, typename Visitor, std::size_t... Is>
constexpr constexpr_member_variable_visitor<
    std::decay_t<Visitor>,
    std::remove_reference_t<Class>...>
    make_constexpr_member_variable_visitor(
        Visitor&&                    visitor,
        const std::tuple<Class&...>& objects,
        std::index_sequence<Is...>)
{
    return {std::get<Is>(objects)..., std::forward<Visitor>(visitor)};
}

template<typename... Class, typename Visitor>
constexpr constexpr_member_variable_visitor<
    std::decay_t<Visitor>,
    std::remove_reference_t<Class>...>
    make_constexpr_member_variable_visitor(
        Visitor&& visitor, const std::tuple<Class&...>& objects)
{
    return make_constexpr_member_variable_visitor(
        std::forward<Visitor>(visitor),
        objects,
        std::index_sequence_for<Class...>{});
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
    constexpr auto operator()(
        const MemberFunction&,
        tinyrefl::entities::kind_constant<
            tinyrefl::entities::entity_kind::MEMBER_FUNCTION>) const
        -> decltype(std::declval<Visitor>()(
            MemberFunction{}.display_name(),
            make_bound_member_function<MemberFunction>(std::declval<Class&>())))
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

template<typename... Class, typename... Visitors>
constexpr void visit_member_variables(
    const std::tuple<Class&...>& objects, Visitors&&... visitors)
{
    auto visitor = ::tinyrefl::impl::make_constexpr_member_variable_visitor(
        tinyrefl::overloaded_function(std::forward<Visitors>(visitors)...),
        objects);

    using class_type = std::decay_t<tinyrefl::meta::pack_head_t<Class...>>;
    tinyrefl::visit_class<class_type>(visitor);
}

template<typename Class, typename... Visitors>
constexpr void visit_member_variables(Class& object, Visitors&&... visitors)
{
    auto visitor = ::tinyrefl::impl::make_constexpr_member_variable_visitor(
        tinyrefl::overloaded_function(std::forward<Visitors>(visitors)...),
        object);

    using class_type = std::decay_t<Class>;
    tinyrefl::visit_class<class_type>(visitor);
}


template<typename Class, typename... Visitors>
constexpr void visit_member_functions(Class& object, Visitors&&... visitors)
{
    auto visitor = ::tinyrefl::impl::make_constexpr_member_function_visitor(
        object,
        tinyrefl::overloaded_function(std::forward<Visitors>(visitors)...));

    using class_type = std::decay_t<Class>;
    tinyrefl::visit_class<class_type>(visitor);
}
} // namespace tinyrefl

#endif // TINYREFL_OBJECT_VISITOR_HPP
