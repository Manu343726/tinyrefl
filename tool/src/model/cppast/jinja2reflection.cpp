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

namespace jinja2
{

std::unordered_map<
    std::string,
    FieldAccessor<entity_ref<cppast::cpp_member_function>>>
    TypeReflection<entity_ref<cppast::cpp_member_function>>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> map{
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
}
