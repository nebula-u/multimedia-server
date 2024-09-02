#include "SendMessage.h"

Any SendMessage::run()
{
    while (true)
    {
        std::string message = sendMQ->DeQueue() + "#end";
        if (true == sharedVariable->getClientStatus())
        {
            std::cout << "\n\n------------------------------------" << std::endl;
            std::cout << message << std::endl;
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
