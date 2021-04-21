#ifndef TINYREFL_TOOL_MODEL_UTILS_HPP
#define TINYREFL_TOOL_MODEL_UTILS_HPP

#include <model/ast.pb.h>

namespace tinyrefl
{

namespace tool
{

namespace model
{

std::vector<const tinyrefl::tool::model::Namespace*>
    namespaces(const tinyrefl::tool::model::Namespace& namespace_);
std::vector<const tinyrefl::tool::model::Function*>
    functions(const tinyrefl::tool::model::Namespace& namespace_);
std::vector<const tinyrefl::tool::model::Variable*>
    variables(const tinyrefl::tool::model::Namespace& namespace_);
std::vector<const tinyrefl::tool::model::Class*>
    classes(const tinyrefl::tool::model::Namespace& namespace_);
std::vector<const tinyrefl::tool::model::Enum*>
    enums(const tinyrefl::tool::model::Namespace& namespace_);

std::vector<const tinyrefl::tool::model::Constructor*>
    constructors(const tinyrefl::tool::model::Class& class_);
std::vector<const tinyrefl::tool::model::MemberFunction*>
    functions(const tinyrefl::tool::model::Class& class_);
std::vector<const tinyrefl::tool::model::MemberVariable*>
    variables(const tinyrefl::tool::model::Class& class_);
std::vector<const tinyrefl::tool::model::Class*>
    classes(const tinyrefl::tool::model::Class& class_);
std::vector<const tinyrefl::tool::model::Enum*>
    enums(const tinyrefl::tool::model::Class& class_);
std::vector<const tinyrefl::tool::model::Function*>
    staticFunctions(const tinyrefl::tool::model::Class& class_);
std::vector<const tinyrefl::tool::model::Variable*>
    staticVariables(const tinyrefl::tool::model::Class& class_);

} // namespace model

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_MODEL_UTILS_HPP
