#ifndef GLOBAL_H
#define GLOBAL_H

#include "common.h"
#include "SharedVariable.h"
#include "PanRequests.h"

extern Config config;
extern SharedVariable sharedVariable;
extern CURL *curl;

/***************************************** 回调函数 *****************************************/
size_t WriteTextCallback(void *contents, size_t size, size_t nmemb, void *userp);
size_t WriteBinaryCallback(void* contents, size_t size, size_t nmemb, void* userp);

#endif