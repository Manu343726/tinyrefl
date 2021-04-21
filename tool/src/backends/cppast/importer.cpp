#include <cassert>
#include <cppast/cpp_variable.hpp>
#include <cppfs/FilePath.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <tinyrefl/tool/backends/cppast/attribute.hpp>
#include <tinyrefl/tool/backends/cppast/identifier.hpp>
#include <tinyrefl/tool/backends/cppast/importer.hpp>
#include <tinyrefl/tool/backends/cppast/source_location.hpp>
#include <tinyrefl/tool/backends/cppast/visitor.hpp>
#include <tinyrefl/tool/unique_id.hpp>

using namespace tinyrefl::tool::cppast_backend;

Importer::Importer(const Params& params)
    : _reporter(params.reporter),
      _cppastParserLogger{params.reporter},
      _parser{type_safe::ref(_index), type_safe::ref(_cppastParserLogger)},
      _config(params.config)
{
}

AstReporter Importer::reporter() const
{
    return {_reporter};
}

type_safe::optional_ref<const cppast::cpp_file>
    Importer::parse(const std::string& sourceFile)
{
    return _parser.parse(sourceFile, _config);
}

type_safe::optional<tinyrefl::tool::model::File>
    Importer::import(const std::string& sourceFile)
{
    const auto file = parse(sourceFile);

    if(not file)
    {
        _reporter.fatalError(sourceFile)(
            "Parsing errors, could not import file");

        return type_safe::nullopt;
    }

    auto result = type_safe::make_optional<tinyrefl::tool::model::File>();
    assert(result.has_value());

    if(not import(file.value(), result.value()))
    {
        return type_safe::nullopt;
    }

    return result;
}

bool Importer::import(
    const cppast::cpp_file& file, tinyrefl::tool::model::File& out)
{
    return importIdentifier(file, *out.mutable_id()) and
           importGlobalNamespace(file, *out.mutable_globalnamespace());
}

bool Importer::importGlobalNamespace(
    const cppast::cpp_file& file, tinyrefl::tool::model::Namespace& out)
{
    IdentifierReader reader{_reporter};

    out.mutable_declaration()->mutable_id()->set_name(
        reader.globalNamespaceName());
    out.mutable_declaration()->mutable_id()->set_displayname(
        reader.globalNamespaceDisplayName());
    out.mutable_declaration()->mutable_id()->set_fullname(
        reader.globalNamespaceFullName());
    out.mutable_declaration()->mutable_id()->set_fulldisplayname(
        reader.globalNamespaceFullDisplayName());
    out.mutable_declaration()->mutable_id()->set_uniqueid(
        reader.globalNamespaceUniqueId());
    out.mutable_declaration()->set_parentuniqueid(reader.uniqueId(file));

    bool ok = true;

    tinyrefl::tool::cppast_backend::visit_entity(
        file, [&](const cppast::cpp_entity& entity) {
            ok &= importNamespaceMember(
                entity, out.declaration(), *out.add_members());
        });

    return ok;
}

bool Importer::import(
    const cppast::cpp_namespace&      namespace_,
    tinyrefl::tool::model::Namespace& out)
{
    if(not importDeclaration(namespace_, *out.mutable_declaration()) or
       not importAttributes(namespace_, *out.mutable_attributes()))
    {
        return false;
    }

    bool ok = true;

    tinyrefl::tool::cppast_backend::visit_entity(
        namespace_, [&](const cppast::cpp_entity& entity) {
            ok &= importNamespaceMember(
                entity, out.declaration(), *out.add_members());
        });

    return ok;
}

bool Importer::importIdentifier(
    const cppast::cpp_entity& entity, tinyrefl::tool::model::Identifier& out)
{
    tinyrefl::tool::cppast_backend::entity_cast(
        entity, [&](const auto& entity) {
            IdentifierReader reader{_reporter};

            out.set_name(reader.name(entity));
            out.set_displayname(reader.displayName(entity));
            out.set_fullname(reader.fullName(entity));
            out.set_fulldisplayname(reader.fullDisplayName(entity));
            out.set_uniqueid(reader.uniqueId(entity));
        });

    return true;
}

bool Importer::importDeclaration(
    const cppast::cpp_entity& entity, tinyrefl::tool::model::Declaration& out)
{
    if(not importIdentifier(entity, *out.mutable_id()))
    {
        return false;
    }

    out.set_parentuniqueid(IdentifierReader(reporter()).parentUniqueId(entity));

    _reporter.warning(entity, "importDeclaration")(
        "Missing source location and range, not supported by cppast backend");
    _reporter.warning(entity, "importDeclaration")(
        "Missing declaration text, not supported by cppast backend");

    assert(not out.has_sourcelocation());
    assert(not out.has_sourcerange());
    assert(not out.has_declarationtext());

    return true;
}

bool Importer::importAttributes(
    const cppast::cpp_entity& entity,
    google::protobuf::RepeatedPtrField<tinyrefl::tool::model::Attribute>& out)
{
    for(const auto& attribute : entity.attributes())
    {
        if(not import(attribute, *out.Add()))
        {
            return false;
        }
    }

    return true;
}

