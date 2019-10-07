#include <tinyrefl/api.hpp>
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
        object, [&](tinyrefl::string name, const auto& variable) {
            std::cout << name << " " << variable << "\n";
        });

    tinyrefl::visit_member_functions(
        object, [&](tinyrefl::string name, auto function) {
            std::cout << name << "\n";
        });

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
}

int main()
{
    dump();
    dump_pretty_function(1, true, "", "");
}
