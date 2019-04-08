#ifndef TINYREFL_UTILS_DEMANGLE_HPP
#define TINYREFL_UTILS_DEMANGLE_HPP

#include <string>
#include <tinyrefl/types/type.hpp>
#include <typeinfo>

namespace tinyrefl
{

namespace utils
{

std::string demangle(const std::string& name);

namespace impl
{
template<typename T>
struct demangle
{
    static std::string apply()
    {
        return tinyrefl::utils::demangle(typeid(T).name());
    }
};

template<typename T>
struct demangle<tinyrefl::Type<T>> : public demangle<T>
{
};
} // namespace impl

template<typename T>
std::string demangle()
{
    return tinyrefl::utils::impl::demangle<T>::apply();
}

template<typename T>
std::string demangle(tinyrefl::Type<T>)
{
    return tinyrefl::utils::demangle(typeid(T).name());
}

} // namespace utils
} // namespace tinyrefl

#endif // TINYREFL_UTILS_DEMANGLE_HPP
