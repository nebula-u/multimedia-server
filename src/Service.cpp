#include "Service.h"

Any Service::run()
{
    this->panAuthStatus_ = false;
    this->sessionWork_ = false;
    this->sessionid_ = "";
    this->username = "nebulau";
    this->uid_ = "123";
    this->password_ = "123456";
    this->aliPanRequests_ = new AliPanRequests();
    this->baiduPanRequests_ = new BaiduPanRequests();
    this->deviceCode_ = "";
    while (true)
    {
        this->JsonParse(recvMQ->DeQueue());
        if ("login-sessionid" == this->clientToServer001_.operation)
        {
            std::cout << "登录-sessionid" << std::endl;
            this->LoginSessionid();
        }
        else if ("login-password" == this->clientToServer001_.operation)
        {
            std::cout << "登录-password" << std::endl;
            this->LoginPassword();
        }
        else if ("auth-status-request" == this->clientToServer001_.operation)
        {
            std::cout << "获取云盘授权状态" << std::endl;
            this->PanAuthStatus();
        }
        else if ("auth-qrcode-request" == this->clientToServer001_.operation)
        {
            std::cout << "获取云盘登录二维码" << std::endl;
            this->DeviceCodeRequest();
        }
        else if("auth-login-status-resuest" == this->clientToServer001_.operation)
        {
            std::cout << "获取二维码登录状态" << std::endl;
            this->GetAuthLoginStatus();
        }
        else if("get-file-list" == this->clientToServer001_.operation)
        {
            std::cout << "获取文件列表" << std::endl;
            this->GetFileList();
        }
        else
        {
            std::cout << "未知的操作请求" << std::endl;
        }
    }
    return 0;
}

void Service::LoginSessionid()
{
    this->serverToClient001_.type = "login-status-si"; // 响应登录状态

    // 会话有效，并且收到客户端的会话id与服务端的会话id一致
    if (true == this->sessionWork_ && (this->sessionid_ == this->clientToServer001_.sessionId))
    {
        // 允许客户端登录，并生成新的会话id，返回给客户端
        this->serverToClient001_.result = "success"; // 登录状态为失败
        this->sessionid_ = this->GenerateId();
        this->sessionWork_ = true;
        this->serverToClient001_.newSessionid = this->sessionid_; // 更新sessionid
    }
    else
    {
        this->serverToClient001_.result = "fail";   // 登录状态为失败
        this->serverToClient001_.newSessionid = ""; // sessionid填为空
    }
    sendMQ->EnQueue(this->Stringify(this->serverToClient001_));
    this->StatusClear();
    this->PanAuthStatus();  //触发一次授权状态的反馈
}

void Service::LoginPassword()
{
    this->serverToClient001_.type = "login-status-pw"; // 响应登陆状态

    if (this->uid_ == this->clientToServer001_.uid && this->password_ == this->clientToServer001_.password)
    {
        this->serverToClient001_.result = "success";
        this->serverToClient001_.username = this->username;
        this->sessionid_ = this->GenerateId();
        this->sessionWork_ = true;
        this->serverToClient001_.newSessionid = this->sessionid_;
    }
    else
    {
        this->serverToClient001_.result = "fail"; // 登录状态为失败
        this->serverToClient001_.username = "";
        this->serverToClient001_.newSessionid = ""; // sessionid填为空
    }
    sendMQ->EnQueue(this->Stringify(this->serverToClient001_));
    this->StatusClear();
    this->PanAuthStatus();  //触发一次授权状态的反馈
}

void Service::PanAuthStatus()
{
    this->serverToClient001_.type = "pan-auth-status";
    if (true == this->panAuthStatus_)
    {
        this->serverToClient001_.result = "true";
    }
    else
    {
        this->serverToClient001_.result = "false";
    }
    sendMQ->EnQueue(this->Stringify(this->serverToClient001_));
    this->StatusClear();
}

void Service::DeviceCodeRequest()
{
    std::string response = baiduPanRequests_->GetDeviceCode();
    std::cout << response << std::endl;
    Json::Reader reader;
    Json::Value root;
    reader.parse(response, root);
    std::string QRCodeurl = root["qrcode_url"].asString();
    this->deviceCode_ = root["device_code"].asString();

    this->serverToClient001_.newSessionid = "";
    this->serverToClient001_.username = "";
    this->serverToClient001_.type = "QRCode-url";
    this->serverToClient001_.result = "true";
    this->serverToClient001_.url = QRCodeurl;
    sendMQ->EnQueue(this->Stringify(this->serverToClient001_));
    this->StatusClear();
}

