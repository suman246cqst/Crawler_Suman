#include "../include/PageStorage.h"

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

PageStorage::PageStorage(): storageDirectory("../CrawlerStorage"){
    createStorageDirectory();
    database.open(buildFilePath("crawler.db"));
    database.createURLTable();
}

void PageStorage::createStorageDirectory()
{
    if(!fs::exists(storageDirectory))
    {
        fs::create_directories(storageDirectory);
    }
}

string PageStorage::buildFilePath(
    const string& fileName) const
{
    return storageDirectory + "/" + fileName;
}

void PageStorage::recoverCrawlerState(
    Frontier& frontier,
    SeenStore& seenStore)
{
    DynamicArray<URLDepth> pendingURLs =
        database.loadPendingURLs();

    for(int i = 0; i < pendingURLs.getSize(); i++)
    {
        frontier.push(pendingURLs[i]);
    }

    DynamicArray<string> completedURLs =
        database.loadCompletedURLs();

    for(int i = 0; i < completedURLs.getSize(); i++)
    {
        seenStore.insert(completedURLs[i]);
    }
}

bool PageStorage::addPendingURL(const string& url,int depth)
{
    return database.insertPendingURL(
        url,
        depth
    );
}

bool PageStorage::storePage(
    const string& url,
    const string& html,
    int depth)
{
    int fileId =
        database.getNextFileId();

    if(fileId == -1)
    {
        return false;
    }

    string fileName =
        to_string(fileId) + ".page";

    ofstream pageFile(
        buildFilePath(fileName));

    if(!pageFile)
    {
        return false;
    }

    // Line 1 : URL
    pageFile << url << '\n';

    // Line 2 : Depth
    pageFile << depth << '\n';

    // Line 3 onwards : Raw HTML
    pageFile << html;

    pageFile.close();

    if(!database.markPageStored(
            url,
            fileId,
            fileName))
    {
        fs::remove(
            buildFilePath(fileName));

        return false;
    }

    return true;
}

string PageStorage::getPage(
    const string& url)
{
    string fileName =
        database.getFileName(url);

    if(fileName.empty())
    {
        return "";
    }

    ifstream pageFile(
        buildFilePath(fileName));

    if(!pageFile)
    {
        return "";
    }

    string line;

    // Skip URL
    getline(pageFile, line);

    // Skip Depth
    getline(pageFile, line);

    stringstream buffer;
    buffer << pageFile.rdbuf();

    return buffer.str();
}

bool PageStorage::hasPage(
    const string& url)
{
    string fileName =
        database.getFileName(url);

    return !fileName.empty() &&
           fs::exists(
               buildFilePath(fileName));
}

int PageStorage::pageCount()
{
    return database.getStoredPageCount();
}

string PageStorage::getURLByID(
    int id)
{
    return database.getURLByID(id);
}