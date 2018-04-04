#ifndef TINYREFL_TESTS_STATIC_TEMPLATEUSINGREFLECTION_H
#define TINYREFL_TESTS_STATIC_TEMPLATEUSINGREFLECTION_H

#include <tinyrefl/api.hpp>

namespace tinyrefl
{

namespace test
{

template<typename T>
struct AssertMetadataAvailableForTemplateParam
{
    static_assert(tinyrefl::has_metadata<T>::value, "Expected metadata available inside class template");
};

}

}

#endif // TINYREFL_TESTS_STATIC_TEMPLATEUSINGREFLECTION_H
