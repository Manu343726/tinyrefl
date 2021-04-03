#ifndef TINYREFL_TOOL_DIAGNOSTICREPORTER_HPP_INCLUDED
#define TINYREFL_TOOL_DIAGNOSTICREPORTER_HPP_INCLUDED

#include <cstdint>
#include <fmt/core.h>
#include <model/ast.pb.h>

namespace tinyrefl
{

namespace tool
{

class Diagnostic
{
public:
    enum class Kind
    {
        Info,
        Warning,
        Error,
        FatalError
    };

    virtual ~Diagnostic() = default;

    virtual Kind                               kind() const        = 0;
    virtual tinyrefl::tool::model::SourceRange sourceRange() const = 0;
    virtual std::string                        message() const     = 0;
    virtual std::string                        context() const     = 0;
};

tinyrefl::tool::model::SourceLocation sourceLocation(
    std::string         file   = "",
    const std::uint64_t line   = 0,
    const std::uint64_t column = 0);

tinyrefl::tool::model::SourceRange sourceRange(
    std::string         file   = "",
    const std::uint64_t line   = 0,
    const std::uint64_t column = 0);

class BasicDiagnostic final : public Diagnostic
{
public:
    BasicDiagnostic(
        const Kind                         kind,
        tinyrefl::tool::model::SourceRange sourceRange,
        std::string                        context,
        std::string                        message);

    Kind                               kind() const override;
    tinyrefl::tool::model::SourceRange sourceRange() const override;
    std::string                        message() const override;
    std::string                        context() const override;

private:
    Kind                               _kind;
    std::string                        _message;
    std::string                        _context;
    tinyrefl::tool::model::SourceRange _sourceRange;
};

class DiagnosticReporter
{
public:
    virtual ~DiagnosticReporter() = default;

    virtual void report(const Diagnostic& diagnostic) = 0;

    auto report(
        const Diagnostic::Kind             kind,
        std::string                        context = "",
        tinyrefl::tool::model::SourceRange sourceRange =
            tinyrefl::tool::sourceRange())
    {
        return [this,
                kind,
                context     = std::move(context),
                sourceRange = std::move(sourceRange)](
                   auto&& messageBody, auto&&... args) {
            const BasicDiagnostic diagnostic{
                kind,
                std::move(sourceRange),
                std::move(context),
                fmt::format(
                    std::forward<decltype(messageBody)>(messageBody),
                    std::forward<decltype(args)>(args)...)};

            report(diagnostic);
        };
    }

#define TINYREFL_TOOL_DIAGNOSTICREPORTER_METHOD(level, Level)                  \
    auto level(                                                                \
        std::string                        context = "",                       \
        tinyrefl::tool::model::SourceRange sourceRange =                       \
            tinyrefl::tool::sourceRange())                                     \
    {                                                                          \
        return report(                                                         \
            Diagnostic::Kind::Level,                                           \
            std::move(context),                                                \
            std::move(sourceRange));                                           \
    }

    TINYREFL_TOOL_DIAGNOSTICREPORTER_METHOD(info, Info)
    TINYREFL_TOOL_DIAGNOSTICREPORTER_METHOD(warning, Warning)
    TINYREFL_TOOL_DIAGNOSTICREPORTER_METHOD(error, Error)
    TINYREFL_TOOL_DIAGNOSTICREPORTER_METHOD(fatalError, FatalError)

#undef TINYREFL_TOOL_DIAGNOSTICREPORTER_METHOD
};

namespace reporters
{

class Null final : public DiagnosticReporter
{
public:
    void report(const Diagnostic& diagnostic) override {}
};

class Log final : public DiagnosticReporter
{
public:
    void report(const Diagnostic& diagnostic) override;
};

} // namespace reporters
} // namespace tool
} // namespace tinyrefl

#endif // TINYREFL_TOOL_DIAGNOSTICREPORTER_HPP_INCLUDED
