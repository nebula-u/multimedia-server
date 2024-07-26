#include "FileServer.h"

// Any FileServer::run()


RETURN_CODE FileServer::GetUserInfo()
{   
    std::string url = "https://openapi.alipan.com/oauth/users/info";
    std::string response = "";
    /****************HEAD LIST***************/
    std::string headAccessToken = "Authorization: Bearer " + sharedVariable.getAccessToken();

    // std::cout << headAccessToken << std::endl;
    std::vector<std::string> headers = {
        "Content-Type: application/json",
        headAccessToken
    };

    std::string json = "";
    if(Get(url, json, headers, response, WriteTextCallback))
    {
        std::cout << response << std::endl;
        return RETURN_CODE::NO_ERROR;
    }
    else
    {
        std::cerr << "获取用户信息的请求失败" << std::endl;
    }
    return RETURN_CODE::NO_ERROR;
}