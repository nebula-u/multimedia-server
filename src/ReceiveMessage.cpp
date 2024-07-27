#include  "ReceiveMessage.h"

Any ReceiveMessage::run()
{
    while (true)
    {
        if(RETURN_CODE::NO_ERROR == CreateServerSocket())
        {
            std::cout << "创建监听socket成功" << std::endl;
            std::cout << "等待客户端连接" << std::endl;
            clientSocket = accept(this->server_fd_, (struct sockaddr*)&(this->address_), (socklen_t*)&(this->addrlen_));
            std::cout << "客户端已连接" << std::endl;
            while (true)
            {
                memset(this->buffer, 0, BUFFER_SIZE);
                read(clientSocket, buffer, BUFFER_SIZE);
                std::cout << buffer << std::endl;
            }
            
        }
        sleep(1000);
    }
    
    return 0;
}

RETURN_CODE ReceiveMessage::CreateServerSocket()
{
    
    this->addrlen_ = sizeof(this->address_);
    this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    this->address_.sin_family = AF_INET;
    this->address_.sin_addr.s_addr = INADDR_ANY;
    this->address_.sin_port = htons(std::stoi(config.server_port));
    bind(this->server_fd_, (struct sockaddr*)&(this->address_), sizeof(this->address_));
    listen(this->server_fd_, 3);
    return RETURN_CODE::NO_ERROR;
}
