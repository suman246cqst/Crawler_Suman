#ifndef FETCH_RESULT_H
#define FETCH_RESULT_H

#include <string>

struct FetchResult
{
    std::string url;
    std::string html;

    long statusCode = 0;

    bool success = false;

    std::string errorMessage;
};

#endif