#include <tinyrefl/types/string.hpp>

#include <fstream>
#include <iostream>

std::string search_string(std::ifstream& is, const tinyrefl::hash_t hash)
{
    for(std::string line; std::getline(is, line);)
    {
        constexpr const tinyrefl::string prefix =
            "    TINYREFL_REGISTER_STRING(";
        const auto prefix_index = line.find(prefix.begin());

        if(prefix_index == 0)
        {
            const auto string_literal =
                line.substr(prefix.size(), line.size() - prefix.size() - 1);
            const auto string_literal_hash = tinyrefl::hash(string_literal);

            std::cout << string_literal << " <-> " << string_literal_hash
                      << "\n";

            if(hash == string_literal_hash)
            {
                return string_literal;
            }
        }
    }

    return "";
}

void print_usage()
{
    std::cerr << "Usage: string-debugger <string literal>\n"
                 "       string-debugger <.tinyrefl file> <hash number>\n";
}

bool is_number(const char* string)
{
    while(*string != '\0')
    {
        if(!std::isdigit(*string))
        {
            return false;
        }

        ++string;
    }

    return true;
}

int main(int argc, const char** argv)
{
    if(argc <= 1)
    {
        print_usage();
        return EXIT_FAILURE;
    }
    else if(argc == 3 && is_number(argv[2]))
    {
        std::ifstream file{argv[1]};

        if(!file)
        {
            std::cerr << "error opening file \"" << argv[1] << "\"\n";
            return EXIT_FAILURE;
        }

        std::cout << search_string(
                         file,
                         static_cast<tinyrefl::hash_t>(std::atoll(argv[2])))
                  << "\n";
    }
    else if(argc == 2)
    {
        std::cout << tinyrefl::hash(argv[1]) << "\n";
    }
}
