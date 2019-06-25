#ifndef TINYTREFL_TOOL_TYPE_VISITOR_HPP
#define TINYTREFL_TOOL_TYPE_VISITOR_HPP

#include <algorithm>
#include <iostream> // TODO: Remove, just for debugging

#include <type_safe/reference.hpp>
#include <type_safe/variant.hpp>

#include <cppast/cpp_template.hpp>
#include <cppast/cpp_type.hpp>

#include <tinyrefl/types/array_view.hpp>
#include <tinyrefl/utils/meta.hpp>
#include <tinyrefl/utils/overloaded_function.hpp>
#include <tinyrefl/utils/switch.hpp>

namespace tinyrefl
{

namespace tool
{

template<cppast::cpp_type_kind Kind>
using cpp_type_kind = std::integral_constant<cppast::cpp_type_kind, Kind>;

namespace detail
{

template<typename T>
struct add_const_pointer
{
    using type = const T;
};

template<typename Type, cppast::cpp_type_kind Kind>
using type_to_kind = tinyrefl::meta::pair<Type, cpp_type_kind<Kind>>;

} // namespace detail


using cppast_types_to_kinds = tinyrefl::meta::slow_map<
    detail::type_to_kind<
        cppast::cpp_builtin_type,
        cppast::cpp_type_kind::builtin_t>,
    detail::type_to_kind<
        cppast::cpp_pointer_type,
        cppast::cpp_type_kind::pointer_t>,
    detail::type_to_kind<
        cppast::cpp_reference_type,
        cppast::cpp_type_kind::reference_t>,
    detail::type_to_kind<
        cppast::cpp_cv_qualified_type,
        cppast::cpp_type_kind::cv_qualified_t>,
    detail::type_to_kind<
        cppast::cpp_user_defined_type,
        cppast::cpp_type_kind::user_defined_t>,
    detail::type_to_kind<
        cppast::cpp_template_instantiation_type,
        cppast::cpp_type_kind::template_instantiation_t>>;

using cppast_kinds_to_types =
    tinyrefl::meta::inverted_map<cppast_types_to_kinds>;

static_assert(
    !std::is_same<cppast_types_to_kinds, cppast_kinds_to_types>::value, "");

using cppast_types      = tinyrefl::meta::keys_t<cppast_types_to_kinds>;
using cppast_type_kinds = tinyrefl::meta::values_t<cppast_types_to_kinds>;

template<typename... Callbacks>
constexpr void type_cast(const cppast::cpp_type& type, Callbacks... callbacks)
{
    tinyrefl::switch_<cppast_type_kinds>(type.kind())(
        [&type,
         callbacks =
             tinyrefl::overloaded_function_default(callbacks...)](auto kind)
            -> std::enable_if_t<tinyrefl::meta::contains<
                decltype(kind),
                cppast_type_kinds>::value> {
            using Kind = decltype(kind);

            callbacks(
                static_cast<
                    const tinyrefl::meta::at_key<cppast_kinds_to_types, Kind>&>(
                    type));
        },
        [](auto kind) TINYREFL_CONSTEXPR_LAMBDA
        -> std::enable_if_t<!tinyrefl::meta::contains<
            decltype(kind),
            cppast_type_kinds>::value>{
            // does nothing
        });
}

} // namespace tool
} // namespace tinyrefl

#endif // TINYREFL_TOOL_TYPE_VISITOR_HPP
