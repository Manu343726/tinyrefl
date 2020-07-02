#ifndef TINYREFL_ENTITIES_ENTITY_HPP_INCLUDED
#define TINYREFL_ENTITIES_ENTITY_HPP_INCLUDED

#include <tinyrefl/backend/metadata.hpp>
#include <tinyrefl/entities/attribute.hpp>
#include <tinyrefl/entities/entity_kind.hpp>
#include <tinyrefl/types/name.hpp>
#include <tinyrefl/types/string.hpp>
#include <tinyrefl/types/type.hpp>
#include <tinyrefl/utils/meta.hpp>

#include <type_traits>

/// The main tinyrefl API namespace
namespace tinyrefl
{

/// This namespace contains classes representing reflection metadata
namespace entities
{

namespace impl
{

template<typename Name, typename DisplayName>
constexpr tinyrefl::string display_name_resolver(
    tinyrefl::type_tag<Name>, tinyrefl::type_tag<DisplayName>)
{
    return tinyrefl::string_constant<DisplayName>();
}

template<typename Name>
constexpr tinyrefl::string
    display_name_resolver(tinyrefl::type_tag<Name>, tinyrefl::type_tag<void>)
{
    return tinyrefl::string_constant<Name>();
}

template<typename FullName, typename FullDisplayName>
constexpr tinyrefl::string full_display_name_resolver(
    tinyrefl::type_tag<FullName>, tinyrefl::type_tag<FullDisplayName>)
{
    return tinyrefl::string_constant<FullDisplayName>();
}

template<typename FullName>
constexpr tinyrefl::string full_display_name_resolver(
    tinyrefl::type_tag<FullName>, tinyrefl::type_tag<void>)
{
    return tinyrefl::string_constant<FullName>();
}

template<typename FullName, typename FullDisplayName>
constexpr tinyrefl::hash_t entity_id_resolver(
    tinyrefl::type_tag<FullName>        full_name,
    tinyrefl::type_tag<FullDisplayName> full_display_name)
{
    return full_display_name_resolver(full_name, full_display_name).hash();
}

template<tinyrefl::entities::entity_kind Kind>
struct EntityFilter
{
    constexpr EntityFilter() = default;

