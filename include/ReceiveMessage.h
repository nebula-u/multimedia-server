#ifndef RECEIVEMESSAGE_H
#define RECEIVEMESSAGE_H

#include "threadpool.h"
#include "global.h"

class ReceiveMessage : public Task
{
public:
    Any run();

private:
    RETURN_CODE CreateServerSocket();

private:
    int server_fd_;
    struct sockaddr_in address_;
    int addrlen_;
    char buffer[BUFFER_SIZE];
};

#endif // RECEIVEMESSAGE_H