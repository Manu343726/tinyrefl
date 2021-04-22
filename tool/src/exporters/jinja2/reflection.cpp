#include <boost/preprocessor/stringize.hpp>
#include <functional>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/util/json_util.h>
#include <tinyrefl/tool/exporters/jinja2/reflection.hpp>
#include <tinyrefl/tool/model/utils.hpp>

using namespace jinja2;
using namespace tinyrefl::tool::model;
using namespace std::placeholders;

std::string toJson(const google::protobuf::Message& entity)
{
    std::string result;
    google::protobuf::util::MessageToJsonString(entity, &result);

    return result;
}

std::unordered_map<std::string, FieldAccessor<SourceLocation>>&
    TypeReflection<SourceLocation>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"file", std::bind(&SourceLocation::file, _1)},
        {"json", toJson},
        {"line",
         [](const SourceLocation& sourceLocation) {
             return static_cast<std::int64_t>(sourceLocation.line());
         }},
        {"column", [](const SourceLocation& sourceLocation) {
             return static_cast<std::int64_t>(sourceLocation.column());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<SourceRange>>&
    TypeReflection<SourceRange>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"begin",
         [](const SourceRange& range) {
             return range.has_begin() ? Reflect(range.begin()) : Value();
         }},
        {"json", toJson},
        {"end",
         [](const SourceRange& range) {
             return range.has_end() ? Reflect(range.end()) : Value();
         }},
        {"has_begin", std::bind(&SourceRange::has_begin, _1)},
        {"has_end", std::bind(&SourceRange::has_end, _1)}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Identifier>>&
    TypeReflection<Identifier>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"name", std::bind(&Identifier::name, _1)},
        {"json", toJson},
        {"fullName", std::bind(&Identifier::fullname, _1)},
        {"displayName", std::bind(&Identifier::displayname, _1)},
        {"fullDisplayName", std::bind(&Identifier::fulldisplayname, _1)},
        {"uniqueId", [](const Identifier& identifier) {
             return std::to_string(identifier.uniqueid());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Declaration>>&
    TypeReflection<Declaration>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"parentUniqueId",
         [](const Declaration& declaration) {
             return std::to_string(declaration.parentuniqueid());
         }},
        {"json", toJson},
        {"id",
         [](const Declaration& declaration) {
             return Reflect(declaration.id());
         }},
        {"sourceLocation",
         [](const Declaration& declaration) {
             return Reflect(declaration.sourcelocation());
         }},
        {"sourceRange",
         [](const Declaration& declaration) {
             return Reflect(declaration.sourcerange());
         }},
        {"declarationText", std::bind(&Declaration::declarationtext, _1)}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Type>>&
    TypeReflection<Type>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"declaration",
         [](const Type& type) { return Reflect(type.declaration()); }},
        {"json", toJson},
        {"decayedType",
         [](const Type& type) {
             return type.has_decayedtype() ? Reflect(type.decayedtype())
                                           : Value();
         }},
        {"has_decayedType", std::bind(&Type::has_decayedtype, _1)}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Attribute>>&
    TypeReflection<Attribute>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"name", std::bind(&Attribute::name, _1)},
        {"json", toJson},
        {"fullName", std::bind(&Attribute::fullname, _1)},
        {"namespace", std::bind(&Attribute::namespace_, _1)},
        {"fullAttribute", std::bind(&Attribute::fullattribute, _1)},
        {"arguments",
         [](const Attribute& attribute) {
             return Reflect(attribute.arguments());
         }},
        {"joinedArguments", std::bind(&Attribute::joinedarguments, _1)},
        {"uniqueId", [](const Attribute& attribute) {
             return std::to_string(attribute.uniqueid());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Parameter>>&
    TypeReflection<Parameter>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"name", std::bind(&Parameter::name, _1)},
        {"json", toJson},
        {"type",
         [](const Parameter& parameter) { return Reflect(parameter.type()); }},
        {"position",
         [](const Parameter& parameter) {
             return static_cast<std::int64_t>(parameter.position());
         }},
        {"attributes", [](const Parameter& parameter) {
             return Reflect(parameter.attributes());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Function>>&
    TypeReflection<Function>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"declaration",
         [](const Function& function) {
             return Reflect(function.declaration());
         }},
        {"json", toJson},
        {"parameters",
         [](const Function& function) {
             return Reflect(function.parameters());
         }},
        {"returnType",
         [](const Function& function) {
             return Reflect(function.returntype());
         }},
        {"attributes", [](const Function& function) {
             return Reflect(function.attributes());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<MemberFunction>>&
    TypeReflection<MemberFunction>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"declaration",
         [](const MemberFunction& function) {
             return Reflect(function.declaration());
         }},
        {"json", toJson},
        {"parameters",
         [](const MemberFunction& function) {
             return Reflect(function.parameters());
         }},
        {"returnType",
         [](const MemberFunction& function) {
             return Reflect(function.returntype());
         }},
        {"attributes",
         [](const MemberFunction& function) {
             return Reflect(function.attributes());
         }},
        {"kind",
         [](const MemberFunction& function) {
             return tinyrefl::tool::model::MemberFunction::Kind_Name(
                 function.kind());
         }},
        {"class", [](const MemberFunction& function) {
             return Reflect(function.class_());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Constructor>>&
    TypeReflection<Constructor>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"declaration",
         [](const Constructor& constructor) {
             return Reflect(constructor.declaration());
         }},
        {"json", toJson},
        {"parameters",
         [](const Constructor& constructor) {
             return Reflect(constructor.parameters());
         }},
        {"class",
         [](const Constructor& constructor) {
             return Reflect(constructor.class_());
         }},
        {"attributes", [](const Constructor& constructor) {
             return Reflect(constructor.attributes());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Variable>>&
    TypeReflection<Variable>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"declaration",
         [](const Variable& variable) {
             return Reflect(variable.declaration());
         }},
        {"json", toJson},
        {"type",
         [](const Variable& variable) { return Reflect(variable.type()); }},
        {"attributes", [](const Variable& variable) {
             return Reflect(variable.attributes());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<MemberVariable>>&
    TypeReflection<MemberVariable>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"declaration",
         [](const MemberVariable& variable) {
             return Reflect(variable.declaration());
         }},
        {"json", toJson},
        {"type",
         [](const MemberVariable& variable) {
             return Reflect(variable.type());
         }},
        {"attributes",
         [](const MemberVariable& variable) {
             return Reflect(variable.attributes());
         }},
        {"class", [](const MemberVariable& variable) {
             return Reflect(variable.class_());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<EnumValue>>&
    TypeReflection<EnumValue>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"declaration",
         [](const EnumValue& value) { return Reflect(value.declaration()); }},
        {"json", toJson},
        {"index",
         [](const EnumValue& value) {
             return static_cast<std::int64_t>(value.index());
         }},
        {"value", [](const EnumValue& value) { return value.value(); }},
        {"attributes",
         [](const EnumValue& value) { return Reflect(value.attributes()); }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Enum>>&
    TypeReflection<Enum>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"declaration",
         [](const Enum& enum_) { return Reflect(enum_.declaration()); }},
        {"type", [](const Enum& enum_) { return Reflect(enum_.type()); }},
        {"json", toJson},
        {"values", [](const Enum& enum_) { return Reflect(enum_.values()); }},
        {"attributes",
         [](const Enum& enum_) { return Reflect(enum_.attributes()); }}};

    return accessors;
}

std::string
    oneOfFieldName(const google::protobuf::Message& message, const char* name)
{
    const auto* fieldDescriptor =
        message.GetReflection()->GetOneofFieldDescriptor(
            message, message.GetDescriptor()->FindOneofByName(name));

    assert(fieldDescriptor != nullptr);

    return fieldDescriptor->name();
}

std::unordered_map<std::string, FieldAccessor<ClassMember>>&
    TypeReflection<ClassMember>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"access",
         [](const ClassMember& member) {
             return tinyrefl::tool::model::ClassMember::Access_Name(
                 member.access());
         }},
        {"json", toJson},
        {"member",
         [](const ClassMember& member) {
             switch(member.member_case())
             {
             case tinyrefl::tool::model::ClassMember::MemberCase::kFunction:
                 return Reflect(member.function());
             case tinyrefl::tool::model::ClassMember::MemberCase::kVariable:
                 return Reflect(member.variable());
             case tinyrefl::tool::model::ClassMember::MemberCase::kConstructor:
                 return Reflect(member.constructor());
             case tinyrefl::tool::model::ClassMember::MemberCase::kClass:
                 return Reflect(member.class_());
             case tinyrefl::tool::model::ClassMember::MemberCase::kEnum:
                 return Reflect(member.enum_());
             case tinyrefl::tool::model::ClassMember::MemberCase::
                 kStaticFunction:
                 return Reflect(member.staticfunction());
             case tinyrefl::tool::model::ClassMember::MemberCase::
                 kStaticVariable:
                 return Reflect(member.staticvariable());
             case tinyrefl::tool::model::ClassMember::MemberCase::
                 MEMBER_NOT_SET:
                 return Value();
             }

             return Value();
         }},
        {"kind", [](const ClassMember& member) {
             return oneOfFieldName(member, "member");
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Class>>&
    TypeReflection<Class>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"type", [](const Class& class_) { return Reflect(class_.type()); }},
        {"json", toJson},
        {"baseClasses",
         [](const Class& class_) { return Reflect(class_.baseclasses()); }},
        {"members",
         [](const Class& class_) { return Reflect(class_.members()); }},
        {"constructors",
         [](const Class& class_) {
             return Reflect(tinyrefl::tool::model::constructors(class_));
         }},
        {"variables",
         [](const Class& class_) {
             return Reflect(tinyrefl::tool::model::variables(class_));
         }},
        {"functions",
         [](const Class& class_) {
             return Reflect(tinyrefl::tool::model::functions(class_));
         }},
        {"staticFunctions",
         [](const Class& class_) {
             return Reflect(tinyrefl::tool::model::staticFunctions(class_));
         }},
        {"staticVariables",
         [](const Class& class_) {
             return Reflect(tinyrefl::tool::model::staticVariables(class_));
         }},
        {"classes",
         [](const Class& class_) {
             return Reflect(tinyrefl::tool::model::classes(class_));
         }},
        {"enums",
         [](const Class& class_) {
             return Reflect(tinyrefl::tool::model::enums(class_));
         }},
        {"declaration",
         [](const Class& class_) { return Reflect(class_.declaration()); }},
        {"attributes",
         [](const Class& class_) { return Reflect(class_.attributes()); }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<NamespaceMember>>&
    TypeReflection<NamespaceMember>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"member",
         [](const NamespaceMember& member) {
             switch(member.member_case())
             {
             case tinyrefl::tool::model::NamespaceMember::MemberCase::
                 kNamespace:
                 return Reflect(member.namespace_());
             case tinyrefl::tool::model::NamespaceMember::MemberCase::kFunction:
                 return Reflect(member.function());
             case tinyrefl::tool::model::NamespaceMember::MemberCase::kVariable:
                 return Reflect(member.variable());
             case tinyrefl::tool::model::NamespaceMember::MemberCase::kClass:
                 return Reflect(member.class_());
             case tinyrefl::tool::model::NamespaceMember::MemberCase::kEnum:
                 return Reflect(member.enum_());
             case tinyrefl::tool::model::NamespaceMember::MemberCase::
                 MEMBER_NOT_SET:
                 return Value();
             }

             return Value();
         }},
        {"json", toJson},
        {"kind", [](const NamespaceMember& member) {
             return oneOfFieldName(member, "member");
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<Namespace>>&
    TypeReflection<Namespace>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"declaration",
         [](const Namespace& namespace_) {
             return Reflect(namespace_.declaration());
         }},
        {"json", toJson},
        {"members",
         [](const Namespace& namespace_) {
             return Reflect(namespace_.members());
         }},
        {"namespaces",
         [](const Namespace& namespace_) {
             return Reflect(tinyrefl::tool::model::namespaces(namespace_));
         }},
        {"variables",
         [](const Namespace& namespace_) {
             return Reflect(tinyrefl::tool::model::variables(namespace_));
         }},
        {"functions",
         [](const Namespace& namespace_) {
             return Reflect(tinyrefl::tool::model::functions(namespace_));
         }},
        {"classes",
         [](const Namespace& namespace_) {
             return Reflect(tinyrefl::tool::model::classes(namespace_));
         }},
        {"enums",
         [](const Namespace& namespace_) {
             return Reflect(tinyrefl::tool::model::enums(namespace_));
         }},
        {"declaration",
         [](const Namespace& namespace_) {
             return Reflect(namespace_.declaration());
         }},
        {"attributes", [](const Namespace& namespace_) {
             return Reflect(namespace_.attributes());
         }}};

    return accessors;
}

std::unordered_map<std::string, FieldAccessor<File>>&
    TypeReflection<File>::GetAccessors()
{
    static std::unordered_map<std::string, FieldAccessor> accessors{
        {"id", [](const File& file) { return Reflect(file.id()); }},
        {"json", toJson},
        {"globalNamespace",
         [](const File& file) { return Reflect(file.globalnamespace()); }}};

    return accessors;
}
