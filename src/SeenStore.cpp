#include<iostream>
#include "../include/SeenStore.h"
#include<filesystem>
#include <fstream>
#include <sstream>
using namespace std;

namespace fs = filesystem;

SeenStore::SeenStore(){
    storageDirectory = "../CrawlerStorage";
    indexFilePath = storageDirectory + "/index.txt";
    createStorageDirectory();
    createIndexFile();
    initializeStorage();
}

void SeenStore::initializeStorage(){
    ifstream indexFile(indexFilePath);

    string line;

    while (getline(indexFile, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string idString;
        string url;
        string depthString;

        getline(ss, idString, '|');
        getline(ss, url, '|');
        getline(ss, depthString);
        int id = stoi(idString);
        int depth = stoi(depthString);

        insert(url);
    }

    indexFile.close();
}
void SeenStore::createStorageDirectory(){
    if (!fs::exists(storageDirectory)){
        fs::create_directories(storageDirectory);
    }
}

void SeenStore::createIndexFile(){
    if (!fs::exists(indexFilePath)){
        ofstream file(indexFilePath);
        file.close();
    }
}
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

