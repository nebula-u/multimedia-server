#ifndef FILESERVER_H
#define FILESERVER_H

#include <unistd.h>
#include "threadpool.h"
#include "common.h"
#include "global.h"

class FileServer : public Task
{
private:
    RETURN_CODE GetUserInfo();
public:
    Any run()
    {
        while (true)
        {
            sleep(3);
            GetUserInfo();
        }
        
        return 0;
    }
};

#endif