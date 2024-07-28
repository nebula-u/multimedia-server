#include "SharedVariable.h"

/**
 * 功能：各个共享变量的互斥访问
 * 注释：只是简单的互斥保护，使用轻锁 std::lock_guard 就行
 * 作者：Nebualu
 * 日期：2024年7月27日
 */

SharedVariable::SharedVariable()
{
    this->accessToken_ = "";
    this->refreshToken_ = "";
    this->isClientConnected_ = false;
}

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

bool SharedVariable::getClientStatus()
{
    std::lock_guard<std::mutex> lock(mtxIsClientConnected_);
    return this->isClientConnected_;
}

void SharedVariable::setClientStatus(bool status)
{
    std::lock_guard<std::mutex> lock(mtxIsClientConnected_);
    this->isClientConnected_ = status;
}
