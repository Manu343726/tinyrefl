#include "static_test.hpp"
#include <tinyrefl/utils/invokable_traits.hpp>

using namespace tinyrefl::invokable_traits;
using tinyrefl::meta::list;

inline namespace test
{

int function();
int functionTakingOneArg(int&);
int variable;

struct Class
{
    int        memberFunction();
    int        constMemberFunction() const;
    static int staticMemberFunction();

    int        memberVariable;
    static int staticMemberVariable;
};

} // namespace test

EXPECT_EQ(kind<decltype(function)>, invokable_kind::FREE_FUNCTION);
EXPECT_SAME(return_type<decltype(function)>, int);
EXPECT_SAME(class_type<decltype(function)>, void);
EXPECT_SAME(arguments<decltype(function)>, list<>);
EXPECT_SAME(decayed_arguments<decltype(function)>, list<>);
EXPECT_SAME(signature<decltype(function)>, int());

EXPECT_EQ(kind<decltype(functionTakingOneArg)>, invokable_kind::FREE_FUNCTION);
EXPECT_SAME(return_type<decltype(functionTakingOneArg)>, int);
EXPECT_SAME(class_type<decltype(functionTakingOneArg)>, void);
EXPECT_SAME(arguments<decltype(functionTakingOneArg)>, list<int&>);
EXPECT_SAME(decayed_arguments<decltype(functionTakingOneArg)>, list<int>);
EXPECT_SAME(signature<decltype(functionTakingOneArg)>, int(int&));

EXPECT_EQ(kind<decltype(variable)>, invokable_kind::VARIABLE);
EXPECT_SAME(return_type<decltype(variable)>, int);
EXPECT_SAME(class_type<decltype(variable)>, void);
EXPECT_SAME(arguments<decltype(variable)>, list<>);
EXPECT_SAME(decayed_arguments<decltype(variable)>, list<>);
EXPECT_SAME(signature<decltype(variable)>, int());

EXPECT_EQ(
    kind<decltype(&Class::memberFunction)>,
    invokable_kind::NON_CONST_MEMBER_FUNCTION);
EXPECT_SAME(return_type<decltype(&Class::memberFunction)>, int);
EXPECT_SAME(class_type<decltype(&Class::memberFunction)>, Class);
EXPECT_SAME(arguments<decltype(&Class::memberFunction)>, list<>);
EXPECT_SAME(decayed_arguments<decltype(&Class::memberFunction)>, list<>);
EXPECT_SAME(signature<decltype(&Class::memberFunction)>, int());

EXPECT_EQ(
    kind<decltype(&Class::constMemberFunction)>,
    invokable_kind::CONST_MEMBER_FUNCTION);
EXPECT_SAME(return_type<decltype(&Class::constMemberFunction)>, int);
EXPECT_SAME(class_type<decltype(&Class::constMemberFunction)>, Class);
EXPECT_SAME(arguments<decltype(&Class::constMemberFunction)>, list<>);
EXPECT_SAME(decayed_arguments<decltype(&Class::constMemberFunction)>, list<>);
EXPECT_SAME(signature<decltype(&Class::constMemberFunction)>, int());

EXPECT_EQ(
    kind<decltype(Class::staticMemberFunction)>, invokable_kind::FREE_FUNCTION);
EXPECT_SAME(return_type<decltype(Class::staticMemberFunction)>, int);
EXPECT_SAME(class_type<decltype(Class::staticMemberFunction)>, void);
EXPECT_SAME(arguments<decltype(Class::staticMemberFunction)>, list<>);
EXPECT_SAME(decayed_arguments<decltype(Class::staticMemberFunction)>, list<>);
EXPECT_SAME(signature<decltype(Class::staticMemberFunction)>, int());

EXPECT_EQ(
    kind<decltype(&Class::memberVariable)>, invokable_kind::MEMBER_VARIABLE);
EXPECT_SAME(return_type<decltype(&Class::memberVariable)>, int);
EXPECT_SAME(class_type<decltype(&Class::memberVariable)>, Class);
EXPECT_SAME(arguments<decltype(&Class::memberVariable)>, list<>);
EXPECT_SAME(decayed_arguments<decltype(&Class::memberVariable)>, list<>);
EXPECT_SAME(signature<decltype(&Class::memberVariable)>, int());

EXPECT_EQ(
    kind<decltype(Class::staticMemberVariable)>, invokable_kind::VARIABLE);
EXPECT_SAME(return_type<decltype(Class::staticMemberVariable)>, int);
EXPECT_SAME(class_type<decltype(Class::staticMemberVariable)>, void);
EXPECT_SAME(arguments<decltype(Class::staticMemberVariable)>, list<>);
EXPECT_SAME(decayed_arguments<decltype(Class::staticMemberVariable)>, list<>);
EXPECT_SAME(signature<decltype(Class::staticMemberVariable)>, int());