    template<typename Entity>
    constexpr auto operator()(const Entity& entity) const
    {
        return entity.kind() == Kind;
    }
};

} // namespace impl

/**
 * @class entity
 * @brief Base class of all reflection metadata entity classes. Provides
 * reflection information common to all kinds of entities.
 *
 * @tparam Kind entity kind
 * @tparam Name `tinyrefl::hash_constant` of the non qualified name
 * @tparam FullName `tinyrefl::hash_constant` of the fullm qualified name
 * @tparam Parent Handle to the parent entity
 * @tparam Children `tinyrefl::meta::list` with the children entity classes
 * @tparam SourceLocation `tinyrefl::entities::source_location<...>` with the
 *         source location of the declaration of the entity
 * @tparam DisplayName `tinyrefl::hash_constant` of the display name
 * @tparam FullDisplayName `tinyrefl::has_constant` of the full display
 * name
 *
 * This class is the common base class for all reflection metadata
 * entities. An entity class represents a C++ entity (A source file, a
 * namespace, a member function, etc) and returns information such as its full
 * qualified name, the kind of entity (A function, a namespace, a class, etc) or
 * its source location.
 *
 * > In dynamically typed languages like Python language entities, such as
 * > types, are part of the language object system (A Python class is an
 * > object); and users can manipulate that entities as if they where normal
 * > entities of their program. There's no difference between the class in the
 * > context of the type system and the class object the user manipulates for
 * > introspection purposes. C++ has no builtin support for reflection, so
 * > there's a difference between **the reflected entity** (The namespace in
 * > your C++ sources) and **the class that represents the reflected entity
 * > within the reflection system** (`tinyrefl::entities::entity` subclasses in
 * > tinyrefl).
 * >
 * > When talking about "entity children", "entity name", and other
 * > properties of a reflected entity this documentation assumes the user
 * > understands the distinction between the reflected entities and the entity
 * > classes, and may name the entity metadata classes and their instances as if
 * > they were the reflected entities themselves.
 *
 * Entity classes expose reflection information as public member functions
 * representing the different properties of the reflected entity. Some
 * properties, such as those exposed by the base `tinyrefl::entities::entity`
 * class, are common to all reflected entities, and others are specific of the
 * entity kind:
 *
 * ``` cpp
 * constexpr auto enum_ = tinyrefl::metadata_by_id("lib::Enum"_id);
 *
 * std::cout << enum_.name() << "\n"; // prints "Enum"
 *
 * // Error: class tinyrefl::entities::enum_<...> has no member named
 * //        "arguments"
 * for(const auto& arg : enum_.arguments())
 * {
 *    ...
 * }
 * ```
 *
 * All entity classes are default-constexpr-constructible, and the constructor
 * always returns instances with the same entity metadata. This allows to
 * arbitrarily switch between type and value domains depending on the way the
 * reflection metadata is being manipulated:
 *
 * ``` cpp
 *  using entities = typelist<NamespaceEntity, ClassEntity, EnumEntity>;
 *
 *  // Equivalent, same metadata:
 *  constexpr std::tuple<NamespaceEntity, ClassEntity, EnumEntity> entities{};
 * ```
 *
 * it also helps to bring metadata back to a constexpr context:
 *
 * ``` cpp
 * tinyrefl::visit<Class>(tinyrefl::member_function_visitor([](const auto&
 * member_function))
 * {
 *      // member_function is not constexpr in this context, so neither name is:
 *      const auto name = member_function.name();
 *
 *      // This is constexpr and has the same data:
 *      constexpr auto constexpr_name = decltype(member_function){}.name();
 * }));
 * ```
 *
 * All entities are guaranteed to have a unique full display name, which can be
 * used to get the entity metadata through `tinyrefl::metadata_by_id()`:
 *
 * ``` cpp
 * using namespace tinyrefl::literals;
 *
 * constexpr auto class_ = tinyrefl::metadata_by_id("example::Class"_id);
 * ```
 *
 * Entity classes also have an `id()` property that returns the unique id of the
 * entity. Querying the reflection database with that id should return the same
 * entity:
 *
 * ``` cpp
 * static_assert(
 *    tinyrefl::metadata_by_id(entity.id()) == entity);
 * ```
 */
#ifdef DOXYGEN_PROCESSING_TINYREFL
template<
    tinyrefl::entities::entity_kind Kind,
    typename Name,
    typename FullName,
    typename Parent,
    typename Children,
    typename SourceLocation,
    typename DisplayName,
    typename FullDisplayName>
#else
template<
    tinyrefl::entities::entity_kind Kind,
    typename Name,
    typename FullName,
    typename Parent_,
    typename Children_,
    typename SourceLocation_,
    typename DisplayName     = void,
    typename FullDisplayName = void>
#endif // DOXYGEN_PROCESSING_TINYREFL
struct entity
{
    struct tinyrefl_entity_tag
    {
    };

#ifdef DOXYGEN_PROCESSING_TINYREFL
    /// @brief Alias to the metadata class of the entity class representing the
    /// parent of this entity.
    ///
    /// If the entity has no parent (it's at the uppermost level
    /// of the entity hierarchy, like @ref `tinyrefl::entities::file`), it
    /// returns @ref `tinyrefl::entities::no_metadata`.
    using Parent = tinyrefl::entities::entity<...>;
#else
    using Parent         = tinyrefl::metadata<Parent_>;
#endif // DOXYGEN_PROCESSING_TINYREFL

#ifdef DOXYGEN_PROCESSING_TINYREFL
    /// @brief Alias to the type list of entity classes representing the
    /// children entities of this entity.
    ///
    /// If the entity has no children, such as a
    /// function entity, returns an empty type list.
    using Children = tinyrefl::meta::list<...>;
#else
    using Children       = Children_;
#endif // DOXYGEN_PROCESSING_TINYREFL

#ifdef DOXYGEN_PROCESSING_TINYREFL
    /// @brief Alias to the type representing the source location of the entity.
    using SourceLocation = tinyrefl::entities::source_location<...>;
#else
    using SourceLocation = SourceLocation_;
#endif // DOXYGEN_PROCESSING_TINYREFL

#ifdef DOXYGEN_PROCESSING_TINYREFL
    /// @returns `true` if the two entity class instances reflect the
    /// same entity, returns `false` otherwise.
    constexpr bool operator==(const entity& lhs, const entity& rhs);

