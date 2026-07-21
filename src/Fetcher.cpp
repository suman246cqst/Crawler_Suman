#include "Fetcher.h"

#include <curl/curl.h>

#include <stdexcept>

Fetcher::Fetcher()
{
    CURLcode result =
        curl_global_init(CURL_GLOBAL_DEFAULT);

    if(result != CURLE_OK)
    {
        throw std::runtime_error(
            "Failed to initialize libcurl.");
    }
}

Fetcher::~Fetcher()
{
    curl_global_cleanup();
}

size_t Fetcher::writeCallback(void* contents,size_t size,size_t nmemb,void* userData)
{
    size_t totalBytes = size * nmemb;

    std::string* html =static_cast<std::string*>(userData);

    html->append(static_cast<char*>(contents),totalBytes);

    return totalBytes;
}

FetchResult Fetcher::fetch(const std::string& url)
{
    FetchResult result;

    result.url = url;

    CURL* curl = curl_easy_init();

    if(curl == nullptr)
    {
        result.errorMessage ="Unable to initialize CURL.";
        return result;
    }

    std::string html;

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        url.c_str());

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        writeCallback);

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &html);

    curl_easy_setopt(
        curl,
        CURLOPT_FOLLOWLOCATION,
        1L);

    curl_easy_setopt(
        curl,
        CURLOPT_TIMEOUT,
        30L);

    curl_easy_setopt(
        curl,
        CURLOPT_USERAGENT,
        "MyCrawler/1.0");

    curl_easy_setopt(
        curl,
        CURLOPT_ACCEPT_ENCODING,
        "");

    CURLcode code =
        curl_easy_perform(curl);

    if(code != CURLE_OK)
    {
        result.errorMessage = curl_easy_strerror(code);

        curl_easy_cleanup(curl);

        return result;
    }

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &result.statusCode);

    result.success = true;
    result.html = std::move(html);

    curl_easy_cleanup(curl);

    return result;
}