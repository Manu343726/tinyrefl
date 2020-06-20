#ifndef TINYREFL_TYPES_STRING_HPP_INCLUDED
#define TINYREFL_TYPES_STRING_HPP_INCLUDED

#include <ctti/detail/cstring.hpp>
#include <ctti/name.hpp>
#include <tinyrefl/types/array.hpp>
#include <tinyrefl/types/array_view.hpp>
#include <tinyrefl/types/static_value.hpp>

#ifdef TINYREFL_HAS_STD_STRING_VIEW
#include <string_view>
#endif // TINYREFL_HAS_STD_STRING_VIEW

namespace tinyrefl
{

struct string : public ctti::detail::cstring
{
    using ctti::detail::cstring::cstring;

    constexpr string(const ctti::detail::cstring str)
        : ctti::detail::cstring{str.begin(), str.size()}
    {
    }

    operator std::string() const
    {
        return str();
    }

#ifdef TINYREFL_HAS_STD_STRING_VIEW
    operator std::string_view() const
    {
        return {begin(), size()};
    }

    constexpr string(const std::string_view str)
        : ctti::detail::cstring{str.data(), str.size()}
    {
    }
#endif // TINYREFL_HAS_STD_STRING_VIEW
};

using name_t = ctti::name_t;
using hash_t = ctti::detail::hash_t;

constexpr hash_t hash(const char* str)
{
    return tinyrefl::string{str}.hash();
}

template<typename... Chars>
constexpr hash_t hash(Chars... chars)
{
    const char string[] = {chars...};
    return tinyrefl::string{string}.hash();
}

inline hash_t hash(const std::string& str)
{
    return tinyrefl::string{&str[0], str.size()}.hash();
}

template<tinyrefl::hash_t Hash>
using hash_constant = tinyrefl::static_value<tinyrefl::hash_t, Hash>;

template<tinyrefl::hash_t Hash>
constexpr hash_constant<Hash> hash_constant_v{};

namespace literals
{

template<typename Char, Char... Chars>
constexpr tinyrefl::hash_constant<tinyrefl::hash(Chars..., '\0')>
    operator""_id()
{
    return {};
}

} // namespace literals

constexpr tinyrefl::string default_string_constant =
    "default tinyrefl string constant";

template<typename Hash>
constexpr tinyrefl::string string_constant()
{
    return default_string_constant;
}

template<typename Hash>
struct hash_to_string
{
    static constexpr tinyrefl::string value = tinyrefl::string_constant<Hash>();
};

template<typename Hash>
constexpr tinyrefl::string hash_to_string<Hash>::value;


template<typename List>
struct typelist_to_array_of_strings
{
    static constexpr tinyrefl::array_view<tinyrefl::string> value =
        tinyrefl::typelist_to_array<
            List,
            tinyrefl::string,
            tinyrefl::meta::defer<hash_to_string>>::value;
};

template<typename List>
constexpr tinyrefl::array_view<tinyrefl::string>
    typelist_to_array_of_strings<List>::value;
} // namespace tinyrefl

#endif // TINYREFL_TYPES_STRING_HPP_INCLUDED
