#include <string>
#include <fstream>

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
    std::ofstream* ofs = static_cast<std::ofstream*>(userp);
    ofs->write(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}