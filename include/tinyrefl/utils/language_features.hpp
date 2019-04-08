#ifndef TINYREFL_UTILS_LANGUAGE_FEATURES_HPP
#define TINYREFL_UTILS_LANGUAGE_FEATURES_HPP

#include <ctti/detail/language_features.hpp>

#if defined(__cpp_constexpr) and __cpp_constexpr >= 201603
#define TINYREFL_HAS_CONSTEXPR_LAMBDAS
#define TINYREFL_CONSTEXPR_LAMBDA
#else
#define TINYREFL_CONSTEXPR_LAMBDA
#endif // CONSTEXPR LAMBDA

#if defined(__cpp_concepts) and __cpp_concepts >= 201507
#define TINYREFL_HAS_CONCEPTS
#endif // CONCEPTS

#endif // TINYREFL_UTILS_LANGUAGE_FEATURES_HPP
