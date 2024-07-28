#include "Service.h"

Any Service::run()
{
    this->panAuthStatus_ = false;
    while (true)
    {
        this->JsonParse(recvMQ->DeQueue());
        if("get_login_status" == this->clientToServer001_.operation){
            std::cout << "返回服务端登录状态：" << this->panAuthStatus_ << std::endl;
        }
        else
        {
            std::cerr << "未知的操作请求" << std::endl;
        }
    }
    

    return 0;
}

void Service::JsonParse(std::string message)
{
    Json::Reader reader;
    Json::Value root;

    if(reader.parse(message, root))
    {
        this->clientToServer001_.operation = root["operation"].asString();
        this->clientToServer001_.params = root["params"].asString();
    }
    else
    {
        std::cerr << "消息解析失败: " << message << std::endl;
    }
}
