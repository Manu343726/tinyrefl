#include <algorithm>
#include <iterator>
#include <tinyrefl/tool/model/utils.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

std::vector<const tinyrefl::tool::model::Namespace*>
    namespaces(const tinyrefl::tool::model::Namespace& namespace_)
{
    std::vector<const Namespace*> result;

    for(const auto& member : namespace_.members())
    {
        if(member.has_namespace_())
        {
            result.push_back(&member.namespace_());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::Function*>
    functions(const tinyrefl::tool::model::Namespace& namespace_)
{
    std::vector<const Function*> result;

    for(const auto& member : namespace_.members())
    {
        if(member.has_function())
        {
            result.push_back(&member.function());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::Variable*>
    variables(const tinyrefl::tool::model::Namespace& namespace_)
{
    std::vector<const Variable*> result;

    for(const auto& member : namespace_.members())
    {
        if(member.has_variable())
        {
            result.push_back(&member.variable());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::Class*>
    classes(const tinyrefl::tool::model::Namespace& namespace_)
{
    std::vector<const Class*> result;

    for(const auto& member : namespace_.members())
    {
        if(member.has_class_())
        {
            result.push_back(&member.class_());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::Enum*>
    enums(const tinyrefl::tool::model::Namespace& namespace_)
{
    std::vector<const Enum*> result;

    for(const auto& member : namespace_.members())
    {
        if(member.has_enum_())
        {
            result.push_back(&member.enum_());
        }
    }

    return result;
}


std::vector<const tinyrefl::tool::model::Constructor*>
    constructors(const tinyrefl::tool::model::Class& class_)
{
    std::vector<const Constructor*> result;

    for(const auto& member : class_.members())
    {
        if(member.has_constructor())
        {
            result.push_back(&member.constructor());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::MemberFunction*>
    functions(const tinyrefl::tool::model::Class& class_)
{
    std::vector<const MemberFunction*> result;

    for(const auto& member : class_.members())
    {
        if(member.has_function())
        {
            result.push_back(&member.function());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::MemberVariable*>
    variables(const tinyrefl::tool::model::Class& class_)
{
    std::vector<const MemberVariable*> result;

    for(const auto& member : class_.members())
    {
        if(member.has_variable())
        {
            result.push_back(&member.variable());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::Function*>
    staticFunctions(const tinyrefl::tool::model::Class& class_)
{
    std::vector<const Function*> result;

    for(const auto& member : class_.members())
    {
        if(member.has_staticfunction())
        {
            result.push_back(&member.staticfunction());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::Variable*>
    staticVariables(const tinyrefl::tool::model::Class& class_)
{
    std::vector<const Variable*> result;

    for(const auto& member : class_.members())
    {
        if(member.has_staticvariable())
        {
            result.push_back(&member.staticvariable());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::Class*>
    classes(const tinyrefl::tool::model::Class& class_)
{
    std::vector<const Class*> result;

    for(const auto& member : class_.members())
    {
        if(member.has_class_())
        {
            result.push_back(&member.class_());
        }
    }

    return result;
}

std::vector<const tinyrefl::tool::model::Enum*>
    enums(const tinyrefl::tool::model::Class& class_)
{
    std::vector<const Enum*> result;

    for(const auto& member : class_.members())
    {
        if(member.has_enum_())
        {
            result.push_back(&member.enum_());
        }
    }

    return result;
}

} // namespace model

} // namespace tool

} // namespace tinyrefl
