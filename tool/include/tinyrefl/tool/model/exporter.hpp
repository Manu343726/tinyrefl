#ifndef TINYREFL_TOOL_MODEL_EXPORTER_HPP
#define TINYREFL_TOOL_MODEL_EXPORTER_HPP

#include <model/ast.pb.h>
#include <string>
#include <type_safe/optional.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

class Exporter
{
public:
    virtual ~Exporter() = default;

    virtual type_safe::optional<std::string>
        Export(const tinyrefl::tool::model::File& file) = 0;
};

} // namespace model

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_MODEL_EXPORTER_HPP
