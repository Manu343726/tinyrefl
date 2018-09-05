#include <tinyrefl/utils/demangle.hpp>


#if defined(_MSC_VER)
#include <Dbghelp.h>

namespace tinyrefl
{
namespace utils
{

constexpr std::size_t UNDECORATED_NAME_LENGHT = 512;

std::string demangle(const std::string& name)
{
    char output_buffer[UNDECORATED_NAME_LENGHT];

    if(!FAILED(UnDecorateSymbolName(
           name.c_str(),
           output_buffer,
           UNDECORATED_NAME_LENGHT,
           UNDNAME_COMPLETE)))
    {
        return std::string(output_buffer);
    }
    else
    {
        return name;
    }
}
}
}
#elif defined(__GNUC__) || defined(__llvm__)
#include <cxxabi.h>

namespace tinyrefl
{
namespace utils
{

std::string demangle(const std::string& name)
{
    int status;

    char* demangled_name = abi::__cxa_demangle(name.c_str(), 0, 0, &status);

    if(status != 0)
        throw;

    std::string result{demangled_name};

    std::free(demangled_name); // IMPORTANT! The result of __cxa_demangle() was
                               // malloc()ed, should be released
    // using free(). See __cxa_demangle() documentation.

    return result;
}
}
}
#else /* Others (Currently unsupported platforms. The name is returned as is) \
         */
namespace tinyrefl
{
namespace utils
{

std::string demangle(const std::string& name)
{
    return name;
}
}
}
#endif
