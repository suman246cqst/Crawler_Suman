#ifndef FETCHER_H
#define FETCHER_H

#include <curl/curl.h>
#include <string>

#include "FetchResult.h"

class Fetcher
{
public:

    Fetcher();

    ~Fetcher();

    FetchResult fetch(
        const std::string& url);

private:

    static size_t writeCallback(
            void* contents,
            size_t size,
            size_t nmemb,
            void* userData);
};

#endif