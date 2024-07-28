#ifndef SERVICE_H
#define SERVICE_H

#include <unistd.h>
#include "json.h"
#include "threadpool.h"
#include "common.h"
#include "global.h"

class Service : public Task
{    
public:
    Any run();

private:
    void SessionStatus();

    void JsonParse(std::string message);
    std::string Stringify(ServerToClient001 &s2c);
    std::string GenerateId();

private:
    bool panAuthStatus_;    // 保存云盘授权状态（true=已获得授权 false=未获得授权）
    bool sessionWork_;      // 当前session是否有效
    std::string sessionid_;
    ClientToServer001 clientToServer001_;
    ServerToClient001 serverToClient001_;
};

#endif  // SERVICE_H