#ifndef SEENSTORE_H
#define SEENSTORE_H
#include<iostream>
#include "../../DS_Library/include/hashmap.h"
class SeenStore{
private:
    string storageDirectory;
    string indexFilePath;
    HashMap<std::string, bool> seenURLs;
private:
    void createStorageDirectory();
    void createIndexFile();
    void initializeStorage();
public:
    SeenStore();
    
    bool contains(string& url);
    void insert(string& url);
    void clear();
    int count();
};
#endif