#include <google/protobuf/util/json_util.h>
#include <tinyrefl/tool/exporters/json.hpp>

using namespace tinyrefl::tool::exporters;

type_safe::optional<std::string>
    Json::Export(const tinyrefl::tool::model::File& file)
{
    google::protobuf::util::JsonPrintOptions options;
    options.always_print_enums_as_ints    = true;
    options.always_print_primitive_fields = true;
    options.add_whitespace                = true;
    options.preserve_proto_field_names    = true;

    std::string result;
    const auto  resultCode =
        google::protobuf::util::MessageToJsonString(file, &result, options);

    if(not resultCode.ok())
    {
        reporter().fatalError("exporters.json")(resultCode.ToString());
        return type_safe::nullopt;
    }

    return result;
}
