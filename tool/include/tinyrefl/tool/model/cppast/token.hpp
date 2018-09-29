#ifndef TINYREFL_TOOL_MODEL_CPPAST_TOKEN_HPP
#define TINYREFL_TOOL_MODEL_CPPAST_TOKEN_HPP

#include <cppast/cpp_token.hpp>
#include <tinyrefl/tool/model/cppast/common.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

std::string            token_spelling(const cppast::cpp_token& token);
cppast::cpp_token_kind token_kind(const cppast::cpp_token& token);
std::string            token_kind_string(const cppast::cpp_token& token);
} // namespace model

} // namespace tool

} // namespace tinyrefl

#endif // TINYREFL_TOOL_MODEL_CPPAST_TOKEN_HPP
