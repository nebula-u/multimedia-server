#include "Server.h"

Any Server::run()
{
    while (true)
    {
        sleep(3);
        GetUserInfo();
    }
    return 0;
}