bool Importer::import(
    const cppast::cpp_attribute&      attribute,
    tinyrefl::tool::model::Attribute& out)
{
    AttributeReader reader{_reporter};

    out.set_name(reader.name(attribute));
    out.set_namespace_(reader.namespace_(attribute));
    out.set_fullname(reader.fullName(attribute));
    out.set_fullattribute(reader.fullAttribute(attribute));
    out.set_joinedarguments(reader.joinedArguments(attribute));
    out.set_uniqueid(reader.uniqueId(attribute));

    for(auto argument : reader.arguments(attribute))
    {
        out.add_arguments(std::move(argument));
    }

    return true;
}

bool Importer::importNamespaceMember(
    const cppast::cpp_entity&                 entity,
    const tinyrefl::tool::model::Declaration& parentNamespaceDeclaration,
    tinyrefl::tool::model::NamespaceMember&   out)
{
    bool ok = true;

    tinyrefl::tool::cppast_backend::entity_cast(
        entity,
        [&](const cppast::cpp_namespace& namespace_) {
            ok = import(namespace_, *out.mutable_namespace_());
        },
        [&](const cppast::cpp_function& function) {
            ok = import(function, *out.mutable_function());
        },
        [&](const cppast::cpp_variable& variable) {
            ok = importVariable(variable, *out.mutable_variable());
        },
        [&](const cppast::cpp_enum& enum_) {
            ok = import(enum_, *out.mutable_enum_());
        },
        [&](const cppast::cpp_class& class_) {
            ok = import(class_, *out.mutable_class_());
        });

    return ok;
}

bool Importer::import(
    const cppast::cpp_function_base& function,
    tinyrefl::tool::model::Function& out)
{
    return importDeclaration(function, *out.mutable_declaration()) and
           importAttributes(function, *out.mutable_attributes());
}

bool Importer::importVariable(
    const cppast::cpp_entity& variable, tinyrefl::tool::model::Variable& out)
{
    assert(
        variable.kind() == cppast::cpp_entity_kind::variable_t or
        variable.kind() == cppast::cpp_entity_kind::member_variable_t);

    return importDeclaration(variable, *out.mutable_declaration()) and
           importAttributes(variable, *out.mutable_attributes());
}

bool Importer::import(
    const cppast::cpp_member_function&     function,
    const tinyrefl::tool::model::Type&     classType,
    tinyrefl::tool::model::MemberFunction& out)
{
    if(not import(function, *out.mutable_function()))
    {
        return false;
    }

    *out.mutable_class_() = classType;

    return true;
}

bool Importer::import(
    const cppast::cpp_member_variable&     variable,
    const tinyrefl::tool::model::Type&     classType,
    tinyrefl::tool::model::MemberVariable& out)
{
    if(not importVariable(variable, *out.mutable_variable()))
    {
        return false;
    }

    *out.mutable_class_() = classType;

    return true;
}

bool Importer::import(
    const cppast::cpp_constructor&      constructor,
    const tinyrefl::tool::model::Type&  classType,
    tinyrefl::tool::model::Constructor& out)
{
    if(not importDeclaration(constructor, *out.mutable_declaration()) or
       not importAttributes(constructor, *out.mutable_attributes()))
    {
        return false;
    }

    *out.mutable_class_() = classType;

    return true;
}

bool Importer::import(
    const cppast::cpp_enum& enum_, tinyrefl::tool::model::Enum& out)
{
    return importType(enum_, *out.mutable_type()) and
           importAttributes(enum_, *out.mutable_attributes());
}

bool Importer::import(
    const cppast::cpp_class& class_, tinyrefl::tool::model::Class& out)
{
    if(not importType(class_, *out.mutable_type()) or
       not importAttributes(class_, *out.mutable_attributes()))
    {
        return false;
    }

    for(const auto& baseClass : class_.bases())
    {
        if(not importType(baseClass, *out.add_baseclasses()))
        {
            return false;
        }
    }

    bool ok = true;

    tinyrefl::tool::cppast_backend::visit_entity(
        class_, [&](const cppast::cpp_entity& entity) {
            ok &= importClassMember(entity, out.type(), *out.add_members());
        });

    return ok;
}

bool Importer::importClassMember(
    const cppast::cpp_entity&           entity,
    const tinyrefl::tool::model::Type&  classType,
    tinyrefl::tool::model::ClassMember& out)
{
    bool ok = true;

    tinyrefl::tool::cppast_backend::entity_cast(
        entity,
        [&](const cppast::cpp_constructor& constructor) {
            ok = import(constructor, classType, *out.mutable_constructor());
        },
        [&](const cppast::cpp_member_function& function) {
            ok = import(function, classType, *out.mutable_function());
        },
        [&](const cppast::cpp_member_variable& variable) {
            ok = import(variable, classType, *out.mutable_variable());
        },
        [&](const cppast::cpp_function& function) {
            ok = import(function, *out.mutable_staticfunction());
        },
        [&](const cppast::cpp_variable& variable) {
            ok = importVariable(variable, *out.mutable_staticvariable());
        },
        [&](const cppast::cpp_enum& enum_) {
            ok = import(enum_, *out.mutable_enum_());
        },
        [&](const cppast::cpp_class& class_) {
            ok = import(class_, *out.mutable_class_());
        });

    return ok;
}

bool Importer::importType(
    const cppast::cpp_entity& entity, tinyrefl::tool::model::Type& out)
{
    if(not importDeclaration(entity, *out.mutable_declaration()))
    {
        return false;
    }

    _reporter.warning(entity, "importType")(
        "Decayed type ignored, not supported by cppast backend");

    assert(not out.has_decayedtype());

    return true;
}
