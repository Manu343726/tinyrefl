#include <fmt/format.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <tinyrefl/tool/diagnostic_reporter.hpp>

using namespace tinyrefl::tool;
using namespace tinyrefl::tool::model;
using namespace tinyrefl::tool::reporters;

BasicDiagnostic::BasicDiagnostic(
    const Kind                         kind,
    tinyrefl::tool::model::SourceRange sourceRange,
    std::string                        context,
    std::string                        message)
    : _kind{kind},
      _message{std::move(message)},
      _context{std::move(context)},
      _sourceRange{std::move(sourceRange)}
{
}

Diagnostic::Kind BasicDiagnostic::kind() const
{
    return _kind;
}

SourceRange BasicDiagnostic::sourceRange() const
{
    return _sourceRange;
}

std::string BasicDiagnostic::message() const
{
    return _message;
}

std::string BasicDiagnostic::context() const
{
    return _context;
}

spdlog::level::level_enum loggerLevel(const Diagnostic::Kind diagnosticKind)
{
    switch(diagnosticKind)
    {
    case Diagnostic::Kind::Info:
        return spdlog::level::info;
    case Diagnostic::Kind::Warning:
        return spdlog::level::warn;
    case Diagnostic::Kind::Error:
        return spdlog::level::err;
    case Diagnostic::Kind::FatalError:
        return spdlog::level::critical;
    }

    // unreachable
    return spdlog::level::critical;
}

void Log::report(const Diagnostic& diagnostic)
{
    const auto sourceRange = diagnostic.sourceRange();
    assert(sourceRange.has_begin());
    const auto& sourceLocation = sourceRange.begin();
    const auto  context        = diagnostic.context();
    const auto  contextString =
        (context.empty() ? "" : fmt::format(", [{}]", context));
    const auto sourceLocationString =
        (sourceLocation.file().empty()
             ? ""
             : fmt::format(
                   "{file}:{line}:{column}",
                   fmt::arg("file", sourceLocation.file()),
                   fmt::arg("line", sourceLocation.line()),
                   fmt::arg("column", sourceLocation.column())));

    spdlog::log(
        loggerLevel(diagnostic.kind()),
        "{sourceLocation}{context}: {message}",
        fmt::arg("sourceLocation", sourceLocationString),
        fmt::arg("message", diagnostic.message()),
        fmt::arg("context", contextString));
}

namespace tinyrefl
{

namespace tool
{

SourceLocation sourceLocation(
    std::string file, const std::uint64_t line, const std::uint64_t column)
{
    SourceLocation result;
    result.set_file(std::move(file));
    result.set_line(line);
    result.set_column(column);
    return result;
}

SourceRange sourceRange(
    std::string file, const std::uint64_t line, const std::uint64_t column)
{
    SourceRange result;
    *result.mutable_begin() = sourceLocation(std::move(file), line, column);
    return result;
}

} // namespace tool

} // namespace tinyrefl
