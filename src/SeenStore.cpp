#include<iostream>
#include "../include/SeenStore.h"
using namespace std;
bool SeenStore::contains(string& url){
    return seenURLs.exists(url);
}
int SeenStore::count(){
    return seenURLs.getSize();
}
void SeenStore::clear(){
    seenURLs.clear();
}
void SeenStore::insert(string& url){
    bool visited=true;
    seenURLs.insert(url,visited);
}

