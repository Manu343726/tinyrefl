#ifndef TINYREFL_TOOL_BACKENDS_CPPAST_HIERARCHY_HPP_INCLUDED
#define TINYREFL_TOOL_BACKENDS_CPPAST_HIERARCHY_HPP_INCLUDED

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_file.hpp>
#include <tinyrefl/tool/backends/cppast/datareader.hpp>
#include <type_safe/optional_ref.hpp>

namespace tinyrefl
{

namespace tool
{

namespace cppast_backend
{

class HierarchyReader : public DataReader
{
public:
    using DataReader::DataReader;

    const cppast::cpp_file& root(const cppast::cpp_entity& entity) const;
    type_safe::optional_ref<const cppast::cpp_entity>
        parent(const cppast::cpp_entity& entity) const;
};

} // namespace cppast_backend

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_BACKENDS_CPPAST_HIERARCHY_HPP_INCLUDED
