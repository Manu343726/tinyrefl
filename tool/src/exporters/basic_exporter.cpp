#include <tinyrefl/tool/exporters/basic_exporter.hpp>

using namespace tinyrefl::tool::exporters;

BasicExporter::BasicExporter(tinyrefl::tool::DiagnosticReporter& reporter)
    : _reporter(reporter)
{
}

tinyrefl::tool::DiagnosticReporter& BasicExporter::reporter() const
{
    return _reporter;
}
