#include <tinyrefl/api.hpp>
#include <tinyrefl/matchers.hpp>
#include <tinyrefl/object_visitor.hpp>
#include <tinyrefl/utils/demangle.hpp>
#include <tinyrefl/visitor.hpp>

#include <iostream>
#include <sstream>

#include "example.hpp"
#include "example.hpp.tinyrefl"

void dump_all_entities_1()
{
    TINYREFL_VISIT_ENTITIES(tinyrefl::function_visitor([&](const auto& entity) {
        std::cout << entity.kind() << " " << entity.full_display_name()
                  << " (root: " << entity.root().full_display_name() << ")\n";
    }));
}

#include "example2.hpp"
#include "example2.hpp.tinyrefl"
#include <tinyrefl/entities.hpp>

void dump_all_entities_2()
{
    TINYREFL_VISIT_ENTITIES(tinyrefl::function_visitor([&](const auto& entity) {
        std::cout << entity.kind() << " " << entity.full_display_name()
                  << " (root: " << entity.root().full_display_name() << ")\n";
    }));
}


using namespace tinyrefl::literals;

void dump_pretty_function(
    int i, bool b, tinyrefl::string str, const std::string& str2)
{
    const auto function_id = tinyrefl::string{__PRETTY_FUNCTION__}.hash();

    std::cout << __PRETTY_FUNCTION__ << " (id: " << function_id << ")\n";
}

template<typename Function>
void dump_function(Function function)
{
    std::cout << "\"" << function.full_display_name() << "\" ("
              << function.kind() << ")\n";
    std::cout << "Parameters:\n";

    tinyrefl::meta::foreach(
        function.signature().parameters(), [&](const auto& param) {
            std::cout << "param \"" << param.name()
                      << "\" (type: " << param.type().full_name() << ")\n";
        });

    std::cout << "Returns: " << function.signature().return_type().full_name()
              << "\n";
}

template<typename Entity>
void recursive_dump(Entity entity)
{
    tinyrefl::recursive_visit(entity, [&](const auto& entity) {
        std::cout << std::string(entity.depth(), ' ') << entity.kind() << " "
                  << entity.full_display_name()
                  << " (parent: " << entity.parent().kind() << " "
                  << entity.parent().full_display_name() << ")\n";
    });
}

template<tinyrefl::hash_t Id>
void recursive_dump(tinyrefl::hash_constant<Id> id)
{
    recursive_dump(tinyrefl::metadata_by_id(id));
}

#ifdef TINYREFL_HAS_CONSTEXPR_LAMBDAS
template<typename Entity>
constexpr bool has_child_named(Entity entity, const tinyrefl::string name)
{
    bool result = false;

    tinyrefl::recursive_visit(
        entity, [&result, name ](const auto& entity) constexpr {
            result |= (entity.name() == name);
        });

    return result;
}
static_assert(has_child_named("example::C"_id, "f"), "");
static_assert(has_child_named(TINYREFL_ENTITIES_NAMESPACE(example), "f"), "");
#endif // TINYREFL_HAS_CONSTEXPR_LAMBDAS

void dump()
{
    std::cout << "\narbitrary entity visitors:\n\n";

    recursive_dump("example.hpp"_id);
    recursive_dump("example2.hpp"_id);

    std::cout << "\nnamespace metadata from multiple header files:\n\n";
    recursive_dump(TINYREFL_ENTITIES_NAMESPACE(example));

    std::cout << "\nvisit all reflection metadata in the translation unit:\n\n";
    dump_all_entities_1();
    std::cout << "\nvisit all reflection metadata in the translation unit:\n\n";
    dump_all_entities_2();

    static_assert(
        tinyrefl::meta::tuple_size(
            tinyrefl::metadata_by_id("example.hpp"_id).children()) == 1,
        "");

    example_namespace::C object;

    std::cout << "\nobject visitors:\n\n";

    tinyrefl::visit_member_variables(
        object,
        [&](tinyrefl::string name, const auto& variable)
            -> std::enable_if_t<
                std::is_same<std::string, std::decay_t<decltype(variable)>>::
                    value> { std::cout << name << " " << variable << "\n"; });

    tinyrefl::visit_member_functions(
        object, [&](tinyrefl::string name, auto function) {
            std::cout << name << "\n";
        });

    std::cout << "\nvisitors with SFINAE and kind constraints:\n\n";

    tinyrefl::visit<example::C>(tinyrefl::member_variable_visitor(
        [](const auto& variable)
            -> std::enable_if_t<std::is_same<
                typename std::decay_t<decltype(variable)>::value_type,
                std::string>::value> {
            std::cout << "(std::string member variable) " << variable.name()
                      << "\n";
        },
        [](const auto& variable)
            -> std::enable_if_t<!std::is_same<
                typename std::decay_t<decltype(variable)>::value_type,
                std::string>::value> {
            std::cout << "(no std::string member variable) " << variable.name()
                      << "\n";
        }));

    std::cout << "\ndetailed function info:\n\n";

    dump_function(tinyrefl::metadata_by_id(
        "example::operator<<(std::ostream&,example::B const&)"_id));

#if FALSE && defined(TINYREFL_HAS_CONCEPTS)

    std::cout << "\narbitrary concepts-constrained AST visitors:\n\n";

    tinyrefl::recursive_visit<"example.hpp"_id>(
        []<tinyrefl::entities::Function Function>(const Function& function) {
            std::cout << function.kind() << " " << function.full_display_name()
                      << "\n";
        });

#endif // TINYREFL_HAS_CONCEPTS

    std::cout << "\nmemberwise object equality:\n\n";

    example::C a, b;
    a.subobject.b = "world";
    a.b           = "world";

    std::cout << "a (" << tinyrefl::to_string(a) << ") equal to b ("
              << tinyrefl::to_string(b) << "): " << std::boolalpha
              << tinyrefl::equal(a, b) << "\n";

    std::cout << "a (" << tinyrefl::to_string(a) << ") equal to a ("
              << tinyrefl::to_string(a) << "): " << std::boolalpha
              << tinyrefl::equal(a, a) << "\n";

    tinyrefl::visit_member_variables(
        std::forward_as_tuple(a, b),
        [](const tinyrefl::string& name, auto& aMember, const auto& bMember) {
            aMember = bMember;
            std::cout << "b." << name << " asigned to a." << name << "\n";
        });

    std::cout << "a (" << tinyrefl::to_string(a) << ") equal to b ("
              << tinyrefl::to_string(b)
              << ") after memberwise assignment: " << std::boolalpha
              << tinyrefl::equal(a, b) << "\n";

    std::cout << "\n\n";
}

