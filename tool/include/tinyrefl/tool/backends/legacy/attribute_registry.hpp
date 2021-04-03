#ifndef TINYREFL_TOOL_ATTRIBUTE_REGISTRY_HPP_INCLUDED
#define TINYREFL_TOOL_ATTRIBUTE_REGISTRY_HPP_INCLUDED

#include <string>
#include <unordered_map>

#include <cppast/cpp_attribute.hpp>
#include <tinyrefl/types/string.hpp>

namespace tinyrefl
{

namespace tool
{

class attribute_registry
{
public:
    struct attribute
    {
        attribute(const cppast::cpp_attribute& attribute);

        tinyrefl::hash_t hash() const;
        std::string      name() const;
        std::string      scope() const;
        std::string      full_name() const;
        std::string      full_attribute() const;
        std::string      type() const;
        std::string      arguments() const;

    private:
        const cppast::cpp_attribute* _attribute;
    };

    const attribute& save(const cppast::cpp_attribute& attribute);
    const attribute& operator[](const cppast::cpp_attribute& attribute) const;

    using iterator = std::
        unordered_map<const cppast::cpp_attribute*, attribute>::const_iterator;

    iterator begin() const;
    iterator end() const;

private:
    std::unordered_map<const cppast::cpp_attribute*, attribute> _registry;
};
} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_ATTRIBUTE_REGISTRY_HPP_INCLUDED
