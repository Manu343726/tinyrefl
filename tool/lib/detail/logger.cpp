#include <spdlog/sinks/stdout_color_sinks.h>
#include <tinyrefl/tool/detail/logger.hpp>

namespace tinyrefl
{

namespace tool
{

namespace detail
{
spdlog::logger& log()
{
    static auto logger = [] {
        auto logger = spdlog::stdout_color_mt("tinyrefl_tool_stdout");
        return logger;
    }();

    return *logger;
}
}
}
}
