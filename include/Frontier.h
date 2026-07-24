#ifndef FRONTIER_H
#define FRONTIER_H

#include <string>

#include "../../DS_Library/include/Queue.h"
#include "../../DS_Library/include/HashMap.h"

class URLDepth
{
public:
    std::string url;
    int depth;

    URLDepth(const std::string& url, int depth);

    bool operator==(const URLDepth& other) const;
};

class Frontier
{
private:
    Queue<URLDepth> pendingQueue;
    HashMap<string, bool> pendingURLs;

public:
    void push(URLDepth& item);

    URLDepth pop();

    const URLDepth& front() ;

    bool contains(std::string& url) ;

    bool empty() const;

    int size() const;
};

#endif