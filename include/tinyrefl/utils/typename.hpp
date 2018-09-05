#ifndef TINYREFL_UTILS_TYPENAME_HPP
#define TINYREFL_UTILS_TYPENAME_HPP

#include <ctti/detail/language_features.hpp>

#ifdef CTTI_HAS_CONSTEXPR_PRETTY_FUNCTION
#include <ctti/nameof.hpp>

namespace tinyrefl
{

namespace utils
{

template<typename T>
const std::string& type_name()
{
    static std::string name{ctti::nameof<T>().str()};
    return name;
}
}
}

#else
#include <tinyrefl/utils/demangle.hpp>
#include <type_info>

namespace tinyrefl
{

namespace utils
{

template<typename T>
const std::string& type_name()
{
    static std::string name{tinyrefl::utils::demangle(typeid(T).name())};
    return name;
}
}
}
#endif // CTTI_HAS_CONSTEXPR_PRETTY_FUNCTION

#endif // TINYREFL_UTILS_TYPENAME_HPP
