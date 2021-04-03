#ifndef TINYREFL_TOOL_BACKENDS_CPPAST_PARSERDIAGNOSTICSLOGGER_HPP_INCLUDED
#define TINYREFL_TOOL_BACKENDS_CPPAST_PARSERDIAGNOSTICSLOGGER_HPP_INCLUDED

#include <cppast/diagnostic_logger.hpp>
#include <tinyrefl/tool/diagnostic_reporter.hpp>

namespace tinyrefl
{

namespace tool
{

namespace cppast_backend
{

class ParserDiagnosticLogger final : public cppast::diagnostic_logger
{
public:
    ParserDiagnosticLogger(tinyrefl::tool::DiagnosticReporter& reporter);

private:
    tinyrefl::tool::DiagnosticReporter& _reporter;

    bool do_log(const char* source, const cppast::diagnostic& diagnostic)
        const override;
};

class ParserDiagnostic final : public tinyrefl::tool::Diagnostic
{
public:
    ParserDiagnostic(const char* source, const cppast::diagnostic& diagnostic);

    Kind                               kind() const override;
    std::string                        message() const override;
    tinyrefl::tool::model::SourceRange sourceRange() const override;
    std::string                        context() const override;

private:
    const char*               _source;
    const cppast::diagnostic& _diagnostic;
};

} // namespace cppast_backend

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_BACKENDS_CPPAST_PARSERDIAGNOSTICSLOGGER_HPP_INCLUDED
