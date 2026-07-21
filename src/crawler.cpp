#include<iostream>
#include "../include/crawler.h"
Crawler::Crawler(int maxDepth, int maxPages):maxDepth(maxDepth),maxPages(maxPages),storage("../CrawlerStorage"){}
void Crawler::crawl(string seedURL){
        URLDepth baseurl(seedURL,0);
        frontier.push(baseurl);
        while (!frontier.empty() && storage.pageCount() < maxPages){
            URLDepth temp=frontier.pop();
            if(seen.contains(temp.url)){
                continue;
            }
            else{
                seen.insert(temp.url);
                FetchResult result=fetcherTool.fetch(temp.url);
                if(!result.success){
                    continue; 
                }
                storage.storePage(result.url,result.html,temp.depth);
                DynamicArray<string>links=extractor.extractLinks(result.html);
                for(int i=0;i<links.getSize();i++){
                    string normalizedURL=normalizer.normalize(temp.url,links[i]);
                    if(!seen.contains(normalizedURL) && !storage.hasPage(normalizedURL)){
                        if(temp.depth + 1 <= maxDepth){
                            URLDepth normalized(normalizedURL,(temp.depth)+1);
                            frontier.push(normalized);
                        }
                    }
                }
            }
        }
    } 