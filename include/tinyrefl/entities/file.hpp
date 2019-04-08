#ifndef TINYREFL_ENTITIES_FILE_HPP
#define TINYREFL_ENTITIES_FILE_HPP

#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/entities/source_location.hpp>

namespace tinyrefl
{

namespace entities
{

template<typename Name, typename FullName, typename Namespaces>
struct file : public tinyrefl::entities::entity_with_attributes<
                  tinyrefl::entities::no_attributes,
                  tinyrefl::entities::entity_kind::FILE,
                  Name,
                  FullName,
                  tinyrefl::entities::no_parent,
                  tinyrefl::meta::remove_duplicates_t<Namespaces>,
                  tinyrefl::entities::source_location<FullName, 0, 0>>
{
};
} // namespace entities

} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_FILE_HPP
