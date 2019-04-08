#ifndef TINYTREFL_TOOL_VISITOR_HPP
#define TINYTREFL_TOOL_VISITOR_HPP

#include <algorithm>
#include <iostream> // TODO: Remove, just for debugging

#include <type_safe/reference.hpp>
#include <type_safe/variant.hpp>

#include <cppast/cpp_class.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_variable.hpp>
#include <cppast/visitor.hpp>

#include <tinyrefl/types/array_view.hpp>
#include <tinyrefl/utils/meta.hpp>
#include <tinyrefl/utils/overloaded_function.hpp>
#include <tinyrefl/utils/switch.hpp>

namespace tinyrefl
{

namespace tool
{

template<cppast::cpp_entity_kind Kind>
using cpp_entity_kind = std::integral_constant<cppast::cpp_entity_kind, Kind>;

namespace detail
{

template<typename T>
struct add_const_pointer
{
    using type = const T;
};

template<typename Entity, cppast::cpp_entity_kind Kind>
using entity_to_kind = tinyrefl::meta::pair<Entity, cpp_entity_kind<Kind>>;

} // namespace detail


using cppast_entities_to_kinds = tinyrefl::meta::slow_map<
    detail::entity_to_kind<
        cppast::cpp_namespace,
        cppast::cpp_entity_kind::namespace_t>,
    detail::entity_to_kind<
        cppast::cpp_function,
        cppast::cpp_entity_kind::function_t>,
    detail::entity_to_kind<
        cppast::cpp_variable,
        cppast::cpp_entity_kind::variable_t>,
    detail::entity_to_kind<cppast::cpp_class, cppast::cpp_entity_kind::class_t>,
    detail::entity_to_kind<
        cppast::cpp_member_function,
        cppast::cpp_entity_kind::member_function_t>,
    detail::entity_to_kind<
        cppast::cpp_member_variable,
        cppast::cpp_entity_kind::member_variable_t>,
    detail::entity_to_kind<
        cppast::cpp_constructor,
        cppast::cpp_entity_kind::constructor_t>,
    detail::entity_to_kind<cppast::cpp_enum, cppast::cpp_entity_kind::enum_t>,
    detail::entity_to_kind<
        cppast::cpp_enum_value,
        cppast::cpp_entity_kind::enum_value_t>>;

using cppast_kinds_to_entities =
    tinyrefl::meta::inverted_map<cppast_entities_to_kinds>;

static_assert(
    !std::is_same<cppast_entities_to_kinds, cppast_kinds_to_entities>::value,
    "");

using cppast_entities = tinyrefl::meta::keys_t<cppast_entities_to_kinds>;
using cppast_kinds    = tinyrefl::meta::values_t<cppast_entities_to_kinds>;

template<typename... Callbacks>
constexpr void
    entity_cast(const cppast::cpp_entity& entity, Callbacks... callbacks)
{
    tinyrefl::switch_<cppast_kinds>(entity.kind())(
        [&entity,
         callbacks =
             tinyrefl::overloaded_function_default(callbacks...)](auto kind)
            -> std::enable_if_t<
                tinyrefl::meta::contains<decltype(kind), cppast_kinds>::value> {
            using Kind = decltype(kind);

            callbacks(static_cast<const tinyrefl::meta::
                                      at_key<cppast_kinds_to_entities, Kind>&>(
                entity));
        },
        [](auto kind) TINYREFL_CONSTEXPR_LAMBDA
        -> std::enable_if_t<
            !tinyrefl::meta::contains<decltype(kind), cppast_kinds>::value>{
            // does nothing
        });
}

template<typename... Visitors>
void visit_entity(const cppast::cpp_entity& root, Visitors... visitors)
{
    const auto filter = [&root](const cppast::cpp_entity& entity) {
        constexpr tinyrefl::array_view<const cppast::cpp_entity_kind>
            supported_kinds = tinyrefl::typelist_to_array<
                tinyrefl::tool::cppast_kinds>::value;

        const bool supported = std::find(
                                   supported_kinds.begin(),
                                   supported_kinds.end(),
                                   entity.kind()) != supported_kinds.end();

        return supported && entity.parent().has_value() &&
               &entity.parent().value() == &root;
    };

    cppast::visit(
        root,
        filter,
        [visitors = tinyrefl::overloaded_function_default(visitors...)](
            const cppast::cpp_entity&   entity,
            const cppast::visitor_info& info) {
            if(info.is_new_entity())
            {
                tinyrefl::tool::entity_cast(entity, visitors);
            }
        });

    return;
}

} // namespace tool
} // namespace tinyrefl

#endif // TINYREFL_TOOL_VISITOR_HPP
