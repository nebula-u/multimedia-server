#include "SendMessage.h"

Any SendMessage::run()
{
    while (true)
    {
        std::string message = sendMQ->DeQueue();
        if (true == sharedVariable->getClientStatus())
        {
            send(clientSocket, message.c_str(), message.size(), 0);
        }
        else
        {
            std::cerr << "与客户端的连接异常，消息发送失败" << std::endl;
        }
    }

    return 0;
}