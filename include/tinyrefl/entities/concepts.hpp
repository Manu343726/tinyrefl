#ifndef TINYREFL_ENTITIES_CONCEPTS_HPP
#define TINYREFL_ENTITIES_CONCEPTS_HPP

#include <tinyrefl/utils/language_features.hpp>

#ifdef TINYREFL_HAS_CONCEPTS

#include <tinyrefl/entities/entity_kind.hpp>

namespace tinyrefl
{

namespace entities
{

template<typename T>
concept bool Entity = tinyrefl::entities::is_entity<T>::value;

template<typename T, tinyrefl::entities::entity_kind Kind>
concept bool Kind = Entity<T> && (T{}.kind() == Kind);

#define TINYREFL_KIND_CONCEPT(Concept, kind)                                   \
    template<typename T>                                                       \
    concept bool Concept =                                                     \
        tinyrefl::entities::Kind<T, tinyrefl::entities::entity_kind::kind>

TINYREFL_KIND_CONCEPT(File, FILE);
TINYREFL_KIND_CONCEPT(Namespace, NAMESPACE);
TINYREFL_KIND_CONCEPT(Function, FUNCTION);
TINYREFL_KIND_CONCEPT(Variable, VARIABLE);
TINYREFL_KIND_CONCEPT(Class, CLASS);
TINYREFL_KIND_CONCEPT(Constructor, CONSTRUCTOR);
TINYREFL_KIND_CONCEPT(MemberFunction, MEMBER_FUNCTION);
TINYREFL_KIND_CONCEPT(MemberVariable, MEMBER_VARIABLE);
TINYREFL_KIND_CONCEPT(StaticMemberFunction, STATIC_MEMBER_FUNCTION);
TINYREFL_KIND_CONCEPT(StaticMemberVariable, STATIC_MEMBER_VARIABLE);
TINYREFL_KIND_CONCEPT(Enum, ENUM);
TINYREFL_KIND_CONCEPT(EnumValue, ENUM_VALUE);

#undef TINYREFL_KIND_CONCEPT
} // namespace entities
} // namespace tinyrefl

#endif // TINYREFL_HAS_CONCEPTS

#endif // TINYREFL_ENTITIES_CONCEPTS_HPP
