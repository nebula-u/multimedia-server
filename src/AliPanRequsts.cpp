#include "AliPanRequests.h"

/**
 * 功能：朴素的HTTP Get函数，需要指定url、json、head、response、回调函数、文件名（响应为二进制数据时填写），仅限类内部使用
 * 作者：Nebulau
 * 日期：2024年7月27日
 */
bool AliPanRequests::Get(const std::string &url, const std::string &json, 
          const std::vector<std::string>& headers, std::string &response, 
          size_t (*callback)(void*, size_t, size_t, void*), const std::string& fileName)
{
    std::lock_guard<std::mutex> lock(mtxRequest);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    std::ofstream ofs;
    struct curl_slist* headerList = nullptr;
    for (const auto& header : headers) {
        headerList = curl_slist_append(headerList, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    // 根据文件名是否为空来判断响应为字符串数据还是二进制数据
    if("" == fileName)
    {
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    }
    else
    {
        ofs.open(fileName, std::ios::binary);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);
    }
    
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headerList);
    curl_easy_cleanup(curl);
    if("" != fileName) ofs.close(); // 响应是二进制数据，保存在文件中之后要关闭文件
    return (res == CURLE_OK);
}

/**
 * 功能：朴素的HTTP Post函数，需要指定url、json、head、response、回调函数，仅限类内部使用
 * 作者：Nebulau
 * 日期：2024年7月27日
 */
bool AliPanRequests::Post(const std::string &url, const std::string &json, 
          const std::vector<std::string>& headers, std::string &response, 
          size_t (*callback)(void*, size_t, size_t, void*))
{
    std::lock_guard<std::mutex> lock(mtxRequest);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    struct curl_slist* headerList = nullptr;
    for (const auto& header : headers) {
        headerList = curl_slist_append(headerList, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headerList);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK);
}


/**
 * 功能：获取登录二维码的链接地址及{sid}
 * 作者：Nebulau
 * 日期：2024年7月27日
 */
std::string AliPanRequests::GetAuthQRCodeSid()
{
    /******************API******************/
    std::string url = "https://openapi.alipan.com/oauth/authorize/qrcode";

    /******************JSON******************
     * {
     *      "client_id": "xxx",
     *      "client_secret": "xxx",
     *      "scopes": [
     *          "user:base"
     *      ],
     *      "width": 430,
     *      "height": 430
     * }
     ****************************************/
    Json::Value root;
    Json::StyledWriter sw;
    root["client_id"] = config.ali_client_id;
    root["client_secret"] = config.ali_client_secret;
    root["scopes"].append("user:base");         // 获取你的用户ID、头像、昵称
    root["scopes"].append("file:all:read");     // 读取云盘所有文件
    root["scopes"].append("file:all:write");    // 写入云盘所有文件
    root["scopes"].append("album:shared:read"); // 读取共享相薄文件
    root["width"] = 300;
    root["height"] = 300;
    std::string json = sw.write(root);
    /****************HEAD LIST***************/
    std::vector<std::string> headers = {
        "Content-Type: application/json"
    };

    /******************响应******************/
    std::string response = "";
    if (Post(url, json, headers, response, WriteTextCallback))
    {
        Json::Reader reader;
        Json::Value root;
        reader.parse(response, root);
        this->sid_ = root["sid"].asString();
        this->qrCodeUrl_ = root["qrCodeUrl"].asString();
        return this->qrCodeUrl_;
    }
    else
    {
        std::cerr << "获取登录二维码 {sid} 的请求失败" << std::endl;
    }
    return "";
}

/**
 * 功能：获取登录二维码
 * 作者：Nebulau
 * 日期：2024年7月27日
 */
RETURN_CODE AliPanRequests::GetAuthQRCode()
{
    /******************API******************/
    std::string url = this->qrCodeUrl_;

    /************QRCode-response************/
    std::string response = "";

    /****************HEAD LIST***************/
    std::vector<std::string> headers = {};

    std::string json = "";

    if (Get(url, json, headers, response, WriteBinaryCallback, "../QRCode.jpg"))
    {
        std::cout << "登录二维码请求成功" << std::endl;
        return RETURN_CODE::NO_ERROR;
    }
    else
    {
        std::cerr << "登录二维码请求失败" << std::endl;
    }
    return RETURN_CODE::GET_LOGIN_QR_ERROR;
}

/**
 * 功能：获取登陆状态
 * 作者：Nebulau
 * 日期：2024年7月27日
 */
