#include <jinja2cpp/template.h>
#include <tinyrefl/tool/exporters/jinja2.hpp>
#include <tinyrefl/tool/exporters/jinja2/reflection.hpp>

using namespace tinyrefl::tool::exporters;
using namespace tinyrefl::tool::model;

type_safe::optional<std::string>
    Jinja2::Export(const tinyrefl::tool::model::File& file)
{
    jinja2::Template template_;
    template_.LoadFromFile("tinyrefl.jinja2");
    const auto result =
        template_.RenderAsString({{"file", jinja2::Reflect(file)}});

    if(result)
    {
        return result.value();
    }
    else
    {
        reporter().error(result.error().ToString());
        return type_safe::nullopt;
    }
}
