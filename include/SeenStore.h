#ifndef SEENSTORE_H
#define SEENSTORE_H
#include<iostream>
#include "../../DS_Library/include/hashmap.h"
class SeenStore{
private:
    HashMap<std::string, bool> seenURLs;
public:
    bool contains(string& url);
    void insert(string& url);
    void clear();
    int count();
};
#endif