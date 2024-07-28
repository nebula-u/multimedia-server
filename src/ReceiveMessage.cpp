#include "ReceiveMessage.h"

Any ReceiveMessage::run()
{
    if (RETURN_CODE::NO_ERROR == CreateServerSocket())
    {
        std::cout << "创建监听socket成功" << std::endl;
        while (true)
        {
            int failureCount = 0;
            std::cout << "等待客户端连接" << std::endl;
            clientSocket = accept(this->server_fd_, (struct sockaddr *)&(this->address_), (socklen_t *)&(this->addrlen_));
            sharedVariable->setClientStatus(true);
            std::cout << "客户端已连接" << std::endl;
            while (true)
            {
                memset(this->buffer, 0, BUFFER_SIZE);
                int size = read(clientSocket, buffer, BUFFER_SIZE);
                if (0 >= size)
                {
                    std::cerr << "与客户端连接已断开" << std::endl;
                    failureCount++;
                    if (failureCount == 5)
                    {
                        sharedVariable->setClientStatus(false);
                        break;
                    }
                    sleep(1);
                }
                else
                {
                    recvMQ->EnQueue(std::string(buffer));
                }
            }
        }
    }
    else
    {
        std::cerr << "监听服务端口创建失败" << std::endl;
    }

    return 0;
}

RETURN_CODE ReceiveMessage::CreateServerSocket()
{
    this->addrlen_ = sizeof(this->address_);
    if (0 == (this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0)))
    {
        std::cerr << "socket failed" << std::endl;
        return RETURN_CODE::CREATE_SERVER_SOCKET_ERROR;
    }

    int opt = 1;

    if (setsockopt(this->server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        std::cerr << "socketopt failed" << std::endl;
        return RETURN_CODE::CREATE_SERVER_SOCKET_ERROR;
    }

    this->address_.sin_family = AF_INET;
    this->address_.sin_addr.s_addr = INADDR_ANY;
    this->address_.sin_port = htons(std::stoi(config.server_port));
    if (bind(this->server_fd_, (struct sockaddr *)&(this->address_), sizeof(this->address_)) < 0)
    {
        std::cerr << "bind failed" << std::endl;
        return RETURN_CODE::CREATE_SERVER_SOCKET_ERROR;
    }

    if (listen(this->server_fd_, 3) < 0)
    {
        std::cerr << "listen fsiled" << std::endl;
        return RETURN_CODE::CREATE_SERVER_SOCKET_ERROR;
    }
    return RETURN_CODE::NO_ERROR;
}
