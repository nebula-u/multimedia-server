#ifndef GLOBAL_H
#define GLOBAL_H

#include "common.h"
#include "SharedVariable.h"

extern Config config;
extern SharedVariable sharedVariable;
extern CURL *curl;

size_t WriteTextCallback(void *contents, size_t size, size_t nmemb, void *userp);
size_t WriteBinaryCallback(void* contents, size_t size, size_t nmemb, void* userp);
bool Get(const std::string &url, const std::string &json, 
          const std::vector<std::string>& headers, std::string &response, 
          size_t (*callback)(void*, size_t, size_t, void*), const std::string& fileName = "");
bool Post(const std::string &url, const std::string &json, 
          const std::vector<std::string>& headers, std::string &response, 
          size_t (*callback)(void*, size_t, size_t, void*));

#endif