#ifndef TINYREFL_TOOL_ENTITY_REGISTRY_HPP_INCLUDED
#define TINYREFL_TOOL_ENTITY_REGISTRY_HPP_INCLUDED

#include <functional>
#include <string>
#include <unordered_map>

#include <cppast/cpp_entity.hpp>

#include <tinyrefl/tool/visitor.hpp>

namespace tinyrefl
{

namespace tool
{

class entity_registry
{
public:
    using entity_registered_callback = std::function<void(
        const cppast::cpp_entity&, const std::string& serialized_entity)>;

    void set_callback(entity_registered_callback callback);

    template<typename... Callbacks>
    void set_callback(Callbacks... callbacks)
    {
        set_callback(static_cast<entity_registered_callback>(
            [callbacks = tinyrefl::overloaded_function(callbacks...)](
                const cppast::cpp_entity& entity,
                const std::string&        serialized_entity) {
                tinyrefl::tool::entity_cast(
                    entity,
                    [callbacks, &serialized_entity](const auto& entity) {
                        callbacks(entity, serialized_entity);
                    });
            }));
    }

    const std::string& operator()(
        const cppast::cpp_entity& entity, std::string serialized_entity);

    const std::string& operator[](const cppast::cpp_entity& entity) const;
    bool already_registered(const cppast::cpp_entity& entity) const;

    using iterator = std::
        unordered_map<const cppast::cpp_entity*, std::string>::const_iterator;

    iterator begin() const;
    iterator end() const;

private:
    std::unordered_map<const cppast::cpp_entity*, std::string> _registry;
    entity_registered_callback _entity_registered_callback;
};
} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_ENTITY_REGISTRY_HPP_INCLUDED
