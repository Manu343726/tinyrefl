#include <tinyrefl/tool/model/cppast/attribute.hpp>
#include <tinyrefl/tool/model/cppast/class.hpp>
#include <tinyrefl/tool/model/cppast/enum.hpp>
#include <tinyrefl/tool/model/cppast/file.hpp>
#include <tinyrefl/tool/model/cppast/jinja2reflection.hpp>

using namespace jinja2;

using namespace tinyrefl::tool::model;

template<typename Entity>
std::string name(const entity_ref<Entity>& entity)
{
    return entity->name();
}

template<typename Entity>
std::string full_name(const entity_ref<Entity>& entity)
{
    return tinyrefl::tool::model::full_qualified_name(entity.entity());
}

template<typename Entity>
std::string member_pointer(const entity_ref<Entity>& entity)
{
    return "&" + tinyrefl::tool::model::full_qualified_name(entity.entity());
}

template<typename Entity>
std::string type_string(const entity_ref<Entity>& entity)
{
    return tinyrefl::tool::model::type_string(entity.entity());
}

template<typename Entity>
std::string signature(const entity_ref<Entity>& entity)
{
    return entity->signature();
}

template<typename Entity>
jinja2::Value parent_class(const entity_ref<Entity>& entity)
{
    return jinja2::Reflect(entity.template parent_ref<cppast::cpp_class>());
}

jinja2::Value base_classes(const entity_ref<cppast::cpp_class>& class_)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::base_classes(class_.index(), class_.entity()));
}

jinja2::Value member_functions(const entity_ref<cppast::cpp_class>& class_)
{
    return jinja2::Reflect(tinyrefl::tool::model::member_functions(
        class_.index(), class_.entity()));
}

jinja2::Value member_variables(const entity_ref<cppast::cpp_class>& class_)
{
    return jinja2::Reflect(tinyrefl::tool::model::member_variables(
        class_.index(), class_.entity()));
}

jinja2::Value constructors(const entity_ref<cppast::cpp_class>& class_)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::constructors(class_.index(), class_.entity()));
}

jinja2::Value member_classes(const entity_ref<cppast::cpp_class>& class_)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::member_classes(class_.index(), class_.entity()));
}

jinja2::Value member_enums(const entity_ref<cppast::cpp_class>& class_)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::member_enums(class_.index(), class_.entity()));
}

template<typename Entity>
jinja2::Value access(const entity_ref<Entity>& entity)
{
    return cppast::to_string(entity.access());
}

jinja2::Value
    enum_value_name(const entity_ref<cppast::cpp_enum_value>& enum_value)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::enum_value_name(enum_value.entity()));
}

jinja2::Value enum_value_qualified_name(
    const entity_ref<cppast::cpp_enum_value>& enum_value)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::enum_value_qualified_name(enum_value.entity()));
}

jinja2::Value enum_value_full_qualified_name(
    const entity_ref<cppast::cpp_enum_value>& enum_value)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::enum_value_full_qualified_name(
            enum_value.entity()));
}

jinja2::Value
    enum_value_raw_value(const entity_ref<cppast::cpp_enum_value>& enum_value)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::enum_raw_value(enum_value.entity()));
}

jinja2::Value enum_values(const entity_ref<cppast::cpp_enum>& enum_)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::enum_values(enum_.index(), enum_.entity()));
}

jinja2::Value enum_names(const entity_ref<cppast::cpp_enum>& enum_)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::enum_names(enum_.index(), enum_.entity()));
}

jinja2::Value enum_qualified_names(const entity_ref<cppast::cpp_enum>& enum_)
{
    return jinja2::Reflect(tinyrefl::tool::model::enum_qualified_names(
        enum_.index(), enum_.entity()));
}

jinja2::Value
    enum_full_qualified_names(const entity_ref<cppast::cpp_enum>& enum_)
{
    return jinja2::Reflect(tinyrefl::tool::model::enum_full_qualified_names(
        enum_.index(), enum_.entity()));
}

jinja2::Value enum_raw_values(const entity_ref<cppast::cpp_enum>& enum_)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::enum_raw_values(enum_.index(), enum_.entity()));
}

jinja2::Value enum_count(const entity_ref<cppast::cpp_enum>& enum_)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::enum_count(enum_.index(), enum_.entity()));
}

