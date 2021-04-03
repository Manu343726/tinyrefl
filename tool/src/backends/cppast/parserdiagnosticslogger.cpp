#include <tinyrefl/tool/backends/cppast/parserdiagnosticslogger.hpp>
#include <tinyrefl/tool/backends/cppast/source_location.hpp>

using namespace tinyrefl::tool::cppast_backend;

ParserDiagnostic::ParserDiagnostic(
    const char* source, const cppast::diagnostic& diagnostic)
    : _source{source}, _diagnostic(diagnostic)
{
}

tinyrefl::tool::Diagnostic::Kind ParserDiagnostic::kind() const
{
    assert(_diagnostic.severity != cppast::severity::debug);

    switch(_diagnostic.severity)
    {
    case cppast::severity::debug:
    case cppast::severity::info:
        return Kind::Info;
    case cppast::severity::warning:
        return Kind::Warning;
    case cppast::severity::error:
        return Kind::Error;
    case cppast::severity::critical:
        return Kind::FatalError;
    }

    // unreachable
    return Kind::FatalError;
}

std::string ParserDiagnostic::message() const
{
    return _diagnostic.message;
}

std::string ParserDiagnostic::context() const
{
    return _source;
}

tinyrefl::tool::model::SourceRange ParserDiagnostic::sourceRange() const
{
    return SourceLocationReader().sourceRange(_diagnostic.location);
}

ParserDiagnosticLogger::ParserDiagnosticLogger(
    tinyrefl::tool::DiagnosticReporter& reporter)
    : _reporter(reporter)
{
}

bool ParserDiagnosticLogger::do_log(
    const char* source, const cppast::diagnostic& diagnostic) const
{
    if(diagnostic.severity == cppast::severity::debug)
    {
        return false;
    }

    _reporter.report(ParserDiagnostic{source, diagnostic});

    return true;
}
