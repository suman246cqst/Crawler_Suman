#include "PageStorage.h"
#include "SeenStore.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = filesystem;

PageStorage::PageStorage(){
    storageDirectory = "../CrawlerStorage";
    indexFilePath = storageDirectory + "/index.txt";
    createStorageDirectory();
    createIndexFile();
    initializeStorage();
}
void PageStorage::initializeStorage(){
    ifstream indexFile(indexFilePath);
    if (!indexFile){
        return;
    }
    string line;
    totalPages = 0;

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

        PageInfo info;
        info.id = id;
        info.depth = depth;
        pageIndex.insert(url, info);
        idToURL.insert(id, url);

        totalPages++;
    }

    indexFile.close();
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


void PageStorage::storePage(string& url,string& html,int depth){
    int fileID = totalPages + 1;
    string pageFilePath =storageDirectory + "/" +to_string(fileID) + ".page";
    ofstream pageFile(pageFilePath);
    if (!pageFile){
        return;
    }
    pageFile << url << '\n';
    pageFile << depth << '\n';
    pageFile << html;
    pageFile.close();

    ofstream indexFile(indexFilePath, ios::app);
    if (!indexFile){
        return;
    }

    indexFile
        << fileID
        << "|"
        << url
        << "|"
        << depth
        << '\n';

    indexFile.close();
    PageInfo info;
    info.id = fileID;
    info.depth = depth;
    pageIndex.insert(url, info);
    idToURL.insert(fileID, url);
    totalPages++;
}

bool PageStorage::hasPage(string& url){
    return pageIndex.exists(url);
}

string PageStorage::getPage(string& url){
    if(!pageIndex.exists(url)){
        return "";
    }
    PageInfo storedPage=pageIndex.get(url);
    string pageFilePath =storageDirectory + "/" +to_string(storedPage.id)+".page";

    ifstream pageFile(pageFilePath);
    if (!pageFile){
        return "";
    }
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

string PageStorage::getURLByID(int id){
    if(!idToURL.exists(id)){
        return "";
    }
    return idToURL.get(id);
}
int PageStorage::getPageCount() const{
    return totalPages;
}