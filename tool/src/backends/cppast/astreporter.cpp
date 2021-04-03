#include <tinyrefl/tool/backends/cppast/astreporter.hpp>
#include <tinyrefl/tool/backends/cppast/identifier.hpp>
#include <tinyrefl/tool/backends/cppast/source_location.hpp>

using namespace tinyrefl::tool::model;
using namespace tinyrefl::tool::cppast_backend;

AstReporter::AstReporter(
    tinyrefl::tool::DiagnosticReporter& reporter, const bool alreadyReporting)
    : _reporter{&reporter}, _alreadyReporting{alreadyReporting}
{
}

tinyrefl::tool::DiagnosticReporter& AstReporter::originalReporter() const
{
    return *_reporter;
}

void AstReporter::report(const tinyrefl::tool::Diagnostic& diagnostic)
{
    _reporter->report(diagnostic);
}

SourceRange AstReporter::sourceRange(const cppast::cpp_entity& entity) const
{
    if(not _alreadyReporting)
    {
        return SourceLocationReader(alreadyReportingReporter())
            .sourceRange(entity);
    }
    else
    {
        return tinyrefl::tool::sourceRange();
    }
}

std::string AstReporter::entityName(const cppast::cpp_entity& entity) const
{
    if(not _alreadyReporting)
    {
        return IdentifierReader(alreadyReportingReporter())
            .fullDisplayName(entity);
    }
    else
    {
        return entity.name();
    }
}

AstReporter AstReporter::alreadyReportingReporter() const
{
    return {originalReporter(), AlreadyReporting};
}
