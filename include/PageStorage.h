#ifndef PAGE_STORAGE_H
#define PAGE_STORAGE_H

#include <string>

using namespace std;
class PageStorage
{
private:
    string storageDirectory;
    string indexFilePath;
    int totalPages;
private:
    void createStorageDirectory();
    void createIndexFile();
    void initializePageCount();
public:
    PageStorage(const string& storageDirectory);
    void storePage(const string& url,const string& html,int depth);
    string getPage(const string& url);
    bool hasPage(const string& url);
    int pageCount() const;
};

#endif