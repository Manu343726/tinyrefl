#include "rttr.hpp"
#include "example.hpp"
#include <iostream>
#include <rttr/type>
#include "example.hpp.tinyrefl"

TINYREFL_REGISTER_RTTR_TYPES(example::C, example::Enum);

void dump_type(const std::string& name)
{
    auto type = rttr::type::get_by_name(name);

    std::cout << ":: Type \"" << type.get_name() << "\":\n";

    if(type.is_class())
    {
        for(auto& prop : type.get_properties())
        {
            std::cout << " - property: " << prop.get_name() << " ("
                      << prop.get_type().get_name() << ")\n";
        }

        for(auto& method : type.get_methods())
        {
            std::cout << " - method: " << method.get_signature() << "\n";
        }

        for(auto& constructor : type.get_constructors())
        {
            std::cout << " - constructor: " << constructor.get_signature()
                      << "\n";
        }
    }
    else if(type.is_enumeration())
    {
        auto enum_ = type.get_enumeration();

        for(auto& value : enum_.get_values())
        {
            std::cout << " - enumerated value: " << value.to_string() << " ("
                      << value.to_int() << ")\n";
        }
    }
}

int main()
{
    dump_type("example::C");
    dump_type("example::Enum");
    dump_type("example::C::Enum");
}
