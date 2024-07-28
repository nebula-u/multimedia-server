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
    void JsonParse(std::string message);

private:
    bool panAuthStatus_;    // 保存云盘授权状态（true=已获得授权 false=未获得授权）
    ClientToServer001 clientToServer001_;
};

#endif  // SERVICE_H