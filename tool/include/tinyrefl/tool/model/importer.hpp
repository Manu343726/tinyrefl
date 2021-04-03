#ifndef TINYREFL_TOOL_MODEL_IMPORTER_HPP
#define TINYREFL_TOOL_MODEL_IMPORTER_HPP

#include <model/ast.pb.h>
#include <type_safe/optional.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

class Importer
{
public:
    virtual ~Importer() = default;

    virtual type_safe::optional<File> import(const std::string& sourceFile) = 0;
};

} // namespace model

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_MODEL_IMPORTER_HPP
