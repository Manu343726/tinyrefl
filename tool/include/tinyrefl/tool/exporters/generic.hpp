#ifndef TINYREFL_TOOL_EXPORTERS_GENERIC_HPP
#define TINYREFL_TOOL_EXPORTERS_GENERIC_HPP

#include <tinyrefl/tool/exporters/basic_exporter.hpp>

namespace tinyrefl
{

namespace tool
{

namespace exporters
{

class Generic final : public BasicExporter
{
public:
    enum class Exporter
    {
        Json,
        ProtobufText,
        ProtobufBinary,
        ProtobufBase64Binary
    };

    Generic(
        const Exporter exporter, tinyrefl::tool::DiagnosticReporter& reporter);

    type_safe::optional<std::string>
        Export(const tinyrefl::tool::model::File& file) override;

private:
    const Exporter _exporter;
};

} // namespace exporters

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_EXPORTERS_GENERIC_HPP
