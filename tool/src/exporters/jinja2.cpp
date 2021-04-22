#include <functional>
#include <jinja2cpp/template.h>
#include <jinja2cpp/user_callable.h>
#include <tinyrefl/tool/exporters/jinja2.hpp>
#include <tinyrefl/tool/exporters/jinja2/reflection.hpp>
#include <tinyrefl/tool/model/index.hpp>

using namespace tinyrefl::tool::exporters;
using namespace tinyrefl::tool::model;
using namespace std::placeholders;

template<typename R>
auto makeCallable(const Index& index, R (Index::*method)() const)
{
    return jinja2::MakeCallable(
        [&index, method] { return jinja2::Reflect((index.*method)()); });
}

template<typename R>
auto makeCallable(
    const Index& index, R (Index::*method)(const std::string&) const)
{
    return jinja2::MakeCallable(
        [&index, method](const std::string& name) {
            return jinja2::Reflect((index.*method)(name));
        },
        jinja2::ArgInfo{"name"});
}

type_safe::optional<std::string>
    Jinja2::Export(const tinyrefl::tool::model::File& file)
{
    Index            index{file};
    jinja2::Template template_;
    template_.LoadFromFile("tinyrefl.jinja2");
    const auto result = template_.RenderAsString(
        {{"file", jinja2::Reflect(file)},
         {"namespace", makeCallable(index, &Index::namespaceByName)},
         {"class", makeCallable(index, &Index::classByName)},
         {"enum", makeCallable(index, &Index::enumByName)},
         {"function", makeCallable(index, &Index::functionByName)},
         {"variable", makeCallable(index, &Index::variableByName)},
         {"namespaces", makeCallable(index, &Index::namespaces)},
         {"classes", makeCallable(index, &Index::classes)},
         {"enums", makeCallable(index, &Index::enums)},
         {"functions", makeCallable(index, &Index::functions)},
         {"variables", makeCallable(index, &Index::variables)}});

    if(result)
    {
        return result.value();
    }
    else
    {
        reporter().error(
            "jinja2 exporter",
            tinyrefl::tool::sourceRange(
                result.error().GetErrorLocation().fileName,
                result.error().GetErrorLocation().line,
                result.error().GetErrorLocation().col))(
            "{}", result.error().ToString());
        return type_safe::nullopt;
    }
}