    /// @returns `true` if the two entity class instances do not reflect the
    /// same entity, returns `false` otherwise.
    constexpr bool operator!=(const entity& lhs, const entity& rhs);
#else
    template<
        tinyrefl::entities::entity_kind RhsKind,
        typename RhsName,
        typename RhsFullName,
        typename RhsParent,
        typename RhsChildren,
        typename RhsSourceLocation,
        typename RhsDisplayName,
        typename RhsFullDisplayName>
    constexpr bool operator==(const entity<
                              RhsKind,
                              RhsName,
                              RhsFullName,
                              RhsParent,
                              RhsChildren,
                              RhsSourceLocation,
                              RhsDisplayName,
                              RhsFullDisplayName>& other) const
    {
        return std::
            is_same<entity, typename std::decay<decltype(other)>::type>::value;
    }

    template<
        tinyrefl::entities::entity_kind RhsKind,
        typename RhsName,
        typename RhsFullName,
        typename RhsParent,
        typename RhsChildren,
        typename RhsSourceLocation,
        typename RhsDisplayName,
        typename RhsFullDisplayName>
    constexpr bool operator!=(const entity<
                              RhsKind,
                              RhsName,
                              RhsFullName,
                              RhsParent,
                              RhsChildren,
                              RhsSourceLocation,
                              RhsDisplayName,
                              RhsFullDisplayName>& other) const
    {
        return !(*this == other);
    }
#endif // DOXYGEN_PROCESSING_TINYREFL

    /**
     * @brief Returns the parent of this entity.
     * @returns The instance of the parent entity class if the entity
     * has parent, else returns an instance of @ref
     * `tinyrefl::entities::no_metadata`.
     */
    constexpr Parent parent() const
    {
        return {};
    }

    /**
     * @brief Checks whether the entity has parent. See @ref parent() for
     * details.
     */
    constexpr bool has_parent() const
    {
        return std::is_same<Parent, tinyrefl::backend::no_metadata>::value;
    }

    /**
     * @brief Returns how deep the entity is in the entity tree.
     * @returns 0 if the entity has no metadata (See
     * `tinyrefl::backend::no_metadata`), 1 if the entity is a root entity (A
     * file), else it returns the distance between the entity and its root file.
     */
    constexpr std::size_t depth() const
    {
        return parent().depth() + 1;
    }

#ifndef DOXYGEN_PROCESSING_TINYREFL
private:
    struct index_lambda
    {
        const entity* self;
        std::size_t&  index;
        std::size_t   i;

        constexpr index_lambda(const entity* self, std::size_t& index)
            : self{self}, index(index), i{0}
        {
        }

        template<typename Entity>
        constexpr void operator()(const Entity& sibling)
        {
            if(*self == sibling)
            {
                index = i;
            }

            if(self->kind() == sibling.kind())
            {
                ++i;
            }
        }
    };

public:
#endif // DOXYGEN_PROCESSING_TINYREFL

