#include <fmt/format.h>
#include <tinyrefl/tool/attribute_registry.hpp>

using namespace tinyrefl::tool;

attribute_registry::attribute::attribute(const cppast::cpp_attribute& attribute)
    : _attribute{&attribute}
{
}

std::string attribute_registry::attribute::arguments() const
{
    const auto args = _attribute->arguments();

    if(args.has_value())
    {
        return args.value().as_string();
    }
    else
    {
        return "";
    }
}

tinyrefl::hash_t attribute_registry::attribute::hash() const
{
    return tinyrefl::hash(full_attribute().c_str());
}

std::string attribute_registry::attribute::name() const
{
    return _attribute->name();
}

std::string attribute_registry::attribute::scope() const
{
    // Standard attributes (those wihtout scope, like [[nodiscard]]) have
    // its classes declared in the std::attributes namespace.
    // Compiler extensions (Like [[gnu::aligned(...)]]) will have
    // their namespace ('gnu') as scope.
    return _attribute->scope().value_or("std::attributes");
}

std::string attribute_registry::attribute::full_name() const
{
    return fmt::format("{}::{}", scope(), name());
}

std::string attribute_registry::attribute::type() const
{
    return full_name();
}

std::string attribute_registry::attribute::full_attribute() const
{
    const auto args = arguments();

    if(args.empty())
    {
        return full_name();
    }
    else
    {
        return fmt::format("{}({})", full_name(), args);
    }
}

const attribute_registry::attribute&
    attribute_registry::save(const cppast::cpp_attribute& attribute)
{
    const auto result = _registry.insert({&attribute, {attribute}});

    return result.first->second;
}

const attribute_registry::attribute&
    attribute_registry::operator[](const cppast::cpp_attribute& attribute) const
{
    return _registry.at(&attribute);
}

attribute_registry::iterator attribute_registry::begin() const
{
    return _registry.begin();
}

attribute_registry::iterator attribute_registry::end() const
{
    return _registry.end();
}
