#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include "threadpool.h"
#include "common.h"
#include "global.h"

class Server : public Task
{
private:
    
public:
    Any run();

private:
    bool panAuthStatus_;    // 保存云盘授权状态 true=已获得授权 false=未获得授权
};

#endif  // SERVER_H