    /**
     * @brief Returns the index among the set of siblings of the same kind
     *
     * This function provides a way to index entities of the same kind coming
     * from the same parent, such as the functions in a namespace or the
     * constructors of a class:
     *
     * ``` cpp
     * namespace module1
     * {
     *     void init();
     *     void deinit();
     * }
     *
     *
     * constexpr auto init = tinyrefl::metadata<"module1::init()"_id>();
     * constexpr auto deinit = tinyrefl::metadata<"module1::deinit()"_id>();
     *
     * const std::unordered_map<std::size_t, std::string_view> module1Functions{
     *     {init.index(), init.name()},
     *     {deinit.index(), deinit.name()}
     * };
     *
     * assert(module1Functions[0] == "init");
     * assert(module1Functions[1] == "deinit");
     * ```
     *
     * @remarks If the entity has no parent (i.e. is a file) the function
     * returns 0. The order is not guaranteed to be the declaration order, **but
     * it is guaranteed to follow the visitation order** (See
     * `tinyrefl::visit()`).
     */
    constexpr std::size_t index() const
    {
        std::size_t index = 0;

        tinyrefl::meta::foreach(parent().children(), index_lambda{this, index});

        return index;
    }

    /**
     * @brief Returns the set of member entities of the entity.
     * @returns `std::tuple<>` containing the children entity class instances.
     * If the entity has no children returns an empty `std::tuple<>`.
     *
     * This method can be used to traverse the members of an entity,
     * regardless of its kind. The same looping code works for namespace
     * members, class members, enum values, etc:
     * ``` cpp
     * constexpr auto class_ = tinyrefl::metadata<lib::Class>();
     *
     * // Print all members (Classes, enums, functions, constructors) of the
     * // class:
     * tinyrefl::meta::foreach(class_.children(), [&](const auto& child)
     * {
     *     std::cout << child.kind() << " " << child.name() << "\n";
     * });
     * ```
     * @par See `tinyrefl::visit()` and `tinyrefl::recursive_visit()` for
     * advanced child visitation.
     */
    constexpr auto children() const
    {
        return tinyrefl::meta::make_tuple(Children{});
    }

    /**
     * @brief Returns the subset of children entities of the given kind
     * @tparam Kind The kind of entity the children must be
     * @returns `std::tuple<>` containing the children entity class instances.
     * If the entity has no children of the given kind returns an empty
     * `std::tuple<>`.
     */
    template<tinyrefl::entities::entity_kind Kind_>
    constexpr auto children() const
    {
        return tinyrefl::meta::tuple_filter(
            children(), impl::EntityFilter<Kind_>{});
    }

    /**
     * @brief Checks whether the entity has children. See @ref children() for
     * details.
     */
    constexpr bool has_children() const
    {
        return Children::size > 0;
    }

    /**
     * @brief Returns the kind of entity
     *
     * The entity kind identifies the type of C++ entity (A class, a
     * member function, a namespace, etc) the entity class is reflecting. This
     * kind is represented as a value of the `tinyrefl::entities::entity_kind`
     * enumeration, which has one value for each C++ entity reflected by
     * tinyrefl.
     *
     * @remarks In case the entity class reflects an unknown entity, like when
     * reflecting a type with no reflection information, the kind is
     * `tinyrefl::entities::entity_kind::UNKNOWN`.
     */
    constexpr tinyrefl::entities::entity_kind kind() const
    {
        return Kind;
    }

    /**
     * @brief Returns the non qualified name of the entity
     */
    constexpr tinyrefl::string name() const
    {
        return tinyrefl::string_constant<Name>();
    }


    /**
     * @brief Returns the full qualified name of the entity
     */
    constexpr tinyrefl::string full_name() const
    {
        return tinyrefl::string_constant<FullName>();
    }

