#include "PageStorage.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = filesystem;

PageStorage::PageStorage(const string& storageDirectory){
    this->storageDirectory = storageDirectory;
    indexFilePath = storageDirectory + "/index.txt";
    totalPages = 0;
    createStorageDirectory();
    createIndexFile();
    initializePageCount();
}

void PageStorage::createStorageDirectory(){
    if (!fs::exists(storageDirectory)){
        fs::create_directories(storageDirectory);
    }
}

void PageStorage::createIndexFile(){
    if (!fs::exists(indexFilePath)){
        ofstream file(indexFilePath);
        file.close();
    }
}

void PageStorage::initializePageCount(){
    ifstream file(indexFilePath);
    string line;
    totalPages = 0;
    while (getline(file, line)){
        if (!line.empty())
        {
            totalPages++;
        }
    }
    file.close();
}