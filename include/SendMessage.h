#ifndef SENDMESSAGE_H
#define SENDMESSAGE_H

#include "json.h"
#include "threadpool.h"
#include "global.h"

class SendMessage : public Task
{
public:
    Any run();
    void SendMessageSize(size_t length);
private:
};

#endif