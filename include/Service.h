#ifndef SERVICE_H
#define SERVICE_H

#include <unistd.h>
#include "json.h"
#include "threadpool.h"
#include "common.h"
#include "global.h"
#include "AliPanRequests.h"
#include "BaiduPanRequests.h"

class Service : public Task
{    
public:
    Any run();

private:
    void LoginSessionid();
    void LoginPassword();
    void PanAuthStatus();
    void DeviceCodeRequest();
    void GetAuthLoginStatus();
    void GetFileList();
    void GetDlink();

    void StatusClear();
    void JsonParse(std::string message);
    std::string Stringify(ServerToClient001 &s2c);
    std::string GenerateId();

private:
    bool panAuthStatus_;    // 保存云盘授权状态（true=已获得授权 false=未获得授权）
    bool sessionWork_;      // 当前session是否有效
    std::string sessionid_;
    std::string username;
    std::string uid_;
    std::string password_;
    std::string deviceCode_;
    std::string accessToken_;
    std::string refreshToken_;
    ClientToServer001 clientToServer001_;
    ServerToClient001 serverToClient001_;
    AliPanRequests* aliPanRequests_;
    BaiduPanRequests* baiduPanRequests_;
};

#endif  // SERVICE_H