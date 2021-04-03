#include <tinyrefl/tool/backends/cppast/hierarchy.hpp>
#include <tinyrefl/tool/backends/cppast/identifier.hpp>
#include <tinyrefl/tool/backends/cppast/source_location.hpp>

using namespace tinyrefl::tool::model;
using namespace tinyrefl::tool::cppast_backend;

void SourceLocationReader::sourceLocation(
    const cppast::source_location&         location,
    tinyrefl::tool::model::SourceLocation& out) const
{
    if(not location.file.has_value())
    {
        reporter().warning()("No file information found in source location");
    }

    if(not location.line.has_value())
    {
        reporter().warning()("No line information found in source location");
    }

    if(not location.column.has_value())
    {
        reporter().warning()("No column information found in source location");
    }

    out.set_file(location.file.value_or(""));
    out.set_line(location.line.value_or(0));
    out.set_column(location.column.value_or(0));
}

void SourceLocationReader::sourceRange(
    const cppast::source_location&      location,
    tinyrefl::tool::model::SourceRange& out) const
{
    sourceLocation(location, *out.mutable_begin());
}

void SourceLocationReader::sourceLocation(
    const cppast::cpp_entity&              entity,
    tinyrefl::tool::model::SourceLocation& out) const
{
    HierarchyReader  hierarchyReader{reporter()};
    IdentifierReader identifierReader{reporter()};

    out.set_file(identifierReader.fullName(hierarchyReader.root(entity)));
    out.set_line(0);
    out.set_column(0);
}

void SourceLocationReader::sourceRange(
    const cppast::cpp_entity&           entity,
    tinyrefl::tool::model::SourceRange& out) const
{
    sourceLocation(entity, *out.mutable_begin());
}

SourceLocation SourceLocationReader::sourceLocation(
    const cppast::source_location& location) const
{
    SourceLocation result;
    sourceLocation(location, result);
    return result;
}

SourceRange SourceLocationReader::sourceRange(
    const cppast::source_location& location) const
{
    SourceRange result;
    sourceRange(location, result);
    return result;
}

SourceLocation
    SourceLocationReader::sourceLocation(const cppast::cpp_entity& entity) const
{
    SourceLocation result;
    sourceLocation(entity, result);
    return result;
}

SourceRange
    SourceLocationReader::sourceRange(const cppast::cpp_entity& entity) const
{
    SourceRange result;
    sourceRange(entity, result);
    return result;
}
