#ifndef GLOBAL_H
#define GLOBAL_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "common.h"
#include "SharedVariable.h"
#include "messagequeue.h"

extern Config config;
extern SharedVariable* sharedVariable;
extern CURL *curl;
extern int clientSocket;
extern MessageQueue* recvMQ;
extern MessageQueue* sendMQ;
extern std::mutex mtxRequest;

/***************************************** 回调函数 *****************************************/
size_t WriteTextCallback(void *contents, size_t size, size_t nmemb, void *userp);
size_t WriteBinaryCallback(void* contents, size_t size, size_t nmemb, void* userp);

#endif