RETURN_CODE AliPanRequests::WaitForLogin()
{
    std::string url = "https://openapi.alipan.com/oauth/qrcode/" + this->sid_ + "/status";
    std::string response = "";
    std::string QRLoginStatus = "";
    std::string json = "";

    /****************HEAD LIST***************/
    std::vector<std::string> headers = {};

    // 二维码有效时间为3分钟，即180秒
    for (int i = 0; i < 180; i++)
    {
        response = "";
        if (Get(url, json, headers, response, WriteTextCallback))
        {
            Json::Reader reader;
            Json::Value root;
            reader.parse(response, root);
            QRLoginStatus = root["status"].asString();
            this->authCode_ = root["authCode"].asString();
            if ("LoginSuccess" == QRLoginStatus)
            {
                std::cout << "登录成功" << std::endl;
                return RETURN_CODE::NO_ERROR;
            }
            else if ("WaitLogin" == QRLoginStatus)
            {
                std::cout << "等待登录" << std::endl;
            }
            else if ("ScanSuccess" == QRLoginStatus)
            {
                std::cout << "扫码成功" << std::endl;
            }
            else if ("QRCodeExpired" == QRLoginStatus)
            {
                std::cout << "二维码过期" << std::endl;
                break;
            }
            else
            {
                std::cerr << "未知的登录状态：" << QRLoginStatus << std::endl;
            }
        }
        else
        {
            std::cout << "登陆状态请求失败" << std::endl;
        }
        sleep(1);
    }
    return RETURN_CODE::QR_EXPIRED;
}

/**
 * 功能：获取access_token
 * 作者：Nebulau
 * 日期：2024年7月26日
 * 更改历史：
 *      - 2024年7月26日：调用Post方法，获取access_token，将access_token保存进类成员access_token_中
 */
RETURN_CODE AliPanRequests::GetAccessToken()
{
    /******************API******************/
    std::string url = "https://openapi.alipan.com/oauth/access_token";

    /******************JSON******************
     * {
     *      "client_id": "123456",
     *      "client_secret": "123456",
     *      "grant_type": "authorization_code",
     *      "code": ""
     * }
     ****************************************/
    Json::Value root;
    Json::StyledWriter sw;
    root["client_id"] = config.ali_client_id;
    root["client_secret"] = config.ali_client_secret;
    root["grant_type"] = "authorization_code";
    root["code"] = this->authCode_;
    std::string json = sw.write(root);
    std::string token_type = "";

    /****************HEAD LIST***************/
    std::vector<std::string> headers = {
        "Content-Type: application/json"};

    /******************响应******************/
    std::string response = "";
    if (Post(url, json, headers, response, WriteTextCallback))
    {
        Json::Reader reader;
        Json::Value root;
        reader.parse(response, root);
        token_type = root["token_type"].asString();
        if ("Bearer" == token_type)
        {
            sharedVariable->setAccessToken(root["access_token"].asString());
            sharedVariable->setRefreshToken(root["refresh_token"].asString());
            return RETURN_CODE::NO_ERROR;
        }
        else
        {
            std::cerr << "GetAccessToken Error: " << token_type << std::endl;
        }
    }
    else
    {
        std::cerr << "获取access_token的请求失败" << std::endl;
    }
    return RETURN_CODE::GET_ACCESS_TOKEN_ERROR;
}

/**
 * 功能：刷新access_token
 * 作者：Nebulau
 * 日期：2024年7月27日
 */
RETURN_CODE AliPanRequests::RefreshAccessToken()
{
    /******************API******************/
    std::string url = "https://openapi.alipan.com/oauth/access_token";

    /******************JSON******************
     * {
     *      "client_id": "123456",
     *      "client_secret": "123456",
     *      "grant_type": "refresh_token",
     *      "refresh_token": ""
     * }
     ****************************************/
    Json::Value root;
    Json::StyledWriter sw;
    root["client_id"] = config.ali_client_id;
    root["client_secret"] = config.ali_client_secret;
    root["grant_type"] = "refresh_token";
    root["refresh_token"] = sharedVariable->getRefreshToken();
    std::string json = sw.write(root);
    std::string token_type = "";

    /****************HEAD LIST***************/
    std::vector<std::string> headers = {
        "Content-Type: application/json; charset=UTF-8"};

    /******************响应******************/
    std::string response = "";
    if (Post(url, json, headers, response, WriteTextCallback))
    {
        Json::Reader reader;
        Json::Value root;
        reader.parse(response, root);
        token_type = root["token_type"].asString();
        if ("Bearer" == token_type)
        {
            sharedVariable->setAccessToken(root["access_token"].asString());
            sharedVariable->setRefreshToken(root["refresh_token"].asString());
            return RETURN_CODE::NO_ERROR;
        }
        else
        {
            std::cerr << "RefreshAccessToken Error: " << token_type << std::endl;
        }
    }
    else
    {
        std::cerr << "刷新access_token的请求失败" << std::endl;
    }
    return RETURN_CODE::REFRESH_ACCESS_TOKEN_ERROR;
}

/**
 * 功能：获取用户信息，包括用户ID，昵称、头像等
 * 作者：Nebualu
 * 日期：2024年7月27日
 */
RETURN_CODE AliPanRequests::GetUserInfo()
{
    std::string url = "https://openapi.alipan.com/oauth/users/info";
    std::string response = "";
    /****************HEAD LIST***************/
    std::string headAccessToken = "Authorization: Bearer " + sharedVariable->getAccessToken();

    std::vector<std::string> headers = {
        "Content-Type: application/json",
        headAccessToken
    };

    std::string json = "";
    if(Get(url, json, headers, response, WriteTextCallback))
    {
        return RETURN_CODE::NO_ERROR;
    }
    else
    {
        std::cerr << "获取用户信息的请求失败" << std::endl;
    }
    return RETURN_CODE::NO_ERROR;
}