static_assert(
    tinyrefl::matches(
        tinyrefl::metadata<example::C>(),
        tinyrefl::matchers::hasChild(tinyrefl::matchers::named("hey_im_here"))),
    "");

static_assert(
    tinyrefl::matches(
        tinyrefl::metadata<example::Enum>(),
        tinyrefl::matchers::hasChild(tinyrefl::matchers::allOf(
            tinyrefl::matchers::ofKind(
                tinyrefl::entities::entity_kind::ENUM_VALUE),
            tinyrefl::matchers::named("A")))),
    "");

static_assert(
    tinyrefl::has_metadata("example::C::C(std::string const&)"_id),
    "tinyrefl-tool is no longer east-const???");
static_assert(
    not tinyrefl::has_metadata("example::C::C(const std::string&)"_id),
    "tinyrefl-tool is no longer east-const???");

constexpr auto c_constructor =
    tinyrefl::metadata_by_id("example::C::C(std::string const&)"_id);
static_assert(c_constructor.has_metadata(), "");

static_assert(
    c_constructor.kind() == tinyrefl::entities::entity_kind::CONSTRUCTOR, "");
static_assert(
    c_constructor.signature().return_type() ==
        tinyrefl::entities::type<example::C>(),
    "");
static_assert(
    tinyrefl::meta::tuple_size(c_constructor.signature().parameters()) == 1,
    "");
constexpr auto c_constructor_parameter =
    std::get<0>(c_constructor.signature().parameters());
static_assert(
    c_constructor_parameter.type() ==
        tinyrefl::entities::type<std::string const&>(),
    "");
static_assert(
    c_constructor_parameter.type().decayed() ==
        tinyrefl::entities::type<std::string>(),
    "");
static_assert(c_constructor_parameter.name() == "str", "");

static_assert(
    tinyrefl::matches(
        c_constructor,
        tinyrefl::matchers::allOf(
            tinyrefl::matchers::ofKind(
                tinyrefl::entities::entity_kind::CONSTRUCTOR),
            tinyrefl::matchers::returns(tinyrefl::matchers::type<example::C>()),
            tinyrefl::matchers::parameterCountIs(1),
            tinyrefl::matchers::hasParameters(tinyrefl::matchers::allOf(
                tinyrefl::matchers::hasType(
                    tinyrefl::matchers::type<std::string const&>()),
                tinyrefl::matchers::hasDecayedType(
                    tinyrefl::matchers::type<std::string>()),
                tinyrefl::matchers::named("str"))))),
    "");

#ifdef TINYREFL_MATCHES
namespace matchers_example
{
using namespace tinyrefl::matchers;
// Find all factory functions in the translation unit:
constexpr auto factories = TINYREFL_MATCHES(allOf(
    ofKind(tinyrefl::entities::entity_kind::STATIC_MEMBER_FUNCTION),
    anyOf(named("factory"), named("create")),
    returns(named("A")),
    hasParameters()));

static_assert(tinyrefl::meta::tuple_size(factories) == 1, "We hate factories");
static_assert(
    std::get<0>(factories) ==
        tinyrefl::metadata_by_id("example::A::create()"_id),
    "We hate factories");

} // namespace matchers_example
#endif // TINYREFL_MATCHES

int main()
{
    dump();
    dump_pretty_function(1, true, "", "");
}
