#ifndef TINYREFL_TOOL_BACKENDS_CPPAST_IDENTIFIER_HPP
#define TINYREFL_TOOL_BACKENDS_CPPAST_IDENTIFIER_HPP

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cstdint>
#include <model/ast.pb.h>
#include <string>
#include <tinyrefl/tool/backends/cppast/datareader.hpp>
#include <tinyrefl/tool/unique_id.hpp>

namespace tinyrefl
{

namespace tool
{

namespace cppast_backend
{

class IdentifierReader : public DataReader
{
public:
    using DataReader::DataReader;

    enum class DisplayNameMode
    {
        WithFile,
        WithoutFile
    };

    std::string name(const cppast::cpp_entity& entity) const;
    std::string fullName(const cppast::cpp_entity& entity) const;
    std::string displayName(
        const cppast::cpp_entity& entity,
        const DisplayNameMode     mode = DisplayNameMode::WithoutFile) const;
    std::string fullDisplayName(
        const cppast::cpp_entity& entity,
        const DisplayNameMode     mode = DisplayNameMode::WithoutFile) const;

    std::string name(const cppast::cpp_file& file) const;
    std::string fullName(const cppast::cpp_file& file) const;
    std::string displayName(const cppast::cpp_file& file) const;
    std::string fullDisplayName(const cppast::cpp_file& file) const;

    std::string displayName(const cppast::cpp_namespace& namespace_) const;
    std::string fullDisplayName(const cppast::cpp_namespace& namespace_) const;

    template<typename T>
    std::uint64_t uniqueId(const T& entity) const
    {
        return tinyrefl::tool::uniqueId(fullDisplayName(entity));
    }

    template<typename T>
    void identitier(
        const T& entity, tinyrefl::tool::model::Identifier& out) const
    {
    }

    template<typename T>
    tinyrefl::tool::model::Identifier identitier(const T& entity) const
    {
        tinyrefl::tool::model::Identifier result;
        identitier(entity, result);
        return result;
    }

    std::uint64_t parentUniqueId(const cppast::cpp_entity& entity) const;
};


} // namespace cppast_backend

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_BACKENDS_CPPAST_IDENTIFIER_HPP
