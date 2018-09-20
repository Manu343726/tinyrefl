#ifndef TINYREFL_TOOL_JINJA2REFLECTION_HPP
#define TINYREFL_TOOL_JINJA2REFLECTION_HPP

#include <jinja2cpp/reflected_value.h>
#include <tinyrefl/api.hpp>
#include <type_traits>

namespace jinja2
{
template<typename T>
struct TypeReflection<
    T,
    std::enable_if_t<tinyrefl::has_metadata<T>() && std::is_class<T>::value>>
    : public TypeReflected<T>
{
    using AccessorsMap = std::unordered_map<std::string, FieldAccessor<T>>;

    static AccessorsMap& GetAccessors()
    {
        static AccessorsMap map = generateMap();
        return map;
    }

private:
    static AccessorsMap generateMap()
    {
        AccessorsMap map;

        tinyrefl::visit_class_member_variables<T>(
            [&map](const std::string& name, const auto& member) {
                map[name] = [member](const T& object) {
                    return jinja2::Reflect(member.get(object));
                };
            });

        return map;
    }
};
}

#endif // TINYREFL_TOOL_JINJA2REFLECTION_HPP
