#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include "threadpool.h"
#include "json.h"

typedef struct
{
    std::string appid = "";
    std::string appsecrete = "";
    std::string code = "";
    std::string grant_type = "";
} Config;

Config config = {""};

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

bool Post(const std::string &url, const std::string &data, std::string &response)
{
    CURL *curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    // 设置请求参数
    struct curl_slist *header_list = NULL;
    header_list = curl_slist_append(header_list, "Content-Type: application/json; charset=UTF-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    return (res == CURLE_OK);
}

// https://openapi.alipan.com/oauth/access_toke?client_id=xxxxxx&client_secret=xxxxx&grant_type=authorization_code&code=xxx
class CurlRequest : public Task
{
public:
    Any run()
    {
        {
            /******************API******************/
            std::string url = "https://openapi.alipan.com/oauth/access_token";

            
            /******************JSON******************
             * {   
             *  "client_id": "123456",    
             *  "client_secret": "123456",     
             *  "grant_type": "authorization_code",   
             *  "code": "2567ff3a817b438185b61a5e52ccfc8b"
             * }
             ****************************************/
            std::string data = "{\"client_id\": \""
                             + config.appid
                             + "\", \"client_secret\": \""
                             + config.appsecrete
                             + "\", \"grant_type\": \""
                             + config.grant_type
                             + "\", \"code\": \""
                             + config.code
                             + "\"}";
            std::cout << data << std::endl;
            std::string response = "";
            if (Post(url, data, response))
            {
                std::cout << response << std::endl;
            }
            else
            {
                std::cerr << "post error" << std::endl;
            }
        }

        return 0;
    }
};

int main()
{
    std::ifstream fconfig;
    fconfig.open("../private/config.json");
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(fconfig, root, false))
    {
        std::cerr << "配置加载出错，程序将退出" << std::endl;
        return 1;
    }

    std::cout << root["appid"].asString() << std::endl;
    std::cout << root["appsecrete"].asString() << std::endl;
    config.appid = root["appid"].asString();
    config.appsecrete = root["appsecrete"].asString();
    config.grant_type = root["grant_type"].asString();
    config.code = root["code"].asString();

    ThreadPool *pool = new ThreadPool();
    pool->start(1);
    pool->submitTask(std::make_shared<CurlRequest>());

    getchar();
    return 0;
}
