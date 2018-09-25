#include <tinyrefl/tool/model/cppast/common.hpp>

namespace tinyrefl
{

namespace tool
{

namespace model
{

bool has_complete_name(const cppast::cpp_entity& entity)
{
    if(entity.parent().has_value())
    {
        return !entity.name().empty();
    }
    else
    {
        return !entity.name().empty() &&
               has_complete_name(entity.parent().value());
    }
}

std::string full_qualified_name(const cppast::cpp_entity& entity)
{
    if(entity.parent().has_value())
    {
        return entity.name();
    }
    else
    {
        return full_qualified_name(entity.parent().value()) +
               "::" + entity.name();
    }
}
}
}
}