    /**
     * @brief Returns the display name of the entity
     *
     * The display name of the entity is a name that not only includes
     * the identifier of the entity but also extra information that helps to
     * uniquely identify the entity. For invokable entities that can be
     * overloaded, like functions, member functions, and constructors; the
     * display name includes the full signature of the invokable. For non-static
     * member functions it also includes any const and reference quelifier:
     * ``` cpp
     *
     * namespace example
     * {
     *     class Class
     *     {
     *     public:
     *         int f();
     *         int f(int a);
     *         int f() const;
     *     };
     * }
     *
     * tinyrefl::visit<example::Class>(tinyrefl::member_function_visitor(
     *     [&](const auto& function)
     *     {
     *         std::cout << function.name(); // prints "f"
     *         std::cout << function.display_name(); // prints "f()"
     *                                               // prints "f(int)"
     *                                               // prints "f() const"
     *     }));
     * ```
     * For namespaces, which can be reflected multiple times from different
     * source files, the display name includes the name of the source file
     * in which the namespace was declared:
     * ``` cpp
     * constexpr auto class_ = tinyrefl::metadata<exmaple::Class>();
     *
     * // prints example.hpp::example
     * std::cout << class_.parent().display_name();
     * ```
     * @par See `TINYREFL_ENTITIES_NAMESPACE()` for details on namespace
     * reflection behavior.
     */
    constexpr tinyrefl::string display_name() const
    {
        return tinyrefl::entities::impl::display_name_resolver(
            tinyrefl::type_constant<Name>,
            tinyrefl::type_constant<DisplayName>);
    }

    /**
     * @brief Returns the full qualified display name of the entity.
     *
     * See `display_name()` for details.
     *
     * @remarks The full display name is guaranteed to be unique to each
     * reflected entity. See `id()`.
     */
    constexpr tinyrefl::string full_display_name() const
    {
        return tinyrefl::entities::impl::full_display_name_resolver(
            tinyrefl::type_constant<FullName>,
            tinyrefl::type_constant<FullDisplayName>);
    }

    /**
     * @brief Returns the entity unique identifier. Equivalent to
     * `tinyrefl::hash_constant<full_display_name().hash()>`
     */
    constexpr tinyrefl::hash_constant<
        tinyrefl::entities::impl::entity_id_resolver(
            tinyrefl::type_constant<FullName>,
            tinyrefl::type_constant<FullDisplayName>)>
        id() const
    {
        return {};
    }

    /**
     * @brief Returns source location information of the declaration of the
     * entity
     * @remarks If the entity is not known, it returns a dummy source location
     * object that points to line 0 0 of an invalid file entity.
     */
    constexpr SourceLocation source_location() const
    {
        return {};
    }

