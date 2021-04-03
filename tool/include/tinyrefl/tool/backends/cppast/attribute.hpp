#ifndef TINYREFL_TOOL_BACKENDS_CPPAST_ATTRIBUTE_HPP_INCLUDED
#define TINYREFL_TOOL_BACKENDS_CPPAST_ATTRIBUTE_HPP_INCLUDED

#include <cppast/cpp_attribute.hpp>
#include <cppast/cpp_entity.hpp>
#include <cstdint>
#include <model/ast.pb.h>
#include <tinyrefl/tool/backends/cppast/datareader.hpp>

namespace tinyrefl
{

namespace tool
{

namespace cppast_backend
{

class AttributeReader : public DataReader
{
public:
    using DataReader::DataReader;

    std::string name(const cppast::cpp_attribute& attribute) const;
    std::string fullName(const cppast::cpp_attribute& attribute) const;
    std::string fullAttribute(const cppast::cpp_attribute& attribute) const;
    std::string namespace_(const cppast::cpp_attribute& attribute) const;
    std::vector<std::string>
                  arguments(const cppast::cpp_attribute& attribute) const;
    std::string   joinedArguments(const cppast::cpp_attribute& attribute) const;
    std::uint64_t uniqueId(const cppast::cpp_attribute& attribute) const;
};

} // namespace cppast_backend

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_BACKENDS_CPPAST_ATTRIBUTES_HPP_INCLUDED
