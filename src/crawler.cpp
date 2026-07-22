#include<iostream>
#include "../include/crawler.h"
Crawler::Crawler(int maxDepth, int maxPages):maxDepth(maxDepth),maxPages(maxPages){}

void Crawler::crawl(string seedURL){
    seedURL = normalizer.normalize(seedURL);
    URLDepth baseurl(seedURL,0);
    frontier.push(baseurl);
    while(!frontier.empty() && storage.getPageCount() < maxPages){
        URLDepth temp = frontier.pop();
        bool isSeed = (temp.url == seedURL);
        if(!isSeed && seen.contains(temp.url)){
            continue;
        }
        FetchResult result = fetcherTool.fetch(temp.url);
        if(!result.success){
            continue;
        }
        if(!seen.contains(temp.url)){
            seen.insert(temp.url);
            storage.storePage(result.url,result.html,temp.depth);
        }
        DynamicArray<string> links = extractor.extractLinks(result.html);
        for(int i=0;i<links.getSize();i++){
            string normalizedURL = normalizer.normalize(temp.url,links[i]);
            if(!normalizedURL.empty()&&!seen.contains(normalizedURL)&&temp.depth+1 <= maxDepth){
                frontier.push(URLDepth(normalizedURL,temp.depth+1));
            }
        }
    }
}