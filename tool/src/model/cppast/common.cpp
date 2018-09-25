#include <cppast/cpp_entity_kind.hpp>
#include <tinyrefl/tool/model/cppast/common.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

bool has_complete_name(const cppast::cpp_entity& entity)
{
    if(!entity.parent().has_value() ||
       entity.parent().value().kind() == cppast::cpp_entity_kind::file_t)
    {
        return !entity.name().empty();
    }
    else
    {
        return !entity.name().empty() &&
               has_complete_name(entity.parent().value());
    }
}

std::string full_qualified_name(const cppast::cpp_entity& entity)
{
    if(!entity.parent().has_value() ||
       entity.parent().value().kind() == cppast::cpp_entity_kind::file_t)
    {
        return entity.name();
    }
    else
    {
        return full_qualified_name(entity.parent().value()) +
               "::" + entity.name();
    }
}

std::string full_decorated_name(const cppast::cpp_entity& entity)
{
    switch(entity.kind())
    {
    case cppast::cpp_entity_kind::member_function_t:
    case cppast::cpp_entity_kind::constructor_t:
        return full_qualified_name(entity) +
               static_cast<const cppast::cpp_function_base&>(entity)
                   .signature();
    default:
        return full_qualified_name(entity);
    }
}

bool equal_entities(
    const cppast::cpp_entity& lhs, const cppast::cpp_entity& rhs)
{
    return lhs.kind() == rhs.kind() &&
           full_decorated_name(lhs) == full_decorated_name(rhs);
}

bool is_direct_child_of(
    const cppast::cpp_entity& entity, const cppast::cpp_entity& parent)
{
    return entity.parent().has_value() &&
           equal_entities(entity.parent().value(), parent);
}

} // namespace model
} // namespace tool
} // namespace tinyrefl
