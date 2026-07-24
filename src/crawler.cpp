#include <iostream>
#include "../include/Crawler.h"

Crawler::Crawler(int maxDepth,int maxPages): maxDepth(maxDepth),maxPages(maxPages){
    initializeCrawler();
}

void Crawler::initializeCrawler(){
    storage.recoverCrawlerState(frontier,seen);
}

void Crawler::crawl(string seedURL){
    seedURL = normalizer.normalize(seedURL);
    if(!seedURL.empty() && !seen.contains(seedURL) && !frontier.contains(seedURL)){
        if(storage.addPendingURL(seedURL, 0)){
            URLDepth baseURL(seedURL, 0);
            frontier.push(baseURL);
        }
    }
    while(!frontier.empty()){
        if(storage.pageCount() >= maxPages){
            break;
        }
        URLDepth current = frontier.pop();
        if(seen.contains(current.url)){
            continue;
        }
        FetchResult result = fetcherTool.fetch(current.url);
        if(!result.success){
            continue;
        }
        DynamicArray<string> links = extractor.extractLinks(result.html);
        for(int i = 0; i < links.getSize(); i++){
            int nextDepth = current.depth + 1;
            if(nextDepth > maxDepth){
                continue;
            }

            string normalizedURL =normalizer.normalize(current.url,links[i]);
            if(normalizedURL.empty()){
                continue;
            }
            if(seen.contains(normalizedURL)){
                continue;
            }

            if(frontier.contains(normalizedURL)){
                continue;
            }

            if(storage.addPendingURL(normalizedURL,nextDepth)){
                URLDepth nextURL(normalizedURL,nextDepth);
                frontier.push(nextURL);
            }
        }
        if(!storage.storePage(current.url,result.html,current.depth)){
            continue;
        }
        seen.insert(current.url);
    }
}