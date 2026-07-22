#ifndef FRONTIER_H
#define FRONTIER_H
#include<iostream>
#include<string>
#include "../../DS_Library/include/queue.h"

using namespace std;
class URLDepth{
public:
    string url;
    int depth;
    URLDepth(const string& url, int depth);
    bool operator==(const URLDepth& other) const;
};

class Frontier{
private:
    Queue<URLDepth> queue;
public:
    void push(const URLDepth& item);
    URLDepth pop();
    const URLDepth& front();
    bool empty();
    int size();
};
#endif