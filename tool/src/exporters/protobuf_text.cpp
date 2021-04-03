#include <tinyrefl/tool/exporters/protobuf_text.hpp>

using namespace tinyrefl::tool::exporters;

type_safe::optional<std::string>
    ProtobufText::Export(const tinyrefl::tool::model::File& file)
{
    return file.DebugString();
}
