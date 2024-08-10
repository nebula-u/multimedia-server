#include "BaiduPanRequests.h"

std::string BaiduPanRequests::GetDeviceCode()
{
    std::string url = "https://openapi.baidu.com/oauth/2.0/device/code?response_type=device_code&client_id=" + config.baidu_app_key + "&scope=basic,netdisk";
    Json::Value root;
    Json::StyledWriter sw;
    std::string json = sw.write(root);
    std::vector<std::string> headers = {
        "User-Agent: pan.baidu.com"};

    std::string response = "";
    if (Get(url, json, headers, response, WriteTextCallback))
    {
        return response;
    }
    else
    {
        std::cerr << "获取登陆二维码URL失败" << std::endl;
    }

    return "";
}

std::string BaiduPanRequests::GetAccessTokenByDeviceCode(std::string deviceCode)
{
    std::string url = "https://openapi.baidu.com/oauth/2.0/token?grant_type=device_token&code=" + deviceCode + "&client_id=" + config.baidu_app_key + "&client_secret=" + config.baidu_screte_key;

    std::string response = "";
    std::vector<std::string> headers = {
        "User-Agent: pan.baidu.com"};

    if (Get(url, "", headers, response, WriteTextCallback))
    {
        return response;
    }
    else
    {
        std::cerr << "设备码换取AccessToken的请求失败了" << std::endl;
    }

    return "";
}

std::string BaiduPanRequests::GetFileList(std::string path, std::string accessToken)
{
    path = UrlEncode(path);
    std::string url = "https://pan.baidu.com/rest/2.0/xpan/file?method=list&dir=" + path + "&order=name&start=0&limit=100&web=web&folder=0&access_token=" + accessToken + "&desc=1";
    std::string response = "";
    std::vector<std::string> headers = {
        "User-Agent: pan.baidu.com"};

    if (Get(url, "", headers, response, WriteTextCallback))
    {
        std::cout << response << std::endl;
        return response;
    }
    else
    {
        std::cerr << "获取文件列表的请求失败了" << std::endl;
    }

    return "";
}

std::string BaiduPanRequests::QueryFileInfo(std::vector<std::string> fids, std::string accessToken, bool dlink)
{
    std::string fs = "[";
    for (int i = 0; i < fids.size()-1; i++)
    {
        fs = fs + fids[i] + ",";
    }
    fs = fs + fids[fids.size()-1] + "]";
    std::string url = "http://pan.baidu.com/rest/2.0/xpan/multimedia?method=filemetas&access_token=" + accessToken + "&fsids=" + fs + "&dlink=1";

    std::cout << "MMMM: " << url << std::endl;

    std::string response = "";
    std::vector<std::string> headers = {
        "User-Agent: pan.baidu.com"};
        if (Get(url, "", headers, response, WriteTextCallback))
    {
        std::cout << response << std::endl;
        return response;
    }
    else
    {
        std::cerr << "获取文件信息的请求失败了" << std::endl;
    }

    return "";
}

bool BaiduPanRequests::Get(const std::string &url, const std::string &json,
                           const std::vector<std::string> &headers, std::string &response,
                           size_t (*callback)(void *, size_t, size_t, void *), const std::string &fileName)
{
    std::lock_guard<std::mutex> lock(mtxRequest);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    std::ofstream ofs;
    struct curl_slist *headerList = nullptr;
    for (const auto &header : headers)
    {
        headerList = curl_slist_append(headerList, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 0L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    // 根据文件名是否为空来判断响应为字符串数据还是二进制数据
    if ("" == fileName)
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
    if ("" != fileName)
        ofs.close(); // 响应是二进制数据，保存在文件中之后要关闭文件
    return (res == CURLE_OK);
}

bool BaiduPanRequests::Post(const std::string &url, const std::string &json,
                            const std::vector<std::string> &headers, std::string &response,
                            size_t (*callback)(void *, size_t, size_t, void *))
{
    std::lock_guard<std::mutex> lock(mtxRequest);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    struct curl_slist *headerList = nullptr;
    for (const auto &header : headers)
    {
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