jinja2::Value all_classes(const entity_ref<cppast::cpp_file>& file)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::all_classes(file.index(), file.entity()));
}

jinja2::Value all_enums(const entity_ref<cppast::cpp_file>& file)
{
    return jinja2::Reflect(
        tinyrefl::tool::model::all_enums(file.index(), file.entity()));
}

jinja2::Value
    all_namespace_level_classes(const entity_ref<cppast::cpp_file>& file)
{
    return jinja2::Reflect(tinyrefl::tool::model::all_namespace_level_classes(
        file.index(), file.entity()));
}

jinja2::Value
    all_namespace_level_enums(const entity_ref<cppast::cpp_file>& file)
{
    return jinja2::Reflect(tinyrefl::tool::model::all_namespace_level_enums(
        file.index(), file.entity()));
}

template<typename Entity>
jinja2::Value entity_attributes(const entity_ref<Entity>& entity)
{
    return jinja2::Reflect(tinyrefl::tool::model::entity_attributes(
        entity.index(), entity.entity()));
}

jinja2::Value full_attribute(const entity_ref<cppast::cpp_attribute>& attribute)
{
    return tinyrefl::tool::model::full_attribute(attribute.entity());
}

jinja2::Value attribute_name(const entity_ref<cppast::cpp_attribute>& attribute)
{
    return tinyrefl::tool::model::attribute_name(attribute.entity());
}

jinja2::Value tokenized_attribute_arguments(
    const entity_ref<cppast::cpp_attribute>& attribute)
{
    return jinja2::Reflect(tinyrefl::tool::model::tokenized_attribute_arguments(
        attribute.index(), attribute.entity()));
}

jinja2::Value attribute_joined_arguments(
    const entity_ref<cppast::cpp_attribute>& attribute)
{
    return tinyrefl::tool::model::attribute_joined_arguments(
        attribute.entity());
}

jinja2::Value
    attribute_has_arguments(const entity_ref<cppast::cpp_attribute>& attribute)
{
    return tinyrefl::tool::model::attribute_has_arguments(attribute.entity());
}

jinja2::Value token_spelling(const entity_ref<cppast::cpp_token>& token)
{
    return tinyrefl::tool::model::token_spelling(token.entity());
}

jinja2::Value token_kind(const entity_ref<cppast::cpp_token>& token)
{
    return tinyrefl::tool::model::token_kind_string(token.entity());
}

template<typename Entity>
jinja2::Value entity_comment(const entity_ref<Entity>& entity)
{
    return tinyrefl::tool::model::entity_comment(entity.entity());
}

template<typename Entity>
jinja2::Value entity_has_comment(const entity_ref<Entity>& entity)
{
    return tinyrefl::tool::model::entity_has_comment(entity.entity());
}

namespace jinja2
{

std::unordered_map<
    std::string,
    FieldAccessor<entity_ref<cppast::cpp_member_function>>>
    TypeReflection<entity_ref<cppast::cpp_member_function>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
        {"comment", entity_comment<cppast::cpp_member_function>},
        {"has_comment", entity_has_comment<cppast::cpp_member_function>},
        {"attributes", entity_attributes<cppast::cpp_member_function>},
        {"access", access<cppast::cpp_member_function>},
        {"name", name<cppast::cpp_member_function>},
        {"fullname", full_name<cppast::cpp_member_function>},
        {"pointer", member_pointer<cppast::cpp_member_function>},
        {"type", type_string<cppast::cpp_member_function>},
        {"class", parent_class<cppast::cpp_member_function>},
        {"signature", signature<cppast::cpp_member_function>}};

    return map;
}


std::unordered_map<
    std::string,
    FieldAccessor<entity_ref<cppast::cpp_member_variable>>>
    TypeReflection<entity_ref<cppast::cpp_member_variable>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
        {"comment", entity_comment<cppast::cpp_member_variable>},
        {"has_comment", entity_has_comment<cppast::cpp_member_variable>},
        {"attributes", entity_attributes<cppast::cpp_member_variable>},
        {"access", access<cppast::cpp_member_variable>},
        {"name", name<cppast::cpp_member_variable>},
        {"fullname", full_name<cppast::cpp_member_variable>},
        {"pointer", member_pointer<cppast::cpp_member_variable>},
        {"class", parent_class<cppast::cpp_member_variable>},
        {"type", type_string<cppast::cpp_member_variable>}};

    return map;
}

