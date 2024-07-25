#include <curl/curl.h>
#include <iostream>
#include <unistd.h>
#include "threadpool.h"

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

class CurlRequest : public Task
{
public:
    Any run()
    {
        CURL *curl;
        CURLcode res;
        std::string readBuffer;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();

        std::cout << readBuffer << std::endl;
        return 0;
    }
};

int main()
{
    ThreadPool *pool = new ThreadPool();
    pool->start(6);
    pool->submitTask(std::make_shared<CurlRequest>());

    getchar();
    return 0;
}
