#include <cppast/visitor.hpp>
#include <tinyrefl/tool/detail/assert.hpp>
#include <tinyrefl/tool/detail/logger.hpp>
#include <tinyrefl/tool/model/cppast/enum.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{
entity_refs<cppast::cpp_enum_value> enum_values(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_)
{
    return direct_child_entities<cppast::cpp_enum_value>(index, enum_);
}

std::int64_t enum_raw_value(const cppast::cpp_enum_value& value)
{
    return 0;
}

std::string enum_value_name(const cppast::cpp_enum_value& value)
{
    return value.name();
}

std::string enum_value_qualified_name(const cppast::cpp_enum_value& value)
{
    DEBUG_ASSERT(
        value.parent().has_value() &&
            value.parent().value().kind() == cppast::cpp_enum::kind(),
        tinyrefl::tool::detail::assert_handler{});
    return value.parent().value().name() + "::" + value.name();
}

std::string enum_value_full_qualified_name(const cppast::cpp_enum_value& value)
{
    DEBUG_ASSERT(
        value.parent().has_value() &&
            value.parent().value().kind() == cppast::cpp_enum::kind(),
        tinyrefl::tool::detail::assert_handler{});
    return full_qualified_name(value);
}

template<typename T>
std::vector<T> enum_values_property(
    const cppast::cpp_entity_index& index,
    const cppast::cpp_enum&         enum_,
    T (*get_property)(const cppast::cpp_enum_value&))
{
    std::vector<T> result;
    const auto     values = enum_values(index, enum_);

    std::transform(
        values.begin(),
        values.end(),
        std::back_inserter(result),
        [get_property](const entity_ref<cppast::cpp_enum_value>& value) {
            return get_property(value);
        });

    return result;
}

std::vector<std::string> enum_names(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_)
{
    return enum_values_property(index, enum_, enum_value_name);
}

std::vector<std::string> enum_qualified_names(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_)
{
    return enum_values_property(index, enum_, enum_value_qualified_name);
}

std::vector<std::string> enum_full_qualified_names(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_)
{
    return enum_values_property(index, enum_, enum_value_full_qualified_name);
}

std::vector<std::int64_t> enum_raw_values(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_)
{
    return enum_values_property(index, enum_, enum_raw_value);
}

std::size_t enum_count(
    const cppast::cpp_entity_index& index, const cppast::cpp_enum& enum_)
{
    return enum_values(index, enum_).size();
}
}
}
}
