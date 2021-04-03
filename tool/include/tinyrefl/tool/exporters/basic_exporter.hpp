#ifndef TINYREFL_TOOL_EXPORTERS_BASICEXPORTER_HPP
#define TINYREFL_TOOL_EXPORTERS_BASICEXPORTER_HPP

#include <tinyrefl/tool/diagnostic_reporter.hpp>
#include <tinyrefl/tool/model/exporter.hpp>

namespace tinyrefl
{

namespace tool
{

namespace exporters
{

class BasicExporter : public tinyrefl::tool::model::Exporter
{
public:
    BasicExporter(tinyrefl::tool::DiagnosticReporter& reporter);

protected:
    tinyrefl::tool::DiagnosticReporter& reporter() const;

private:
    tinyrefl::tool::DiagnosticReporter& _reporter;
};

} // namespace exporters

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_EXPORTERS_BASICEXPORTER_HPP
