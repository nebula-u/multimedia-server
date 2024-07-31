#ifndef PANEQUESTS_H
#define PANEQUESTS_H

#include <unistd.h>
#include <curl/curl.h>
#include "threadpool.h"
#include "json.h"
#include "common.h"
#include "global.h"

class PanRequests
{
public:

    std::string GetAuthQRCodeSid();

    RETURN_CODE GetAuthQRCode();

    RETURN_CODE WaitForLogin();

    RETURN_CODE GetAccessToken();

    RETURN_CODE RefreshAccessToken();

    RETURN_CODE GetUserInfo();

private:
    bool Get(const std::string &url, const std::string &json, 
          const std::vector<std::string>& headers, std::string &response, 
          size_t (*callback)(void*, size_t, size_t, void*), const std::string& fileName="");

    bool Post(const std::string &url, const std::string &json, 
          const std::vector<std::string>& headers, std::string &response, 
          size_t (*callback)(void*, size_t, size_t, void*));

private:
    std::string sid_;
    std::string qrCodeUrl_;
    std::string authCode_;
};

#endif  //PANQUESTS_H