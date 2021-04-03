#include <tinyrefl/tool/backends/legacy/entity_registry.hpp>

using namespace tinyrefl::tool;

void entity_registry::set_callback(entity_registered_callback callback)
{
    _entity_registered_callback = callback;
}

const std::string& entity_registry::operator()(
    const cppast::cpp_entity& entity, std::string serialized_entity)
{
    const auto result =
        _registry.insert({&entity, std::move(serialized_entity)});

    if(result.second && _entity_registered_callback)
    {
        _entity_registered_callback(entity, result.first->second);
    }

    return result.first->second;
}

const std::string&
    entity_registry::operator[](const cppast::cpp_entity& entity) const
{
    return _registry.at(&entity);
}

bool entity_registry::already_registered(const cppast::cpp_entity& entity) const
{
    return _registry.count(&entity) > 0;
}

entity_registry::iterator entity_registry::begin() const
{
    return _registry.begin();
}

entity_registry::iterator entity_registry::end() const
{
    return _registry.end();
}
