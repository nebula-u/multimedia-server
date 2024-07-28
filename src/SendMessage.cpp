#include "SendMessage.h"

Any SendMessage::run()
{
    while (true)
    {
        std::cout << "发送函数等待取消息" << std::endl;
        std::string message = sendMQ->DeQueue();
        std::cout << "发送函数取到消息" << message << std::endl;
        if (true == sharedVariable->getClientStatus())
        {
            send(clientSocket, message.c_str(), message.size(), 0);
            std::cout << "已经向客户端发送："  << message << std::endl;
        }
        else
        {
            std::cerr << "与客户端的连接异常，消息发送失败" << std::endl;
        }
    }

    return 0;
}