#ifndef CURLREQUEST_H
#define CURLREQUEST_H

#include <curl/curl.h>
#include "threadpool.h"
#include "json.h"
#include "global.h"

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

size_t WriteCallbackImg(void* contents, size_t size, size_t nmemb, void* userp) {
    // 这里可以将接收到的数据写入文件或处理内存缓冲区
    std::cout << "开始写入二维码" << std::endl;
    size_t written = fwrite(contents, size, nmemb, static_cast<FILE*>(userp));
    return written;
}

class CurlRequest : public Task
{
public:
    Any run()
    {
        GetAuthQRCodeSid();
        GetAuthQRCode();
        GetQRCodeLoginStatus();
        GetAccessToken();
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
     */
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
        std::string data = sw.write(root);
        std::cout << data << std::endl;

        /******************响应******************/
        std::string response = "";
        if (Post(url, data, response))
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
            std::cerr << "获取登录二维码 {sid} 请求失败" << std::endl;
        }
        return RETURN_CODE::GET_LOGIN_QR_SID_ERROR;
    }

    RETURN_CODE GetAuthQRCode()
    {
        /******************API******************/
        std::string url = this->qrCodeUrl_;

        /************QRCode-response************/
        FILE* fp = fopen("../QRCode.jpg", "wb");
        
        if(Get(url, fp))
        {
            fclose(fp);
            std::cout << "登录二维码请求成功" << std::endl;
            return RETURN_CODE::NO_ERROR;
        }
        else
        {
            std::cerr << "登录二维码请求失败" << std::endl;
        }
        return RETURN_CODE::GET_LOGIN_QR_ERROR;
    }

    RETURN_CODE GetQRCodeLoginStatus()
    {
        // API： GET 域名 + /oauth/qrcode/{sid}/status
        std::string url = "https://openapi.alipan.com/oauth/qrcode/" + this->sid_ + "/status";
        std::string response = "";
        std::string QRLoginStatus="";
        while ("LoginSuccess" != QRLoginStatus)
        {
            response = "";
            if(Get(url, response))
            {
                Json::Reader reader;
                Json::Value root;
                reader.parse(response, root);
                QRLoginStatus = root["status"].asString();
                this->authCode_ = root["authCode"].asString();
                std::cout << "登陆状态为：" << QRLoginStatus << std::endl;
                std::cout << response << std::endl;
            }
            else
            {
                std::cout << "登陆状态请求失败" << std::endl;
            }
            sleep(1);
        }
        return RETURN_CODE::NO_ERROR;
    }

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
        std::string data = sw.write(root);

        /******************响应******************/
        std::string response = "";
        if (Post(url, data, response))
        {
            Json::Reader reader;
            Json::Value root;
            reader.parse(response, root);
            this->access_token_ = root["access_token"].asString();
            std::cout << "access_token: " << this->access_token_ << std::endl;
            return RETURN_CODE::NO_ERROR;
        }
        else
        {
            std::cerr << "请求失败" << std::endl;
        }
        return RETURN_CODE::GET_ACCESS_TOKEN_ERROR;
    }

private:
    CURL *curl_;
    std::string sid_;
    std::string qrCodeUrl_;
    std::string authCode_;
    std::string access_token_;
};

#endif