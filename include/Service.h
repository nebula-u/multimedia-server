#ifndef SERVICE_H
#define SERVICE_H

#include <unistd.h>
#include "json.h"
#include "threadpool.h"
#include "common.h"
#include "global.h"
#include "PanRequests.h"

class Service : public Task
{    
public:
    Any run();

private:
    void LoginSessionid();
    void LoginPassword();
    void PanAuthStatus();
    void PanQRCodeRequest();

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
    ClientToServer001 clientToServer001_;
    ServerToClient001 serverToClient001_;
    PanRequests* panRequests_;
};

#endif  // SERVICE_H