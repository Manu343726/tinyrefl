#ifndef TINYREFL_TOOL_DETAIL_ASSERT_HPP
#define TINYREFL_TOOL_DETAIL_ASSERT_HPP

#include <debug_assert.hpp>

#ifndef TINYREFL_TOOL_ASSERT_LEVEL
#ifdef NDEBUG
#define TINYREFL_TOOL_ASSERT_LEVEL 0
#else
#define TINYREFL_TOOL_ASSERT_LEVEL 1
#endif // NDEBUG
#endif // TINYREFL_TOOL_ASSERT_LEVEL

namespace tinyrefl
{

namespace tool
{

namespace detail
{

struct assert_handler : debug_assert::set_level<TINYREFL_TOOL_ASSERT_LEVEL>,
                        debug_assert::default_handler
{
};
}
}
}

#endif // TINYREFL_TOOL_DETAIL_ASSERT_HPP