/**
 * 百度网盘请求二维码后，查询授权状态
 */
void Service::GetAuthLoginStatus()
{
    std::string response = baiduPanRequests_->GetAccessTokenByDeviceCode(this->deviceCode_);
    Json::Reader reader;
    Json::Value root;
    reader.parse(response, root);
    std::string errorMsg = root["error"].asString();
    std::string expires_in = root["expires_in"].asString();

    this->serverToClient001_.type = "auth-login-status";
    if(errorMsg!=""){
        this->panAuthStatus_ = false;
        this->serverToClient001_.result = "false";
    }
    if(expires_in!=""){
        this->panAuthStatus_ = true;
        this->accessToken_ = root["access_token"].asString();
        this->refreshToken_ = root["refresh_token"].asString();
        this->serverToClient001_.result = "true";
    }
    this->serverToClient001_.newSessionid = "";
    this->serverToClient001_.username = "";
    this->serverToClient001_.url = "";
    sendMQ->EnQueue(this->Stringify(this->serverToClient001_));
    this->StatusClear();
    this->PanAuthStatus();  //触发一次授权状态的反馈
}

/**
 * 百度网盘获取文件列表
 */
void Service::GetFileList()
{
    std::string requestPath = this->clientToServer001_.path;
    this->serverToClient001_.fileList.resize(0);
    std::string response = baiduPanRequests_->GetFileList(requestPath, this->accessToken_);
    Json::Reader reader;
    Json::Value root;
    reader.parse(response, root);

    this->serverToClient001_.type = "file-list";

    std::string errorno = root["errno"].asString();
    if(errorno == "0"){
        for(int i = 0; i < root["list"].size(); i++){
            std::vector<std::string> item;
            item.push_back(root["list"][i]["server_filename"].asString());
            item.push_back(root["list"][i]["isdir"].asString());
            item.push_back(root["list"][i]["size"].asString());
            item.push_back(root["list"][i]["path"].asString());
            item.push_back(root["list"][i]["thumbs"]["url2"].asString());
            item.push_back(root["list"][i]["server_mtime"].asString());
            item.push_back(root["list"][i]["category"].asString());
            item.push_back(root["list"][i]["fs_id"].asString());
            this->serverToClient001_.fileList.push_back(item);
        }
        this->serverToClient001_.result = "true";
    }
    else
    {
        this->serverToClient001_.result = "false";
    }
    sendMQ->EnQueue(this->Stringify(this->serverToClient001_));
    this->StatusClear();
}

void Service::StatusClear()
{
    this->serverToClient001_.type = "";
    this->serverToClient001_.result = "";
    this->serverToClient001_.username = "";
    this->serverToClient001_.newSessionid = "";
    this->serverToClient001_.url = "";
    this->serverToClient001_.fileList.resize(0);
}

void Service::JsonParse(std::string message)
{
    Json::Reader reader;
    Json::Value root;

    if (reader.parse(message, root))
    {
        this->clientToServer001_.operation = root["operation"].asString();
        this->clientToServer001_.sessionId = root["sessionId"].asString();
        this->clientToServer001_.uid = root["uid"].asString();
        this->clientToServer001_.username = root["username"].asString();
        this->clientToServer001_.password = root["password"].asString();
        this->clientToServer001_.path = root["path"].asString();
    }
    else
    {
        std::cerr << "消息解析失败: " << message << std::endl;
    }
}

std::string Service::Stringify(ServerToClient001 &s2c)
{
    Json::Value root;
    Json::Value fileList;;
    Json::StreamWriterBuilder writer;

    root["type"] = s2c.type;
    root["result"] = s2c.result;
    root["username"] = s2c.username;
    root["newSessionid"] = s2c.newSessionid;
    root["url"] = s2c.url;

    for(int i = 0; i < s2c.fileList.size(); i++){
        fileList["filename"] = s2c.fileList[i][0];
        fileList["isdir"] = s2c.fileList[i][1];
        fileList["size"] = s2c.fileList[i][2];
        fileList["path"] = s2c.fileList[i][3];
        fileList["thumbs"] = s2c.fileList[i][4];
        fileList["mtime"] = s2c.fileList[i][5];
        fileList["category"] = s2c.fileList[i][6];
        fileList["fid"] = s2c.fileList[i][7];
        root["filelist"].append(fileList);
    }
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
