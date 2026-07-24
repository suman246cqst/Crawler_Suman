#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include "../../DS_Library/include/dynamicArray.h"
#include "Frontier.h"

using namespace std;

class Database
{
private:
    sqlite3* db;

public:
    Database();
    ~Database();

    // ==============================
    // Database Connection
    // ==============================

    bool open(const string& databaseName);
    void close();

    bool execute(const string& query);

    sqlite3* getConnection();

    // ==============================
    // Database Initialization
    // ==============================

    bool createURLTable();

    // ==============================
    // Pending URL Operations
    // ==============================

    bool insertPendingURL(
        const string& url,
        int depth);

    // ==============================
    // Stored Page Operations
    // ==============================

    int getNextFileId();

    bool markPageStored(
        const string& url,
        int fileId,
        const string& fileName);

    bool pageExists(
        const string& url);

    string getFileName(
        const string& url);

    string getURLByID(
        int id);

    int getStoredPageCount();

    // ==============================
    // Recovery Operations
    // ==============================

    DynamicArray<URLDepth> loadPendingURLs();

    DynamicArray<string> loadCompletedURLs();
};

#endif