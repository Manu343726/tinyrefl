#ifndef TINYREFL_TOOL_BACKENDS_CPPAST_SOURCELOCATION_HPP_INCLUDED
#define TINYREFL_TOOL_BACKENDS_CPPAST_SOURCELOCATION_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>
#include <cppast/diagnostic.hpp>
#include <model/ast.pb.h>
#include <tinyrefl/tool/backends/cppast/datareader.hpp>

namespace tinyrefl
{

namespace tool
{

namespace cppast_backend
{

class SourceLocationReader : public DataReader
{
public:
    using DataReader::DataReader;

    void sourceLocation(
        const cppast::source_location&         sourceLocation,
        tinyrefl::tool::model::SourceLocation& out) const;
    void sourceRange(
        const cppast::source_location&      sourceLocation,
        tinyrefl::tool::model::SourceRange& out) const;
    void sourceLocation(
        const cppast::cpp_entity&              entity,
        tinyrefl::tool::model::SourceLocation& out) const;
    void sourceRange(
        const cppast::cpp_entity&           entity,
        tinyrefl::tool::model::SourceRange& out) const;

    tinyrefl::tool::model::SourceLocation
        sourceLocation(const cppast::source_location& sourceLocation) const;
    tinyrefl::tool::model::SourceRange
        sourceRange(const cppast::source_location& sourceLocation) const;
    tinyrefl::tool::model::SourceLocation
        sourceLocation(const cppast::cpp_entity& entity) const;
    tinyrefl::tool::model::SourceRange
        sourceRange(const cppast::cpp_entity& entity) const;
};
} // namespace cppast_backend

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_BACKENDS_CPPAST_SOURCELOCATION_HPP_INCLUDED
