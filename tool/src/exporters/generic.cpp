#include <tinyrefl/tool/exporters/generic.hpp>
#include <tinyrefl/tool/exporters/json.hpp>
#include <tinyrefl/tool/exporters/protobuf_base64_binary.hpp>
#include <tinyrefl/tool/exporters/protobuf_binary.hpp>
#include <tinyrefl/tool/exporters/protobuf_text.hpp>

using namespace tinyrefl::tool::exporters;

Generic::Generic(
    const Exporter exporter, tinyrefl::tool::DiagnosticReporter& reporter)
    : BasicExporter{reporter}, _exporter{exporter}
{
}

type_safe::optional<std::string>
    Generic::Export(const tinyrefl::tool::model::File& file)
{
    switch(_exporter)
    {
    case Exporter::Json:
        return Json(reporter()).Export(file);
    case Exporter::ProtobufBinary:
        return ProtobufBinary(reporter()).Export(file);
    case Exporter::ProtobufBase64Binary:
        return ProtobufBase64Binary(reporter()).Export(file);
    case Exporter::ProtobufText:
        return ProtobufText(reporter()).Export(file);
    }

    // unreachable
    return "";
}
