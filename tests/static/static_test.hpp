#ifndef TINYREFL_TESTS_STATIC_TEST_HPP
#define TINYREFL_TESTS_STATIC_TEST_HPP

#include <type_traits>

#define PP_STR_IMPL(x) #x
#define PP_STR(...) PP_STR_IMPL((__VA_ARGS__))


template<typename A, typename B>
struct expect_same
{
    static_assert(std::is_same<A, B>(), "Expected A to equal B");

    constexpr expect_same() = default;

    constexpr operator bool() const
    {
        return true;
    }
};

template<typename A, typename B>
struct expect_not_same
{
    static_assert(not std::is_same<A, B>(), "Expected A not to equal B");

    constexpr expect_not_same() = default;

    constexpr operator bool() const
    {
        return true;
    }
};

#define EXPECT_TRUE(...)                                                       \
    static_assert(                                                             \
        (__VA_ARGS__), "Expected '" PP_STR(__VA_ARGS__) "' to be true")

#define EXPECT_FALSE(...)                                                      \
    static_assert(                                                             \
        !(__VA_ARGS__), "Expected '" PP_STR(__VA_ARGS__) "' to be false")

#define EXPECT_EQ(x, y)                                                        \
    static_assert(                                                             \
        x == y, "Expected '" PP_STR(x) "' to be equal to '" PP_STR(y) "'")

#define EXPECT_NE(x, y)                                                        \
    static_assert(                                                             \
        x != y, "Expected '" PP_STR(x) "' to be not equal to '" PP_STR(y) "'")

#define EXPECT_SAME(x, y) static_assert(expect_same<x, y>{}, "");

#define EXPECT_NOT_SAME(x, y) static_assert(expect_not_same<x, y>{}, "");

#endif // TINYREFL_TESTS_STATIC_TEST_HPP