std::unordered_map<
    std::string,
    FieldAccessor<entity_ref<cppast::cpp_constructor>>>
    TypeReflection<entity_ref<cppast::cpp_constructor>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
        {"comment", entity_comment<cppast::cpp_constructor>},
        {"has_comment", entity_has_comment<cppast::cpp_constructor>},
        {"attributes", entity_attributes<cppast::cpp_constructor>},
        {"access", access<cppast::cpp_constructor>},
        {"name", name<cppast::cpp_constructor>},
        {"fullname", full_name<cppast::cpp_constructor>},
        {"class", parent_class<cppast::cpp_constructor>},
        {"signature", signature<cppast::cpp_constructor>}};

    return map;
}

std::unordered_map<std::string, FieldAccessor<entity_ref<cppast::cpp_class>>>
    TypeReflection<entity_ref<cppast::cpp_class>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
        {"comment", entity_comment<cppast::cpp_class>},
        {"has_comment", entity_has_comment<cppast::cpp_class>},
        {"attributes", entity_attributes<cppast::cpp_class>},
        {"access", access<cppast::cpp_class>},
        {"name", name<cppast::cpp_class>},
        {"fullname", full_name<cppast::cpp_class>},
        {"bases", ::base_classes},
        {"variables", ::member_variables},
        {"functions", ::member_functions},
        {"constructors", ::constructors},
        {"classes", ::member_classes},
        {"enums", ::member_enums}};

    return map;
}

std::unordered_map<
    std::string,
    FieldAccessor<entity_ref<cppast::cpp_enum_value>>>
    TypeReflection<entity_ref<cppast::cpp_enum_value>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
        {"comment", entity_comment<cppast::cpp_enum_value>},
        {"has_comment", entity_has_comment<cppast::cpp_enum_value>},
        {"attributes", entity_attributes<cppast::cpp_enum_value>},
        {"name", name<cppast::cpp_enum_value>},
        {"fullname", full_name<cppast::cpp_enum_value>},
        {"qualifiedname", ::enum_value_qualified_name},
        {"value", ::enum_raw_value}};

    return map;
}

std::unordered_map<std::string, FieldAccessor<entity_ref<cppast::cpp_enum>>>
    TypeReflection<entity_ref<cppast::cpp_enum>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
        {"comment", entity_comment<cppast::cpp_enum>},
        {"has_comment", entity_has_comment<cppast::cpp_enum>},
        {"attributes", entity_attributes<cppast::cpp_enum>},
        {"access", access<cppast::cpp_enum>},
        {"name", name<cppast::cpp_enum>},
        {"fullname", full_name<cppast::cpp_enum>},
        {"values", ::enum_values},
        {"raw_values", ::enum_raw_values},
        {"names", ::enum_names},
        {"qualifiednames", ::enum_names},
        {"fullnames", ::enum_names},
        {"count", ::enum_count}};

    return map;
}

std::unordered_map<std::string, FieldAccessor<entity_ref<cppast::cpp_file>>>
    TypeReflection<entity_ref<cppast::cpp_file>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
        {"classes", ::all_classes},
        {"enums", ::all_enums},
        {"namespace_level_classes", ::all_namespace_level_classes},
        {"namespace_level_enums", ::all_namespace_level_enums}};

    return map;
}

std::unordered_map<std::string, FieldAccessor<entity_ref<cppast::cpp_token>>>
    TypeReflection<entity_ref<cppast::cpp_token>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
        {"kind", ::token_kind}, {"spelling", ::token_spelling}};

    return map;
}

std::
    unordered_map<std::string, FieldAccessor<entity_ref<cppast::cpp_attribute>>>
    TypeReflection<entity_ref<cppast::cpp_attribute>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
        {"full", ::full_attribute},
        {"name", ::attribute_name},
        {"tokenized_arguments", ::tokenized_attribute_arguments},
        {"has_arguments", ::attribute_has_arguments},
        {"arguments", ::attribute_joined_arguments}};

    return map;
}

} // namespace jinja2
