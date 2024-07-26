#ifndef SHAREDVARIABLE_H
#define SHAREDVARIABLE_H

#include <string>
#include <mutex>

class SharedVariable
{
public:
    std::string getAccessToken();
    void setAccessToken(std::string accessToken);

    std::string getRefreshToken();
    void setRefreshToken(std::string accessToken);

private:
    std::string accessToken_;
    std::mutex mtxAccessToken_;

    std::string refreshToken_;
    std::mutex mtxRefreshToken_;
};

#endif