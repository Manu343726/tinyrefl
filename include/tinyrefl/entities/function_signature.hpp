#ifndef TINYREFL_ENTITIES_FUNCTION_SIGNATURE_HPP_INCLUDED
#define TINYREFL_ENTITIES_FUNCTION_SIGNATURE_HPP_INCLUDED

#include <ctti/nameof.hpp>
#include <tinyrefl/entities/type.hpp>
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

    constexpr tinyrefl::entities::type<T> type() const
    {
        return {};
    }

private:
    std::size_t      _index;
    tinyrefl::string _name;
};

template<typename Names, typename Signature>
struct function_signature;

template<typename ReturnType_, typename... Names, typename... Types_>
struct function_signature<
    tinyrefl::meta::list<Names...>,
    ReturnType_(Types_...)>
{

    using ReturnType     = ReturnType_;
    using Types          = tinyrefl::meta::list<Types_...>;
    using NamesConstants = tinyrefl::meta::list<Names...>;

    constexpr function_signature() = default;

    constexpr auto arguments() const
    {
        return _arguments(std::index_sequence_for<Types_...>{});
    }

    constexpr tinyrefl::entities::type<ReturnType> returnType() const;

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

#endif // TINYREFL_ENTITIES_FUNCTION_SIGNATURE_HPP_INCLUDED
