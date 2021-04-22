#ifndef TINYREFL_TOOL_MODEL_INDEX_HPP_INCLUDED
#define TINYREFL_TOOL_MODEL_INDEX_HPP_INCLUDED

#include <model/ast.pb.h>
#include <spdlog/spdlog.h>
#include <tinyrefl/utils/overloaded_function.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

enum class VisitResult
{
    Continue,
    ContinueRecursive,
    End,
    Error
};

template<typename Entity, typename Visitor>
VisitResult visit(const Entity& entity, const Visitor& visitor)
{
    return visitor(entity);
}

template<typename Visitor>
VisitResult visit(
    const tinyrefl::tool::model::Namespace& namespace_, const Visitor& visitor);

template<typename Visitor>
VisitResult
    visit(const tinyrefl::tool::model::Class& class_, const Visitor& visitor);

template<typename Visitor>
VisitResult
    visit(const tinyrefl::tool::model::Enum& enum_, const Visitor& visitor)
{
    auto result = visitor(enum_);

    if(result == VisitResult::ContinueRecursive)
    {
        for(const auto& member : enum_.values())
        {
            const auto result = visit(member, visitor);

            if(result != VisitResult::Continue and
               result != VisitResult::ContinueRecursive)
            {
                return result;
            }
        }
    }

    return result;
}

template<typename Visitor>
VisitResult visit(
    const tinyrefl::tool::model::NamespaceMember& member,
    const Visitor&                                visitor)
{
    switch(member.member_case())
    {
    case tinyrefl::tool::model::NamespaceMember::MemberCase::kClass:
        return visit(member.class_(), visitor);
    case tinyrefl::tool::model::NamespaceMember::MemberCase::kEnum:
        return visit(member.enum_(), visitor);
    case tinyrefl::tool::model::NamespaceMember::MemberCase::kNamespace:
        return visit(member.namespace_(), visitor);
    case tinyrefl::tool::model::NamespaceMember::MemberCase::kFunction:
        return visit(member.function(), visitor);
    case tinyrefl::tool::model::NamespaceMember::MemberCase::kVariable:
        return visit(member.variable(), visitor);
    case tinyrefl::tool::model::NamespaceMember::MemberCase::MEMBER_NOT_SET:
        return VisitResult::Error;
    }

    return VisitResult::Error;
}

template<typename Visitor>
VisitResult visit(
    const tinyrefl::tool::model::ClassMember& member, const Visitor& visitor)
{
    switch(member.member_case())
    {
    case tinyrefl::tool::model::ClassMember::MemberCase::kClass:
        return visit(member.class_(), visitor);
    case tinyrefl::tool::model::ClassMember::MemberCase::kEnum:
        return visit(member.enum_(), visitor);
    case tinyrefl::tool::model::ClassMember::MemberCase::kFunction:
        return visit(member.function(), visitor);
    case tinyrefl::tool::model::ClassMember::MemberCase::kConstructor:
        return visit(member.constructor(), visitor);
    case tinyrefl::tool::model::ClassMember::MemberCase::kVariable:
        return visit(member.variable(), visitor);
    case tinyrefl::tool::model::ClassMember::MemberCase::kStaticFunction:
        return visit(member.staticfunction(), visitor);
    case tinyrefl::tool::model::ClassMember::MemberCase::kStaticVariable:
        return visit(member.staticvariable(), visitor);
    case tinyrefl::tool::model::ClassMember::MemberCase::MEMBER_NOT_SET:
        return VisitResult::Error;
    }

    return VisitResult::Error;
}

template<typename Entity, typename Visitor>
VisitResult visitMembers(const Entity& entity, const Visitor& visitor)
{
    for(const auto& member : entity.members())
    {
        const auto result = visit(member, visitor);

        if(result != VisitResult::Continue and
           result != VisitResult::ContinueRecursive)
        {
            return result;
        }
    }

    return VisitResult::End;
}

template<typename Visitor>
VisitResult visit(
    const tinyrefl::tool::model::Namespace& namespace_, const Visitor& visitor)
{
    auto result = visitor(namespace_);

    if(result == VisitResult::ContinueRecursive)
    {
        result = visitMembers(namespace_, visitor);
    }

    return result;
}

template<typename Visitor>
VisitResult
    visit(const tinyrefl::tool::model::Class& class_, const Visitor& visitor)
{
    auto result = visitor(class_);

    if(result == VisitResult::ContinueRecursive)
    {
        result = visitMembers(class_, visitor);
    }

    return result;
}

template<typename Visitor>
VisitResult
    visit(const tinyrefl::tool::model::File& file, const Visitor& visitor)
{
    return visit(file.globalnamespace(), visitor);
}

class Index
{
public:
    Index(const tinyrefl::tool::model::File& file);

    template<typename Entity>
    const Entity* byName(const std::string& name) const
    {
        Entity const* result = nullptr;

        visit(_file, [&](const auto& entity) {
            auto visitResult = VisitResult::ContinueRecursive;


            tinyrefl::overloaded_function_default([&](const Entity& entity) {
                if(entity.declaration().id().fulldisplayname() == name or
                   entity.declaration().id().displayname() == name)
                {
                    result      = &entity;
                    visitResult = VisitResult::End;
                }
            })(entity);

            return visitResult;
        });

        return result;
    }

    template<typename Entity>
    std::vector<const Entity*> all() const
    {
        std::vector<const Entity*> result;

        visit(_file, [&](const auto& entity) {
            spdlog::trace(
                "visit {} '{}'",
                entity.GetDescriptor()->name(),
                entity.declaration().id().fulldisplayname());

            tinyrefl::overloaded_function_default([&](const Entity& entity) {
                spdlog::trace(
                    "found {} '{}'",
                    entity.GetDescriptor()->name(),
                    entity.declaration().id().fulldisplayname());
                result.push_back(&entity);
            })(entity);

            return VisitResult::ContinueRecursive;
        });

        return result;
    }

    const Namespace* namespaceByName(const std::string& name) const;
    const Class*     classByName(const std::string& name) const;
    const Enum*      enumByName(const std::string& name) const;
    const Function*  functionByName(const std::string& name) const;
    const Variable*  variableByName(const std::string& name) const;

    std::vector<const Namespace*> namespaces() const;
    std::vector<const Class*>     classes() const;
    std::vector<const Enum*>      enums() const;
    std::vector<const Function*>  functions() const;
    std::vector<const Variable*>  variables() const;

private:
    const tinyrefl::tool::model::File& _file;
};

} // namespace model

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_MODEL_INDEX_HPP_INCLUDED
