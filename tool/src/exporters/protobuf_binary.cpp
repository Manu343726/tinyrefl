#include <tinyrefl/tool/exporters/protobuf_binary.hpp>

using namespace tinyrefl::tool::exporters;

type_safe::optional<std::string>
    ProtobufBinary::Export(const tinyrefl::tool::model::File& file)
{
    return file.SerializeAsString();
}
