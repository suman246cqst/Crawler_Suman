#include "../include/Frontier.h"

URLDepth::URLDepth(const std::string& url, int depth) : url(url), depth(depth){}

bool URLDepth::operator==(const URLDepth& other) const{
    return url == other.url;
}

void Frontier::push(URLDepth& item){
    if(pendingURLs.exists(item.url)){
        return;
    }
    pendingQueue.enqueue(item);
    bool visited=true;
    pendingURLs.insert(item.url, visited);
}

URLDepth Frontier::pop(){
    URLDepth item = pendingQueue.dequeue();
    pendingURLs.remove(item.url);
    return item;
}

const URLDepth& Frontier::front() {
    return pendingQueue.peek();
}

bool Frontier::contains(string& url){
    return pendingURLs.exists(url);
}

bool Frontier::empty() const{
    return pendingQueue.isEmpty();
}

int Frontier::size() const{
    return pendingQueue.size();
}

