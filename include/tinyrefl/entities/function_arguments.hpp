#ifndef TINYREFL_ENTITIES_FUNCTION_ARGUMENTS_HPP_INCLUDED
#define TINYREFL_ENTITIES_FUNCTION_ARGUMENTS_HPP_INCLUDED

#include <ctti/nameof.hpp>
#include <tinyrefl/types/array.hpp>
#include <tinyrefl/types/array_view.hpp>
#include <tinyrefl/types/string.hpp>
#include <tinyrefl/types/type.hpp>
#include <tinyrefl/utils/meta.hpp>

namespace tinyrefl
{

namespace entities
{

template<typename T>
struct function_argument
{
    constexpr function_argument(const std::size_t index, tinyrefl::string name)
        : _index{index}, _name{name}
    {
    }

    constexpr std::size_t index() const
    {
        return _index;
    }

    constexpr tinyrefl::string name() const
    {
        return _name;
    }

    constexpr tinyrefl::Type<T> type() const
    {
        return {};
    }

    constexpr tinyrefl::string type_name() const
    {
        return ctti::nameof<T>();
    }

private:
    std::size_t      _index;
    tinyrefl::string _name;
};

template<typename Names, typename Types>
struct function_arguments;

template<typename... Names, typename... Types_>
struct function_arguments<
    tinyrefl::meta::list<Names...>,
    tinyrefl::meta::list<Types_...>>
{
    using Types          = tinyrefl::meta::list<Types_...>;
    using NamesConstants = tinyrefl::meta::list<Names...>;
    using NamesToTypesMap =
        tinyrefl::meta::slow_map<tinyrefl::meta::pair<Names, Types>...>;

    template<tinyrefl::hash_t Id>
    using argument_type =
        tinyrefl::meta::at_key<NamesToTypesMap, tinyrefl::hash_constant<Id>>;

    constexpr function_arguments() = default;

    constexpr tinyrefl::string operator[](const std::size_t index) const
    {
        return names()[index];
    }

    template<tinyrefl::hash_t Id>
    constexpr tinyrefl::Type<argument_type<Id>>
        operator[](tinyrefl::hash_constant<Id>) const
    {
        return {};
    }

    constexpr auto types() const
    {
        return tinyrefl::meta::make_tuple(
            tinyrefl::meta::list<tinyrefl::Type<Types_>...>{});
    }

    constexpr tinyrefl::array_view<tinyrefl::string> names() const
    {
        return tinyrefl::typelist_to_array_of_strings<
            tinyrefl::meta::list<Names...>>::value;
    }

    constexpr auto arguments() const
    {
        return _arguments(std::index_sequence_for<Types_...>{});
    }

    constexpr const tinyrefl::string* begin() const
    {
        return names().begin();
    }

    constexpr const tinyrefl::string* end() const
    {
        return names().end();
    }

    constexpr std::size_t size() const
    {
        return sizeof...(Names);
    }

private:
    template<std::size_t... Indices>
    constexpr auto _arguments(std::index_sequence<Indices...>) const
    {
        return std::make_tuple(tinyrefl::entities::function_argument<Types_>{
            Indices, tinyrefl::string_constant<Names>()}...);
    }
};

} // namespace entities
} // namespace tinyrefl

#endif // TINYREFL_ENTITIES_FUNCTION_ARGUMENTS_HPP_INCLUDED
