#ifndef CRAWLER_H
#define CRAWLER_H
#include<iostream>
#include "Fetcher.h"
#include "Frontier.h"
#include "LinkExtractor.h"
#include "PageStorage.h"
#include "SeenStore.h"
#include "URLNormalizer.h"
using namespace std;
class Crawler { 
private: 
    Frontier    frontier; 
    SeenStore   seen; 
    PageStorage storage;
    Fetcher fetcherTool;
    LinkExtractor extractor;
    URLNormalizer normalizer;
    int  maxDepth;       
    int  maxPages;
public: 
    Crawler(int maxDepth, int maxPages);
    void crawl(string seedURL);
};
#endif