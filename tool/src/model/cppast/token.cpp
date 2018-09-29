#include <tinyrefl/tool/detail/assert.hpp>
#include <tinyrefl/tool/model/cppast/token.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

std::string token_spelling(const cppast::cpp_token& token)
{
    return token.spelling;
}

cppast::cpp_token_kind token_kind(const cppast::cpp_token& token)
{
    return token.kind;
}

std::string token_kind_string(const cppast::cpp_token& token)
{
    switch(token_kind(token))
    {
    case cppast::cpp_token_kind::char_literal:
        return "char literal";
    case cppast::cpp_token_kind::float_literal:
        return "float literal";
    case cppast::cpp_token_kind::identifier:
        return "identifier";
    case cppast::cpp_token_kind::int_literal:
        return "int literal";
    case cppast::cpp_token_kind::keyword:
        return "keyword";
    case cppast::cpp_token_kind::punctuation:
        return "punctuation";
    case cppast::cpp_token_kind::string_literal:
        return "string literal";
    }

    DEBUG_UNREACHABLE(tinyrefl::tool::detail::assert_handler{});
    return "";
}

} // namespace model

} // namespace tool

} // namespace tinyrefl
