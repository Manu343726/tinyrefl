#include <spdlog/spdlog.h>
#include <tinyrefl/tool/model/index.hpp>

using namespace tinyrefl::tool::model;

Index::Index(const File& file) : _file(file) {}


const Namespace* Index::namespaceByName(const std::string& name) const
{
    return byName<Namespace>(name);
}

const Class* Index::classByName(const std::string& name) const
{
    return byName<Class>(name);
}

const Enum* Index::enumByName(const std::string& name) const
{
    return byName<Enum>(name);
}

const Function* Index::functionByName(const std::string& name) const
{
    return byName<Function>(name);
}

const Variable* Index::variableByName(const std::string& name) const
{
    return byName<Variable>(name);
}

std::vector<const Namespace*> Index::namespaces() const
{
    return all<Namespace>();
}

std::vector<const Class*> Index::classes() const
{
    return all<Class>();
}

std::vector<const Enum*> Index::enums() const
{
    return all<Enum>();
}

std::vector<const Function*> Index::functions() const
{
    spdlog::trace("functions");
    return all<Function>();
}

std::vector<const Variable*> Index::variables() const
{
    return all<Variable>();
}
