#ifndef TINYREFL_TOOL_BACKENDS_CPPAST_DATAREADER_HPP_INCLUDED
#define TINYREFL_TOOL_BACKENDS_CPPAST_DATAREADER_HPP_INCLUDED

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
    DataReader(tinyrefl::tool::DiagnosticReporter& reporter);

protected:
    tinyrefl::tool::DiagnosticReporter& reporter() const;

private:
    tinyrefl::tool::DiagnosticReporter* _reporter;
    tinyrefl::tool::reporters::Null     _dummyReporter;
};

} // namespace cppast_backend

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_BADCKENDS_CPPAST_DATAREADER_HPP_INCLUDED
