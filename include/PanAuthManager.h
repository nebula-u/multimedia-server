#ifndef PANAUTHMANAGER_H
#define PANAUTHMANAGER_H

#include <unistd.h>
#include <curl/curl.h>
#include "threadpool.h"
#include "json.h"
#include "global.h"

class PanAuthManager : public Task
{
public:
    Any run()
    {
        while (true)
        {
            //这里提醒用户需要授权 
            GetAuthQRCodeSid();
            GetAuthQRCode();
            WaitForLogin();
            GetAccessToken();
            while (true)
            {
                sleep(7000);
                std::cout << "即将刷新access_token" << std::endl;
                if(RETURN_CODE::NO_ERROR != RefreshAccessToken()) break;
                std::cout << "access_token刷新成功" << std::endl;
            }
        }
        return 0;
    }

private:
    /**
     * 功能：发送 POST 请求
     * 作者：Nebulau
     * 日期：2024年7月25日
     * 输入参数：
     *      - url： 请求API
     *      - data： 请求json
     * 输出参数：
     *      - response： 请求响应
     * 返回值：
     *      - true：请求成功
     *      - false：请求失败
     * 修改历史：
     *      - 2024年7月25日：创建函数
     *      - 2024年7月26日：该函数已经废弃，替代函数见 common.h 中Post函数
     */
#if 0
    bool Post(const std::string &url, const std::string &data, std::string &response)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_ = curl_easy_init();
        // 设置请求参数
        struct curl_slist *header_list = NULL;
        header_list = curl_slist_append(header_list, "Content-Type: application/json; charset=UTF-8");
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_list);
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_POST, 1L);
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl_);
        curl_easy_cleanup(curl_);
        return (res == CURLE_OK);
    }
#endif

#if 0
    bool Get(const std::string& url, FILE* fp) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_ = curl_easy_init();
        // 设置请求参数
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_POST, 0L);
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallbackImg);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, fp); 

        CURLcode res = curl_easy_perform(curl_);
        curl_easy_cleanup(curl_);
        return (res == CURLE_OK);
    }
#endif

#if 0
    bool Get(const std::string& url, std::string &response) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_ = curl_easy_init();
        // 设置请求参数
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_POST, 0L);
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &WriteCallback);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response); 

        CURLcode res = curl_easy_perform(curl_);
        curl_easy_cleanup(curl_);
        return (res == CURLE_OK);
    }
#endif

    RETURN_CODE GetAuthQRCodeSid()
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
        root["client_id"] = config.client_id;
        root["client_secret"] = config.client_secret;
        root["scopes"].append("user:base");             // 获取你的用户ID、头像、昵称
        root["scopes"].append("file:all:read");         // 读取云盘所有文件
        root["scopes"].append("file:all:write");        // 写入云盘所有文件
        root["scopes"].append("album:shared:read");     // 读取共享相薄文件
        root["width"] = 400;
        root["height"] = 400;
        std::string json = sw.write(root);
        std::cout << json << std::endl;
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
            std::cout << response << std::endl;
            return RETURN_CODE::NO_ERROR;
        }
        else
        {
            std::cerr << "获取登录二维码 {sid} 的请求失败" << std::endl;
        }
        return RETURN_CODE::GET_LOGIN_QR_SID_ERROR;
    }

    RETURN_CODE GetAuthQRCode()
    {
        /******************API******************/
        std::string url = this->qrCodeUrl_;

        /************QRCode-response************/
        // FILE* fp = fopen("../QRCode.jpg", "wb");
        std::string response = "";

        /****************HEAD LIST***************/
        std::vector<std::string> headers = {};

        std::string json = "";
        
        if(Get(url, json, headers, response, WriteBinaryCallback, "../QRCode.jpg"))
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
     * 功能：获取到登录二维码之后，等待用户登录
     * 作者：Nebulau
     * 日期：2024年7月26日
     * 更改历史：
     *      - 2024年7月26日：每隔一秒钟调用一次Get方法，更新登录状态
     *      - 2024年7月26日：修改函数名为WaitForLogin，
     */
    RETURN_CODE WaitForLogin()
    {
        std::string url = "https://openapi.alipan.com/oauth/qrcode/" + this->sid_ + "/status";
        std::string response = "";
        std::string QRLoginStatus="";
        std::string json = "";

        /****************HEAD LIST***************/
        std::vector<std::string> headers = {};

        // 二维码有效时间为3分钟，即180秒
        for(int i = 0; i < 180; i++)
        {
            response = "";
            if(Get(url, json, headers, response, WriteTextCallback))
            {
                Json::Reader reader;
                Json::Value root;
                reader.parse(response, root);
                QRLoginStatus = root["status"].asString();
                this->authCode_ = root["authCode"].asString();
                if("LoginSuccess" == QRLoginStatus)
                {
                    std::cout << "登录成功" << std::endl;
                    return RETURN_CODE::NO_ERROR;
                }
                else if("WaitLogin" == QRLoginStatus)
                {
                    std::cout << "等待登录" << std::endl;
                }
                else if("ScanSuccess" == QRLoginStatus)
                {
                    std::cout << "扫码成功" << std::endl;
                }
                else if("QRCodeExpired" == QRLoginStatus)
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
    RETURN_CODE GetAccessToken()
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
        root["client_id"] = config.client_id;
        root["client_secret"] = config.client_secret;
        root["grant_type"] = "authorization_code";
        root["code"] = this->authCode_;
        std::string json = sw.write(root);
        std::string token_type="";

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
            token_type=root["token_type"].asString();
            if("Bearer" == token_type)
            {
                sharedVariable.setAccessToken(root["access_token"].asString());
                sharedVariable.setRefreshToken(root["refresh_token"].asString());
                return RETURN_CODE::NO_ERROR;
            }
            else
            {
                std::cout << response << std::endl;
                std::cerr << "GetAccessToken Error: " << token_type << std::endl;
            }
        }
        else
        {
            std::cerr << "获取access_token的请求失败" << std::endl;
        }
        return RETURN_CODE::GET_ACCESS_TOKEN_ERROR;
    }

    RETURN_CODE RefreshAccessToken()
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
        root["client_id"] = config.client_id;
        root["client_secret"] = config.client_secret;
        root["grant_type"] = "refresh_token";
        root["refresh_token"] = sharedVariable.getRefreshToken();
        std::string json = sw.write(root);
        std::string token_type="";

        /****************HEAD LIST***************/
        std::vector<std::string> headers = {
            "Content-Type: application/json; charset=UTF-8"
        };

        /******************响应******************/
        std::string response = "";
        if (Post(url, json, headers, response, WriteTextCallback))
        {
            Json::Reader reader;
            Json::Value root;
            reader.parse(response, root);
            token_type=root["token_type"].asString();
            if("Bearer" == token_type)
            {
                sharedVariable.setAccessToken(root["access_token"].asString());
                sharedVariable.setRefreshToken(root["refresh_token"].asString());
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

private:
    std::string sid_;
    std::string qrCodeUrl_;
    std::string authCode_;
};

#endif  //PANAUTHMANAGER_H