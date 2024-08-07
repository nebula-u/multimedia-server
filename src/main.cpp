#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include "threadpool.h"
#include "json.h"
#include "common.h"
#include "Service.h"
#include "ReceiveMessage.h"
#include "SendMessage.h"
#include "messagequeue.h"

/******************全局变量******************/
Config config;
SharedVariable* sharedVariable;
CURL *curl;
MessageQueue* recvMQ;
MessageQueue* sendMQ;
int clientSocket;
std::mutex mtxRequest;


/******************函数声明******************/
RETURN_CODE initializeConfig();
void initializeApp();


/**
 * 功能：完成服务的初始化
 * 作者：Nebulau
 * 日期：2024年7月25日
 * 修改历史：
 *      - 2024年7月25日：增加配置文件加载功能
 */
void initializeApp()
{
    initializeConfig();
}

/**
 * 功能：加载配置文件
 * 作者：Nebulau
 * 日期：2024年7月25日
 * 修改历史：
 *      - 2024年7月25日：读取json配置文件，做json解析，然后将配置保存在 config 中
 * 
 */
RETURN_CODE initializeConfig()
{
    std::ifstream fconfig;
    fconfig.open("../private/config.json");
    Json::Reader reader;
    Json::FastWriter writer;
    Json::Value root;
    if (reader.parse(fconfig, root, false))
    {
        config.ali_client_id = root["ali_client_id"].asString();
        config.ali_client_secret = root["ali_client_secret"].asString();
        config.baidu_app_key = root["baidu_app_key"].asString();
        config.baidu_screte_key = root["baidu_screte_key"].asString();
        config.baidu_sign_key = root["baidu_sign_key"].asString();
        config.server_port= root["server_port"].asString();
        std::cerr << "配置文件加载成功" << std::endl;
        return RETURN_CODE::NO_ERROR;
    }
    std::cerr << "配置文件加载失败" << std::endl;
    return RETURN_CODE::LOAD_CONFIG_ERROR;
}

int main()
{
    initializeApp();
    ThreadPool *pool = new ThreadPool();
    sharedVariable = new SharedVariable();
    recvMQ = new MessageQueue(20);
    sendMQ = new MessageQueue(20);

    pool->start(3);
    pool->submitTask(std::make_shared<ReceiveMessage>());
    pool->submitTask(std::make_shared<Service>());
    pool->submitTask(std::make_shared<SendMessage>());

    getchar();
    return 0;
}
