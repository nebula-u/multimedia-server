#include "SharedVariable.h"

std::string SharedVariable::getAccessToken()
{
    std::lock_guard<std::mutex> lock(mtxAccessToken_);
    return this->accessToken_;
}

void SharedVariable::setAccessToken(std::string accessToken)
{
    std::lock_guard<std::mutex> lock(mtxAccessToken_);
    this->accessToken_ = accessToken;
}

std::string SharedVariable::getRefreshToken()
{
    std::lock_guard<std::mutex> lock(mtxRefreshToken_);
    return this->refreshToken_;
}

void SharedVariable::setRefreshToken(std::string refreshToken)
{
    std::lock_guard<std::mutex> lock(mtxRefreshToken_);
    this->refreshToken_ = refreshToken;
}