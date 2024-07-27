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
};


/*************************全局配置**************************/
typedef struct
{
    std::string client_id;
    std::string client_secret;
    std::string server_port;
} Config;

#endif