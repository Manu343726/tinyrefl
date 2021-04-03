#ifndef TINYREFL_TOOL_EXPORTERS_PROTOBUFBINARY_HPP
#define TINYREFL_TOOL_EXPORTERS_PROTOBUFBINARY_HPP

#include <tinyrefl/tool/exporters/basic_exporter.hpp>

namespace tinyrefl
{

namespace tool
{

namespace exporters
{

class ProtobufBinary final : public BasicExporter
{
public:
    using BasicExporter::BasicExporter;

    type_safe::optional<std::string>
        Export(const tinyrefl::tool::model::File& file) override;
};

} // namespace exporters

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_EXPORTERS_PROTOBUFBINARY_HPP
