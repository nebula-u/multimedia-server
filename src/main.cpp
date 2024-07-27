#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include "threadpool.h"
#include "json.h"
#include "common.h"
#include "PanRequests.h"
#include "Server.h"

/******************全局变量******************/
Config config;
SharedVariable sharedVariable;
CURL *curl;
PanRequests requests;

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
        config.client_id = root["client_id"].asString();
        config.client_secret = root["client_secret"].asString();
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
    pool->start(2);
    pool->submitTask(std::make_shared<Server>());

    getchar();
    return 0;
}
