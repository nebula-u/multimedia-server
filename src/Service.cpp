#include "Service.h"

Any Service::run()
{
    this->panAuthStatus_ = false;
    this->sessionWork_ = true;
    this->sessionid_ = "12345678901234567890";
    while (true)
    {
        this->JsonParse(recvMQ->DeQueue());
        if("get_login_status" == this->clientToServer001_.operation){
            this->serverToClient001_.type = "pan-auth-status";
            if(true == this->panAuthStatus_)
            {
                // this->serverToClient001_.param = "1";
            }
            else
            {
                // this->serverToClient001_.param = "0";
            }
            sendMQ->EnQueue(this->Stringify(this->serverToClient001_));
        }
        else if("get_session_status")
        {
            this->SessionStatus();
        }
        else
        {
            std::cout << "未知的操作请求" << std::endl;
        }
    }
    return 0;
}

void Service::SessionStatus()
{
    this->serverToClient001_.type = "loginStatus";  // 响应登录状态

    // 会话有效，并且收到客户端的会话id与服务端的会话id一致
    if(true == this->sessionWork_ && (this->sessionid_ == this->clientToServer001_.sessionId))
    {
        // 允许客户端登录，并生成新的会话id，返回给客户端
        this->serverToClient001_.result = "1";                      // 登录状态为1
        this->sessionid_ = this->GenerateId();
        this->serverToClient001_.newSessionid = this->sessionid_;   // 更新sessionid
        std::cout << this->serverToClient001_.newSessionid << std::endl;
    }
    else
    {
        this->serverToClient001_.result = "0";          // 登录状态为0
        this->serverToClient001_.newSessionid = "";     // sessionid填为空
    }
    sendMQ->EnQueue(this->Stringify(this->serverToClient001_));
}

void Service::JsonParse(std::string message)
{
    Json::Reader reader;
    Json::Value root;

    if(reader.parse(message, root))
    {
        this->clientToServer001_.operation = root["operation"].asString();
        this->clientToServer001_.sessionId = root["sessionId"].asString();
    }
    else
    {
        std::cerr << "消息解析失败: " << message << std::endl;
    }
}

std::string Service::Stringify(ServerToClient001 &s2c)
{
    Json::Value root;
    Json::StreamWriterBuilder writer;

    root["type"] = s2c.type;
    root["result"] = s2c.result;
    root["newSessionid"] = s2c.newSessionid;

    return Json::writeString(writer, root);
}

std::string Service::GenerateId()
{
    std::srand(std::time(0));
    int r1 = 1000 + std::rand() % 9000;
    int r2 = 1000 + std::rand() % 9000;
    int r3 = 1000 + std::rand() % 9000;
    int r4 = 1000 + std::rand() % 9000;
    int r5 = 1000 + std::rand() % 9000;

    return std::to_string(r1) + std::to_string(r2) + std::to_string(r3) + std::to_string(r4) + std::to_string(r5);
}
