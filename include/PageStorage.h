#ifndef PAGE_STORAGE_H
#define PAGE_STORAGE_H

#include <string>
#include "Database.h"
#include "frontier.h"
#include "SeenStore.h"

using namespace std;

class PageStorage
{
private:
    string storageDirectory;
    Database database;

private:
    void createStorageDirectory();
    string buildFilePath(const string& fileName) const;

public:
    // Constructor
    PageStorage();

    // ==============================
    // Crawler APIs
    // ==============================

    // Restores crawler state from persistent storage
    void recoverCrawlerState(Frontier& frontier,
                             SeenStore& seenStore);

    // Stores a newly discovered URL in the persistent frontier
    bool addPendingURL(const string& url,
                       int depth);

    // Stores the crawled page and updates persistent state
    bool storePage(const string& url,
                   const string& html,
                   int depth);

    // ==============================
    // Indexer APIs
    // ==============================

    // Returns the total number of stored pages
    int pageCount();

    // Returns the URL corresponding to a page ID
    string getURLByID(int id);

    // Returns the HTML of the specified URL
    string getPage(const string& url);

    // Checks whether the page already exists
    bool hasPage(const string& url);
};

#endif