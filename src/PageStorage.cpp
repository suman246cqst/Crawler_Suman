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

void PageStorage::storePage(const string& url,const string& html,int depth){
    int fileID = totalPages + 1;
    string pageFilePath =storageDirectory + "/" +to_string(fileID) + ".page";
    ofstream pageFile(pageFilePath);
    pageFile << url << '\n';
    pageFile << depth << '\n';
    pageFile << html;
    pageFile.close();

    ofstream indexFile(indexFilePath, ios::app);

    indexFile
        << fileID
        << "|"
        << url
        << "|"
        << depth
        << '\n';

    indexFile.close();

    totalPages++;
}

bool PageStorage::hasPage(const string& url){
    ifstream indexFile(indexFilePath);
    string line;
    while (getline(indexFile, line)){
        stringstream ss(line);
        string id;
        string storedURL;
        string depth;

        getline(ss, id, '|');
        getline(ss, storedURL, '|');
        getline(ss, depth);

        if (storedURL == url){
            indexFile.close();
            return true;
        }
    }
    indexFile.close();
    return false;
}

string PageStorage::getPage(const string& url){
    ifstream indexFile(indexFilePath);
    string line;
    string fileID;
    while (getline(indexFile, line)){
        stringstream ss(line);
        string id;
        string storedURL;
        string depth;

        getline(ss, id, '|');
        getline(ss, storedURL, '|');
        getline(ss, depth);
        if (storedURL == url)
        {
            fileID = id;
            break;
        }
    }
    indexFile.close();
    if (fileID.empty()){
        return "";
    }
    string pageFilePath =storageDirectory + "/" +fileID + ".page";

    ifstream pageFile(pageFilePath);
    string storedURL;
    string storedDepth;
    getline(pageFile, storedURL);
    getline(pageFile, storedDepth);

    string html;
    string temp;

    while (getline(pageFile, temp))
    {
        html += temp;

        if (!pageFile.eof())
        {
            html += '\n';
        }
    }
    pageFile.close();
    return html;
}

int PageStorage::pageCount() const{
    return totalPages;
}