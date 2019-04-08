#ifndef TINYREFL_TYPES_ARRAY_VIEW_HPP_INCLUDED
#define TINYREFL_TYPES_ARRAY_VIEW_HPP_INCLUDED

#include <cstdint>
#include <tinyrefl/types/array.hpp>
#include <tinyrefl/utils/algorithm.hpp>

namespace tinyrefl
{

template<typename T>
struct array_view
{
    constexpr array_view(const T* begin, const T* end)
        : _begin{begin}, _end{end}
    {
    }

    template<std::size_t N>
    constexpr array_view(const T (&array)[N])
        : array_view{ctti::detail::begin(array), ctti::detail::end(array)}
    {
    }

    template<std::size_t N>
    constexpr array_view(
        const tinyrefl::array<std::remove_const_t<T>, N>& array)
        : array_view{array.begin(), array.end()}
    {
    }

    template<std::size_t N>
    constexpr array_view(const tinyrefl::array<const T, N>& array)
        : array_view{array.begin(), array.end()}
    {
    }

    constexpr std::size_t size() const
    {
        return static_cast<std::size_t>(_end - _begin);
    }

    constexpr const T* begin() const
    {
        return _begin;
    }

    constexpr const T* end() const
    {
        return _end;
    }

    constexpr array_view operator()(std::size_t begin, std::size_t end) const
    {
        return {_begin + begin, _begin + end};
    }

    constexpr array_view trim(std::size_t n) const
    {
        return (*this)(0, size() - n);
    }

    constexpr const T& operator[](std::size_t i) const
    {
        return _begin[i];
    }

private:
    const T* _begin;
    const T* _end;
};

template<typename T, std::size_t N>
constexpr array_view<T> make_array_view(const T (&array)[N])
{
    return {array};
}

template<typename T, std::size_t N>
constexpr tinyrefl::array_view<T>
    make_array_view(const tinyrefl::array<T, N>& array)
{
    return {array};
}

template<typename T, typename U>
constexpr const T* find(const T* begin, const T* end, const U& value)
{
    return begin != end ? (*begin == value ? begin : find(++begin, end, value))
                        : end;
}

template<typename T, typename U>
constexpr const T* find(array_view<T> range, const U& value)
{
    return find(range.begin(), range.end(), value);
}

template<typename T, std::size_t N, typename U>
constexpr const T* find(const array<T, N>& array, const U& value)
{
    return find(make_array_view(array), value);
}
} // namespace tinyrefl

#endif // TINYREFL_TYPES_ARRAY_VIEW_HPP_INCLUDED
