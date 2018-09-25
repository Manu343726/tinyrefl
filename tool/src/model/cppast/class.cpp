#include <algorithm>
#include <cppast/visitor.hpp>
#include <fmt/format.h>
#include <tinyrefl/tool/detail/assert.hpp>
#include <tinyrefl/tool/model/cppast/class.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

entity_refs<cppast::cpp_class> base_classes(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_)
{
    entity_refs<cppast::cpp_class> result;

    for(const auto& base : class_.bases())
    {
        const auto& type = base.type();
        DEBUG_ASSERT(
            type.kind() == cppast::cpp_type_kind::user_defined_t,
            tinyrefl::tool::detail::assert_handler{});

        const auto& type_ref =
            static_cast<const cppast::cpp_user_defined_type&>(type).entity();
        DEBUG_ASSERT(
            !type_ref.is_overloaded(),
            tinyrefl::tool::detail::assert_handler{});

        const auto entities = type_ref.get(index);
        DEBUG_ASSERT(
            entities.size() == 1, tinyrefl::tool::detail::assert_handler{});
        DEBUG_ASSERT(
            entities[0].get().kind() == cppast::cpp_class::kind(),
            tinyrefl::tool::detail::assert_handler{});

        result.emplace_back(
            index,
            static_cast<const cppast::cpp_class&>(entities[0].get()),
            base.access_specifier());
    }

    return result;
}

template<typename Entity>
entity_refs<Entity> members(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_)
{
    entity_refs<Entity> result;

    cppast::visit(
        class_,
        [&result, &class_, &index](
            const cppast::cpp_entity&   entity,
            const cppast::visitor_info& info) {
            if(entity.kind() == Entity::kind() && entity.parent() == class_ &&
               has_complete_name(entity))
            {
                result.emplace_back(
                    index, static_cast<const Entity&>(entity), info.access);
            }
        });

    return result;
}

entity_refs<cppast::cpp_member_variable> member_variables(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_)
{
    return members<cppast::cpp_member_variable>(index, class_);
}

entity_refs<cppast::cpp_member_function> member_functions(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_)
{
    return members<cppast::cpp_member_function>(index, class_);
}

entity_refs<cppast::cpp_constructor> constructors(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_)
{
    return members<cppast::cpp_constructor>(index, class_);
}

entity_refs<cppast::cpp_class> member_classes(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_)
{
    return members<cppast::cpp_class>(index, class_);
}

entity_refs<cppast::cpp_enum> member_enums(
    const cppast::cpp_entity_index& index, const cppast::cpp_class& class_)
{
    return members<cppast::cpp_enum>(index, class_);
}

std::string type_string(const cppast::cpp_member_function& function)
{
    return fmt::format(
        "{}({}::*){}",
        cppast::to_string(function.return_type()),
        full_qualified_name(function.parent().value()),
        function.signature());
}

std::string type_string(const cppast::cpp_member_variable& variable)
{
    return fmt::format(
        "{} {}::*",
        cppast::to_string(variable.type()),
        full_qualified_name(variable.parent().value()));
}
}
}
}
