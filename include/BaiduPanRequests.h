#ifndef BAIDUPANREQUESTS_H
#define BAIDUPANREQUESTS_H

#include <string>
#include <vector>
#include <curl/curl.h>
#include "json.h"
#include "common.h"
#include <global.h>

class BaiduPanRequests{
public:
    std::string GetDeviceCode();
    std::string GetAccessTokenByDeviceCode(std::string deviceCode);
private:
    bool Get(const std::string &url, const std::string &json, 
          const std::vector<std::string>& headers, std::string &response, 
          size_t (*callback)(void*, size_t, size_t, void*), const std::string& fileName="");

    bool Post(const std::string &url, const std::string &json, 
          const std::vector<std::string>& headers, std::string &response, 
          size_t (*callback)(void*, size_t, size_t, void*));

};

#endif