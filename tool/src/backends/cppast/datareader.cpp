#include <tinyrefl/tool/backends/cppast/datareader.hpp>

using namespace tinyrefl::tool::cppast_backend;

DataReader::DataReader() : _reporter{_dummyReporter} {}

DataReader::DataReader(tinyrefl::tool::DiagnosticReporter& reporter)
    : _reporter{reporter}
{
}

DataReader::DataReader(AstReporter reporter) : _reporter{std::move(reporter)} {}

AstReporter& DataReader::reporter() const
{
    return _reporter;
}
