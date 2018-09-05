#include <cstddef>

template<typename T>
struct constexpr_array_view
{
    constexpr constexpr_array_view(const T* begin, const T* end)
        : begin{begin}, end{end}
    {
    }

    template<std::size_t N>
    constexpr constexpr_array_view(const T (&array)[N])
        : constexpr_array_view{&array[0], &array[0] + N}
    {
    }

    const T* begin;
    const T* end;

    constexpr const T& operator[](const std::size_t i) const
    {
        return begin[i];
    }
};

constexpr constexpr_array_view<char> array{"hello"};
constexpr char                       c = array[1];
static_assert(c == 'e', "");

int main()
{
}
