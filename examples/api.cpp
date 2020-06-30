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
    std::cout << "Arguments:\n";

    tinyrefl::meta::foreach(
        function.arguments().arguments(), [&](const auto& arg) {
            std::cout << "arg \"" << arg.name()
                      << "\" (type: " << arg.type_name() << ")\n";
        });
}

template<typename Entity>
void recursive_dump(Entity entity)
{
    tinyrefl::recursive_visit(entity, [&](const auto& entity) {
        std::cout << entity.kind() << " " << entity.full_display_name()
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

static_assert(tinyrefl::matches(
    tinyrefl::metadata<example::C>(),
    tinyrefl::matchers::has(tinyrefl::matchers::named("hey_im_here"))));

static_assert(tinyrefl::matches(
    tinyrefl::metadata<example::Enum>(),
    tinyrefl::matchers::has(tinyrefl::matchers::allOf(
        tinyrefl::matchers::ofKind(tinyrefl::entities::entity_kind::ENUM_VALUE),
        tinyrefl::matchers::named("A")))));

// Find all factory functions in the translation unit:
constexpr auto factories = tinyrefl::matches(tinyrefl::matchers::allOf(
    tinyrefl::matchers::ofKind(
        tinyrefl::entities::entity_kind::STATIC_MEMBER_FUNCTION),
    tinyrefl::matchers::anyOf(
        tinyrefl::matchers::named("factory"),
        tinyrefl::matchers::named("create"))));

static_assert(tinyrefl::meta::tuple_size(factories) == 0);


int main()
{
    dump();
    dump_pretty_function(1, true, "", "");
}
