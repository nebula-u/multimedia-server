#include  "ReceiveMessage.h"

Any ReceiveMessage::run()
{
    while (true)
    {
        if(RETURN_CODE::NO_ERROR == CreateServerSocket())
        {
            std::cout << "创建监听socket成功" << std::endl;
        }
        sleep(1000);
    }
    
    return 0;
}

RETURN_CODE ReceiveMessage::CreateServerSocket()
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(std::stoi(config.server_port));
    bind(this->server_fd_, (struct sockaddr*)&address, sizeof(address));
    listen(this->server_fd_, 3);
    return RETURN_CODE::NO_ERROR;
}
