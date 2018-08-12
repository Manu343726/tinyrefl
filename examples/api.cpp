#include <tinyrefl/api.hpp>
#include <iostream>
#include <sstream>
#include "example.hpp"
#include "example.hpp.tinyrefl"


/*
 * A generic to std::ostream serialization operator
 *
 * Uses tinyrefl::visit_object() function to visit all member
 * variables of the given object (From base classes included).
 */
template<typename T,
    // SFINAE-out any type which tinyrefl has no registered reflection data
    typename = typename std::enable_if<tinyrefl::has_metadata<T>()>::type>
std::ostream& operator<<(std::ostream& os, const T& object)
{
    std::size_t total = 0;
    std::size_t current = 0;
    os << "{";

    // The visitor filters entities on the fly, so there's no direct way to get the total number
    // of entities. Instead, we run a dummy visit first to do the count...
    tinyrefl::visit_object(object, [&total](const std::string& /* name */, auto /* depth */, auto /* member */, TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        total++;
    });

    /*
     * visit_object() takes two arguments: The object to visit and a visitor function with signature void(name string, depth, member, filter)
     * where 'name' argument contains the name of the current entity, 'depth' is a counter telling how deep in the object inheritance hierarchy
     * the visitor is, entity is a "reference" to the entity currently being visited, and filter is a type indicating the kind of entity the visitor
     * must take care of.
     * The visitor filter works by tag dispatching/overload resolution, so the exact kind of entities being visited (And how them are given to the user)
     * is determined statically. For example, if MEMBER_VARIABLE kind is given as filter (As in this operator<<() example) the "entity" parameter is a
     * reference to the current object member variable being visited. If the filter is BASE_CLASS, the entity parameter would be a reference to the base class
     * subobject.
     *
     * Since the API uses overload resolution to control the visitor behavior, to support multiple kinds of entities in the same call to visit_object()
     * users just have to pass an overloaded visitor function with different filters.
     */
    tinyrefl::visit_object(object, [&os, &current, total](const std::string& name, auto /* depth */, auto member, TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        // Since we're using MEMBER_VARIABLE as filter, the entity parameter, 'auto member' here, is
        // a reference to the current member variable of the original object
        os << "\"" << name << "\": " << member;

        if(current < total - 1)
        {
            os << ", ";
        }

        current++;
    });

    os << "}";

    return os;
}

/*
 * Generic comparison function using visit_objects() API
 */
template<typename T,
    typename = typename std::enable_if<tinyrefl::has_metadata<T>()>::type>
bool operator==(const T& lhs, const T& rhs)
{
    bool equal = true;

    /*
     * visit_objects() implements the same visitation mechanism of visit_object(),
     * but allows passing multiple objects to be visited at the same time. The only difference
     * is that where the entity argument represented a reference to the entity in visit_object(),
     * visit_objects() visitors take a multiple-value argument "entities" instead, which is a
     * std::tuple with each entity of each object.
     *
     * Also note the call syntax is a bit different, delegating the visitor argument to a
     * following call to a returned object. This was done this way to allow passing an
     * arbitrary number of objects `first.
     */
    tinyrefl::visit_objects(lhs, rhs)([&equal](const std::string& /* name */, auto /* depth */, auto members,
        TINYREFL_STATIC_VALUE(tinyrefl::entity::MEMBER_VARIABLE))
    {
        equal &= std::get<0>(members) == std::get<1>(members);
    });

    return equal;
}

int main()
{
    example::C c;

    static_assert(tinyrefl::has_metadata<example::C>(), "");
    static_assert(!tinyrefl::has_metadata<int>(), "");

    /*
     * visit_class() works the same way that visit_object(), except it recurses a class
     * instead of an object. Member variable entities are represented by member pointers instead
     * of references, and type entities (Such as member classes, member enumerations, base classes)
     * are represented as opaque tag types which the type can be extracted from (i.e. typename decltype(entity)::type)
     */
    tinyrefl::visit_class<example::C>([](const std::string& name, auto depth, auto /* entity */, auto entity_kind)
    {
        std::cout << std::string(2*depth, ' ') << "at '" << name << "' (" << decltype(entity_kind)::value << ")\n";
    });

    std::cout << "c object dump: " << c << "\n";

    std::cout << "is c == c ?: " << std::boolalpha << (c == c) << "\n";

    /*
     * $() acts as a "reflection operator", returing a constexpr metaobject
     * with the reflection information of the given type.
     *
     * In case of enum types, this constexpr metaobject implements an intuitive
     * API to introspect enumeration values
     */
    for(std::size_t i = 0; i < $(example::Enum)().count(); ++i)
    {
        const auto& value = $(example::Enum)().get_value(i);

        std::cout << " - example::Enum[" << i << "]: \"" << value.name() << "\" ("
            << value.underlying_value() << ")\n";
    }

    static_assert( $(example::Enum)().is_enumerated_value(42), "");
    static_assert(!$(example::Enum)().is_enumerated_value(42*42), "");

    std::cout << "enum to string: " << tinyrefl::metadata<example::Enum>().get_value(example::Enum::A).name() << "\n";
    constexpr example::Enum enum_value_a = tinyrefl::metadata<example::Enum>().get_value("A").value();
    (void)enum_value_a;
}


