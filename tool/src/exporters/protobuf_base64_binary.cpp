#include "detail/base64/base64.h"
#include <tinyrefl/tool/exporters/protobuf_base64_binary.hpp>
#include <tinyrefl/tool/exporters/protobuf_binary.hpp>

using namespace tinyrefl::tool::exporters;

type_safe::optional<std::string>
    ProtobufBase64Binary::Export(const tinyrefl::tool::model::File& file)
{
    return ProtobufBinary(reporter())
        .Export(file)
        .map([](const std::string& binary) { return base64_encode(binary); });
}
