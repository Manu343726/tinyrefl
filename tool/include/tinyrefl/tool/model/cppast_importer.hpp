#ifndef TINYREFL_TOOL_MODEL_CPPASTIMPORTER_HPP
#define TINYREFL_TOOL_MODEL_CPPASTIMPORTER_HPP

#include <cppast/cpp_class.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_variable.hpp>
#include <cppast/libclang_parser.hpp>
#include <cppast/parser.hpp>
#include <tinyrefl/tool/model/importer.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

class CppastImporter : public Importer
{
public:
    using CppastParser = cppast::simple_file_parser<cppast::libclang_parser>;

    CppastImporter(CppastParser& parser, CppastParser::config config);

    File import(const std::string& sourceFile) override;

private:
    CppastParser&        _parser;
    CppastParser::config _config;

    bool import(const cppast::cpp_namespace& namespace_, Namespace& out);
    bool import(const cppast::cpp_class& class_, Class& out);
    bool import(const cppast::cpp_enum& enum_, Enum& out);
    bool import(const cppast::cpp_enum_value& value, EnumValue& out);
    bool import(const cppast::cpp_function& function, Function& out);
    bool import(const cppast::cpp_variable& variable, Variable& out);
    bool importMemberVariable(
        const cppast::cpp_variable& variable,
        const Type&                 classType,
        MemberVariable&             out);
    bool importMemberFunction(
        const cppast::cpp_function& function,
        const Type&                 classType,
        MemberVariable&             out);
    bool import(
        const cppast::cpp_constructor& constructor,
        const Type&                    classType,
        Constructor&                   out);
    bool importMemberClass(
        const cppast::cpp_class& memberClass,
        const Type&              parentClassType,
        Class&                   out);
    bool importMemberEnum(
        const cppast::cpp_enum& memberEnum,
        const Type&             parentClassType,
        Enum&                   out);
    bool importClassMember(
        const cppast::cpp_entity& entity,
        const Type&               classType,
        ClassMember&              out);
    bool importNamespaceMember(
        const cppast::cpp_entity& entity,
        const Declaration&        parentNamespace,
        NamespaceMember&          out);
};

} // namespace model

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_MODEL_CPPASTIMPORTER_HPP