    /**
     * @brief Returns the uppermost entity in the entity tree. Equivalent to
     * `source_location().file()`.
     */
    constexpr auto root() const
    {
        return source_location().file();
    }
};

/**
 * @class entity
 * @brief Base class of all reflection metadata entity classes. Provides
 * reflection information common to all kinds of entities.
 *
 * @tparam Attributes `tinyrefl::meta::list<...>` of
 *         `tinyrefl::entities::attribute_metadata<...>` containing user defined
 *         attributes written as part of the declaration of the entity
 * @tparam Kind entity kind
 * @tparam Name `tinyrefl::hash_constant` of the non qualified name
 * @tparam FullName `tinyrefl::hash_constant` of the fullm qualified name
 * @tparam Parent Handle to the parent entity
 * @tparam Children `tinyrefl::meta::list` with the children entity classes
 * @tparam SourceLocation `tinyrefl::entities::source_location<...>` with the
 *         source location of the declaration of the entity
 * @tparam DisplayName `tinyrefl::hash_constant` of the display name
 * @tparam FullDisplayName `tinyrefl::has_constant` of the full display
 * name
 */
template<
    typename Attributes,
    tinyrefl::entities::entity_kind Kind,
    typename Name,
    typename FullName,
    typename Parent,
    typename Children,
    typename SourceLocation,
    typename DisplayName     = void,
    typename FullDisplayName = void>
struct entity_with_attributes : public entity<
                                    Kind,
                                    Name,
                                    FullName,
                                    Parent,
                                    Children,
                                    SourceLocation,
                                    DisplayName,
                                    FullDisplayName>,
                                public attributes_metadata<Attributes>
{
};

using no_children   = tinyrefl::meta::list<>;
using no_parent     = tinyrefl::hash_constant<0>;
using no_attributes = tinyrefl::meta::list<>;

template<typename T, typename = void>
struct is_entity : public std::false_type
{
};

template<typename T>
struct is_entity<T, tinyrefl::meta::void_t<typename T::tinyrefl_entity_tag>>
    : public std::true_type
{
};

template<
    tinyrefl::entities::entity_kind Kind,
    typename Name,
    typename FullName,
    typename Parent,
    typename Children,
    typename SourceLocation,
    typename DisplayName,
    typename FullDisplayName>
constexpr bool operator==(
    const entity<
        Kind,
        Name,
        FullName,
        Parent,
        Children,
        SourceLocation,
        DisplayName,
        FullDisplayName>&,
    const tinyrefl::backend::no_metadata&)
{
    return false;
}

template<
    tinyrefl::entities::entity_kind Kind,
    typename Name,
    typename FullName,
    typename Parent,
    typename Children,
    typename SourceLocation,
    typename DisplayName,
    typename FullDisplayName>
constexpr bool operator==(
    const tinyrefl::backend::no_metadata&,
    const entity<
        Kind,
        Name,
        FullName,
        Parent,
        Children,
        SourceLocation,
        DisplayName,
        FullDisplayName>&)
{
    return false;
}

template<
    tinyrefl::entities::entity_kind Kind,
    typename Name,
    typename FullName,
    typename Parent,
    typename Children,
    typename SourceLocation,
    typename DisplayName,
    typename FullDisplayName>
constexpr bool operator!=(
    const entity<
        Kind,
        Name,
        FullName,
        Parent,
        Children,
        SourceLocation,
        DisplayName,
        FullDisplayName>&,
    const tinyrefl::backend::no_metadata&)
{
    return true;
}

template<
    tinyrefl::entities::entity_kind Kind,
    typename Name,
    typename FullName,
    typename Parent,
    typename Children,
    typename SourceLocation,
    typename DisplayName,
    typename FullDisplayName>
constexpr bool operator!=(
    const tinyrefl::backend::no_metadata&,
    const entity<
        Kind,
        Name,
        FullName,
        Parent,
        Children,
        SourceLocation,
        DisplayName,
        FullDisplayName>&)
{
    return true;
}

} // namespace entities

/**
 * @brief same as `entity.index()`, but can be used as
 * part of a constant expression.
 *
 * Consider this:
 *
 * ``` cpp
 * template<typename Member, typename... Ts>
 * decltype(auto) readMemberFromTuple(
 *     const Member& member,
 *     const std::tuple<Ts...>& tuple)
 * {
 *     return std::get<member.index()>(tuple);
 * }
 *
 * readMemberFromTuple(
 *     tinyrefl::metadata<"Class::memberVariable"_id>(),
 *     std::make_tuple("hello", 42));
 * ```
 *
 * The above code does not compile since `member` is not `constexpr`
 * in the body of the function (Even if the original metadata object
 * is generated at compile time). `tinyrefl::index_of()` provides an
 * alternative for when metadata is needed in a constant expression
 * inside a function:
 *
 * ``` cpp
 * template<typename Member, typename... Ts>
 * decltype(auto) readMemberFromTuple(
 *     const Member& member,
 *     const std::tuple<Ts...>& tuple)
 * {
 *     return std::get<tinyrefl::index_of(member)>(tuple);
 * }
 * ```
 */
template<typename Entity>
constexpr std::size_t index_of(const Entity&)
{
    constexpr Entity      entity;
    constexpr std::size_t index = entity.index();
    return index;
}

} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_ENTITY_HPP_INCLUDED
