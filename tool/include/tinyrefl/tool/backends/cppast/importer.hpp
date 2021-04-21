#ifndef TINYREFL_TOOL_BACKENDS_CPPAST_IMPORTER_HPP
#define TINYREFL_TOOL_BACKENDS_CPPAST_IMPORTER_HPP

#include <cppast/cpp_class.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_variable.hpp>
#include <cppast/libclang_parser.hpp>
#include <cppast/parser.hpp>
#include <tinyrefl/tool/backends/cppast/astreporter.hpp>
#include <tinyrefl/tool/backends/cppast/parserdiagnosticslogger.hpp>
#include <tinyrefl/tool/model/importer.hpp>
#include <type_safe/optional.hpp>

namespace tinyrefl
{

namespace tool
{

namespace cppast_backend
{

class Importer : public tinyrefl::tool::model::Importer
{
public:
    using CppastParser = cppast::simple_file_parser<cppast::libclang_parser>;

    struct Params
    {
        const CppastParser::config&         config;
        tinyrefl::tool::DiagnosticReporter& reporter;
    };

    Importer(const Params& params);

    type_safe::optional<tinyrefl::tool::model::File>
        import(const std::string& sourceFile) override;

private:
    cppast::cpp_entity_index    _index;
    AstReporter                 _reporter;
    ParserDiagnosticLogger      _cppastParserLogger;
    CppastParser                _parser;
    const CppastParser::config& _config;

    AstReporter reporter() const;

    type_safe::optional_ref<const cppast::cpp_file>
        parse(const std::string& file);

    bool import(const cppast::cpp_file& file, tinyrefl::tool::model::File& out);
    bool importGlobalNamespace(
        const cppast::cpp_file& file, tinyrefl::tool::model::Namespace& out);
    bool import(
        const cppast::cpp_namespace&      namespace_,
        tinyrefl::tool::model::Namespace& out);
    bool import(
        const cppast::cpp_class& class_, tinyrefl::tool::model::Class& out);
    bool
        import(const cppast::cpp_enum& enum_, tinyrefl::tool::model::Enum& out);
    bool import(
        const cppast::cpp_enum_value&     value,
        tinyrefl::tool::model::EnumValue& out);
    bool import(
        const cppast::cpp_function_base& function,
        tinyrefl::tool::model::Function& out);
    bool importVariable(
        const cppast::cpp_entity&        variable,
        tinyrefl::tool::model::Variable& out);
    bool importDeclaration(
        const cppast::cpp_entity&           entity,
        tinyrefl::tool::model::Declaration& out);
    bool importIdentifier(
        const cppast::cpp_entity&          entity,
        tinyrefl::tool::model::Identifier& out);
    bool importAttributes(
        const cppast::cpp_entity& entity,
        google::protobuf::RepeatedPtrField<tinyrefl::tool::model::Attribute>&
            out);
    bool import(
        const cppast::cpp_attribute&      attribute,
        tinyrefl::tool::model::Attribute& out);
    bool import(
        const cppast::cpp_member_variable&     variable,
        const tinyrefl::tool::model::Type&     classType,
        tinyrefl::tool::model::MemberVariable& out);
    bool import(
        const cppast::cpp_member_function&     function,
        const tinyrefl::tool::model::Type&     classType,
        tinyrefl::tool::model::MemberFunction& out);
    bool import(
        const cppast::cpp_constructor&      constructor,
        const tinyrefl::tool::model::Type&  classType,
        tinyrefl::tool::model::Constructor& out);
    bool importClassMember(
        const cppast::cpp_entity&           entity,
        const tinyrefl::tool::model::Type&  classType,
        tinyrefl::tool::model::ClassMember& out);
    bool importNamespaceMember(
        const cppast::cpp_entity&                 entity,
        const tinyrefl::tool::model::Declaration& parentNamespace,
        tinyrefl::tool::model::NamespaceMember&   out);
    bool importType(
        const cppast::cpp_entity& entity, tinyrefl::tool::model::Type& out);
    bool importType(const cppast::cpp_type& type, tinyrefl::tool::model::Type&);
};

} // namespace cppast_backend

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_BACKENDS_CPPAST_IMPORTER_HPP
