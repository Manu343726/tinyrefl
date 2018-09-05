#include "userapi.hpp"
#include <cassert>
#include <tinyrefl/api.hpp>
#include "userapi.hpp.tinyrefl"

using namespace userapi;

int main()
{
    auto sessionData =
        tinyrefl::from_json<EntryPoint::SessionData>(nlohmann::json::parse(R"(
            {
                "url": "https://github.com/Manu343726/tinyrefl",
                "port": 4242,
                "user": "Manu343726",
                "userCredentials": "READ",
                "tags": ["c++", "reflection", "libclang"]
            }
        )"));

    EntryPoint entryPoint{sessionData};

    assert(sessionData.url == "https://github.com/Manu343726/tinyrefl");
    assert(sessionData.port == 4242);
    assert(sessionData.user == "Manu343726");
    assert(
        sessionData.userCredentials ==
        EntryPoint::SessionData::UserCredentials::READ);
    assert(sessionData.tags.size() == 3);
    assert(sessionData.tags[0] == "c++");
    assert(sessionData.tags[1] == "reflection");
    assert(sessionData.tags[2] == "libclang");
}
