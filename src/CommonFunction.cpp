#include "global.h"

std::mutex mtxRequest;

/*************************通用函数**************************/

/**
 * 功能：HTTP请求的回调函数：接收字符串
 * 作者：Nebulau
 * 日期：2024年7月26日
 */
size_t WriteTextCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

/**
 * 功能：HTTP请求的回调函数：接收文件
 * 作者：Nebulau
 * 日期：2024年7月26日
 */
size_t WriteBinaryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::cout << "写入文件..." << std::endl;
    std::ofstream* ofs = static_cast<std::ofstream*>(userp);
    ofs->write(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

bool Get(const std::string &url, const std::string &json, 
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

bool Post(const std::string &url, const std::string &json, 
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
    std::cout << "@@@: " << json << std::endl;
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