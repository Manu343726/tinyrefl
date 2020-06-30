#ifndef TINYREFL_BACKEND_REGISTER_ENTITY_MACROS_HPP_INCLUDED
#define TINYREFL_BACKEND_REGISTER_ENTITY_MACROS_HPP_INCLUDED

#include <tinyrefl/backend/metadata.hpp>
#include <tinyrefl/types/string.hpp>
#include <tinyrefl/utils/preprocessor.hpp>

#define TINYREFL_REGISTER_ENTITY_BY_FULL_DISPLAY_NAME(...)                     \
    namespace tinyrefl                                                         \
    {                                                                          \
    namespace backend                                                          \
    {                                                                          \
    template<>                                                                 \
    struct entity_metadata<                                                    \
        ::tinyrefl::hash_constant<__VA_ARGS__().full_display_name().hash()>>   \
    {                                                                          \
        using type = __VA_ARGS__;                                              \
    };                                                                         \
    } /* namespace backend */                                                  \
    } // namespace tinyrefl

#define TINYREFL_REGISTER_ENTITY_BY_TYPE(key, ...)                             \
    namespace tinyrefl                                                         \
    {                                                                          \
    namespace backend                                                          \
    {                                                                          \
    template<>                                                                 \
    struct entity_metadata<typename __VA_ARGS__::key>                          \
    {                                                                          \
        using type = __VA_ARGS__;                                              \
    };                                                                         \
    } /* namespace backend */                                                  \
    } /* namespace tinyrefl */                                                 \
                                                                               \
    TINYREFL_REGISTER_ENTITY_BY_FULL_DISPLAY_NAME(__VA_ARGS__)

#define TINYREFL_REGISTER_ENTITY_BY_POINTER(entity)                            \
    TINYREFL_REGISTER_ENTITY_BY_TYPE(                                          \
        pointer_static_value, TINYREFL_PP_UNWRAP entity)

#define TINYREFL_REGISTER_ENTITY_BY_LOCATION(entity)                           \
    TINYREFL_REGISTER_ENTITY_BY_TYPE(SourceLocation, TINYREFL_PP_UNWRAP entity)

#endif // TINYREFL_BACKEND_REGISTER_ENTITY_MACROS_HPP_INCLUDED
