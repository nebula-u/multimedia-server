#ifndef COMMMON_H
#define COMMMON_H

#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <unistd.h>

/*********************收发短消息缓存大小*********************/
#define BUFFER_SIZE 1024


/***************************返回值**************************/
enum class RETURN_CODE
{
    NO_ERROR,                   // 函数执行正确
    LOAD_CONFIG_ERROR,          // 配置文件加载失败
    GET_LOGIN_QR_SID_ERROR,     // 获取登录二维码 {sid} 失败
    GET_LOGIN_QR_ERROR,         // 获取登录二维码失败
    QR_EXPIRED,                 // 登录二维码过期
    GET_ACCESS_TOKEN_ERROR,     // 获取access_token失败
    REFRESH_ACCESS_TOKEN_ERROR, // 刷新sccess_token失败
    MESSAGE_ENQUEUE_ERROR,      // 向消息队列中添加消息失败
    CREATE_SERVER_SOCKET_ERROR, // 创建服务端监听socket失败
};


/*************************全局配置**************************/
typedef struct
{
    std::string ali_client_id;
    std::string ali_client_secret;
    std::string baidu_app_key;
    std::string baidu_screte_key;
    std::string baidu_sign_key;
    std::string server_port;
} Config;

/****************************ICD****************************/
typedef struct
{
    std::string operation;
    std::string sessionId;
    std::string uid;
    std::string username;
    std::string password;
}ClientToServer001;

typedef struct
{
    std::string type;              // 状态类型
    std::string result;            // 状态类型的结果
    std::string username;          // 用户名
    std::string newSessionid;      // 新的会话ID
    std::string url;
}ServerToClient001;

#endif