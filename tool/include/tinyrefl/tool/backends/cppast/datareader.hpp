#ifndef TINYREFL_TOOL_BACKENDS_CPPAST_SERIALIZER_HPP_INCLUDED
#define TINYREFL_TOOL_BACKENDS_CPPAST_SERIALIZER_HPP_INCLUDED

#include <tinyrefl/tool/backends/cppast/astreporter.hpp>
#include <tinyrefl/tool/diagnostic_reporter.hpp>

namespace tinyrefl
{

namespace tool
{

namespace cppast_backend
{

class DataReader
{
public:
    DataReader();
    DataReader(AstReporter reporter);
    DataReader(tinyrefl::tool::DiagnosticReporter& reporter);

protected:
    AstReporter& reporter() const;

private:
    tinyrefl::tool::reporters::Null _dummyReporter;
    mutable AstReporter             _reporter;
};

} // namespace cppast_backend

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_BADCKENDS_CPPAST_SERIALIZER_HPP_INCLUDED
