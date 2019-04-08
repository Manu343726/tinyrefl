#ifndef TINYREFL_ENTITIES_CLASS_HPP_INCLUDED
#define TINYREFL_ENTITIES_CLASS_HPP_INCLUDED

#include <tinyrefl/backend/metadata.hpp>
#include <tinyrefl/entities/entity.hpp>
#include <tinyrefl/utils/meta.hpp>

namespace tinyrefl
{

namespace entities
{

template<
    typename Name,
    typename FullName,
    typename Parent,
    typename SourceLocation,
    typename Class,
    typename BaseClasses,
    typename Constructors,
    typename MemberFunctions,
    typename MemberVariables,
    typename StaticMemberFunctions,
    typename StaticMemberVariables,
    typename Classes,
    typename Enums,
    typename Attributes>
struct class_ : public tinyrefl::entities::entity_with_attributes<
                    Attributes,
                    tinyrefl::entities::entity_kind::CLASS,
                    Name,
                    FullName,
                    Parent,
                    tinyrefl::meta::pack_foldl_t<
                        tinyrefl::meta::defer<tinyrefl::meta::cat>,
                        Constructors,
                        MemberFunctions,
                        MemberVariables,
                        StaticMemberFunctions,
                        StaticMemberVariables,
                        Classes,
                        Enums>,
                    SourceLocation>
{
    using class_type              = Class;
    using base_classes            = BaseClasses;
    using constructors            = Constructors;
    using member_functions        = MemberFunctions;
    using member_variables        = MemberVariables;
    using static_member_functions = StaticMemberFunctions;
    using static_member_variables = StaticMemberVariables;
    using member_classes          = tinyrefl::meta::fmap_t<
        tinyrefl::meta::defer<tinyrefl::backend::entity_metadata>,
        Classes>;
    using member_enums = tinyrefl::meta::fmap_t<
        tinyrefl::meta::defer<tinyrefl::backend::entity_metadata>,
        Enums>;
};
} // namespace entities
} // namespace tinyrefl
#endif // TINYREFL_ENTITIES_CLASS_HPP_INCLUDED
