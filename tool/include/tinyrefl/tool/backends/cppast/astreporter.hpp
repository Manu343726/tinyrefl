#ifndef TINYREFL_TOOL_BACKENDS_CPPAST_ASTREPORTER_HPP_INCLUDED
#define TINYREFL_TOOL_BACKENDS_CPPAST_ASTREPORTER_HPP_INCLUDED

#include <cassert>
#include <cppast/cpp_entity.hpp>
#include <fmt/format.h>
#include <model/ast.pb.h>
#include <tinyrefl/tool/diagnostic_reporter.hpp>

namespace tinyrefl
{

namespace tool
{

namespace cppast_backend
{

class AstReporter final : public tinyrefl::tool::DiagnosticReporter
{
public:
    static constexpr bool AlreadyReporting    = true;
    static constexpr bool NotAlreadyReporting = false;

    AstReporter(
        tinyrefl::tool::DiagnosticReporter& reporter,
        const bool alreadyReporting = NotAlreadyReporting);

#define TINYREFL_TOOL_BACKENDS_CPPAST_ASTREPORTER_METHOD(level, Level)         \
    using tinyrefl::tool::DiagnosticReporter::level;                           \
                                                                               \
    auto level(const cppast::cpp_entity& entity, std::string context = "")     \
    {                                                                          \
        const auto range = sourceRange(entity);                                \
        assert(range.has_begin());                                             \
                                                                               \
        if(context.empty())                                                    \
        {                                                                      \
            context = entityName(entity);                                      \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            context = fmt::format("{}, {}", context, entityName(entity));      \
        }                                                                      \
                                                                               \
        return level(std::move(context), std::move(range));                    \
    }

    TINYREFL_TOOL_BACKENDS_CPPAST_ASTREPORTER_METHOD(info, Info)
    TINYREFL_TOOL_BACKENDS_CPPAST_ASTREPORTER_METHOD(warning, Warning)
    TINYREFL_TOOL_BACKENDS_CPPAST_ASTREPORTER_METHOD(error, Error)
    TINYREFL_TOOL_BACKENDS_CPPAST_ASTREPORTER_METHOD(fatalError, FatalError)

#undef TINYREFL_TOOL_BACKENDS_CPPAST_ASTREPORTER_METHOD

    void report(const tinyrefl::tool::Diagnostic& diagnostic) override;

    tinyrefl::tool::DiagnosticReporter& originalReporter() const;

private:
    tinyrefl::tool::model::SourceRange
                sourceRange(const cppast::cpp_entity& entity) const;
    std::string entityName(const cppast::cpp_entity& entity) const;

    tinyrefl::tool::DiagnosticReporter* _reporter;
    bool                                _alreadyReporting;

    AstReporter alreadyReportingReporter() const;
};

} // namespace cppast_backend

} // namespace tool

} // namespace tinyrefl
#endif // TINYREFL_TOOL_BACKENDS_CPPAST_ASTREPORTER_HPP_INCLUDED
