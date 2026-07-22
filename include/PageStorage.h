#ifndef PAGE_STORAGE_H
#define PAGE_STORAGE_H
#include "../../DS_Library/include/hashMap.h"
#include "SeenStore.h"
#include <string>

using namespace std;
class PageInfo{
public:
    int id;
    int depth;
};

class PageStorage
{
private:
    string storageDirectory;
    string indexFilePath;
    int totalPages;
    HashMap<string, PageInfo> pageIndex;
    HashMap<int, string> idToURL;
private:
    void createStorageDirectory();
    void createIndexFile();
    
public:
    PageStorage();
    void initializeStorage();
    void storePage(string& url,string& html,int depth);
    string getPage(string& url);
    bool hasPage(string& url);
    int getPageCount() const;
    string getURLByID(int id);
};

#endif