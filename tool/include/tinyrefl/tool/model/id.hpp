#ifndef TINYREFL_TOOL_MODEL_ID_HPP
#define TINYREFL_TOOL_MODEL_ID_HPP

namespace tinyrefl
{

namespace tool
{

namespace model
{
struct id
{
    id(std::string full_name);

    [[tinyrefl::tool::property]] const std::string& full_name() const;
    [[tinyrefl::tool::property]] std::size_t        hash() const;
    std::size_t operator std::size_t() const;

private:
    std::size_t _hash;
    std::string _full_name;
};
}
}
}

namespace std
{

template<>
struct hash<tinyrefl::tool::model::id>
{
    std::size_t operator()(const tinyrefl::tool::model::id& id) const;
};
}

#endif // TINYREFL_TOOL_MODEL_ID_HPP
