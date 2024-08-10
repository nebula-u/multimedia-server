#include "SendMessage.h"

Any SendMessage::run()
{
    while (true)
    {
        std::string message = sendMQ->DeQueue();
        if (true == sharedVariable->getClientStatus())
        {
            SendMessageSize(message.size());
            std::cout << "发送的字节数：" << message.size() << std::endl;
            send(clientSocket, message.c_str(), message.size(), 0);
        }
        else
        {
            std::cerr << "与客户端的连接异常，消息发送失败" << std::endl;
        }
        usleep(50000);
    }

    return 0;
}

void SendMessage::SendMessageSize(size_t length)
{
    send(clientSocket, &length, sizeof(length), 0);
    usleep(50000);
}
