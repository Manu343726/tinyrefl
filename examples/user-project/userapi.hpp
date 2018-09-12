#ifndef USERAPI_H
#define USERAPI_H

#include <string>
#include <vector>

namespace userapi
{

class EntryPoint
{
public:
    struct SessionData
    {
        enum class UserCredentials
        {
            READ  = 1,
            WRITE = 2
        };

        std::string              url;
        int                      port;
        std::string              user;
        UserCredentials          userCredentials;
        std::vector<std::string> tags;

        std::string to_string() const;
        bool        operator==(const SessionData& other) const;
        bool        operator!=(const SessionData& other) const;
    };

    EntryPoint(const SessionData& sessionData);

    const SessionData& sessionData() const;

private:
    SessionData _sessionData;
};
}

#endif // USERAPI_H
