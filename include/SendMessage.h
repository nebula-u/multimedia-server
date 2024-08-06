#ifndef SENDMESSAGE_H
#define SENDMESSAGE_H

#include "threadpool.h"
#include "global.h"

class SendMessage : public Task
{
public:
    Any run();

private:
};

#endif