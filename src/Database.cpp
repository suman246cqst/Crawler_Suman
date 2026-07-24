#include "../include/Database.h"
#include <iostream>

Database::Database()
{
    db = nullptr;
}

Database::~Database()
{
    close();
}

bool Database::open(const string& databaseName)
{
    int result = sqlite3_open(databaseName.c_str(), &db);

    if(result != SQLITE_OK)
    {
        cout << "Failed to open database: "
             << sqlite3_errmsg(db)
             << endl;

        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    cout << "Database connected successfully." << endl;
    return true;
}

void Database::close()
{
    if(db != nullptr)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::execute(const string& query)
{
    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        db,
        query.c_str(),
        nullptr,
        nullptr,
        &errorMessage
    );

    if(result != SQLITE_OK)
    {
        cout << "SQL Error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
        return false;
    }

    return true;
}

sqlite3* Database::getConnection()
{
    return db;
}

bool Database::createURLTable()
{
    string sql =
        "CREATE TABLE IF NOT EXISTS urls("
        "url TEXT PRIMARY KEY,"
        "fileId INTEGER,"
        "fileName TEXT,"
        "depth INTEGER,"
        "status TEXT NOT NULL "
        "CHECK(status IN ('PENDING','DONE'))"
        ");";

    char* error = nullptr;

    int rc = sqlite3_exec(
        db,
        sql.c_str(),
        nullptr,
        nullptr,
        &error
    );

    if(rc != SQLITE_OK)
    {
        cout << "SQL Error : "
             << error
             << endl;

        sqlite3_free(error);
        return false;
    }

    return true;
}

int Database::getNextFileId()
{
    string sql =
        "SELECT IFNULL(MAX(fileId),0) + 1 "
        "FROM urls;";

    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr)
        != SQLITE_OK)
    {
        return -1;
    }

    int nextId = -1;

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        nextId =
            sqlite3_column_int(stmt,0);
    }

    sqlite3_finalize(stmt);

    return nextId;
}

bool Database::insertPendingURL(
    const string& url,
    int depth)
{
    string sql =
        "INSERT OR IGNORE INTO urls "
        "(url,fileId,fileName,depth,status) "
        "VALUES(?,NULL,NULL,?,'PENDING');";

    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr)
        != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        stmt,
        1,
        url.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_int(
        stmt,
        2,
        depth
    );

    bool success =
        sqlite3_step(stmt)
        == SQLITE_DONE;

    sqlite3_finalize(stmt);

    return success;
}

bool Database::markPageStored(
    const string& url,
    int fileId,
    const string& fileName)
{
    string sql =
        "UPDATE urls "
        "SET fileId=?, "
        "fileName=?, "
        "status='DONE' "
        "WHERE url=?;";

    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr)
        != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int(
        stmt,
        1,
        fileId
    );

    sqlite3_bind_text(
        stmt,
        2,
        fileName.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    sqlite3_bind_text(
        stmt,
        3,
        url.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    bool success =
        sqlite3_step(stmt)
        == SQLITE_DONE;

    sqlite3_finalize(stmt);

    return success;
}

bool Database::pageExists(
    const string& url)
{
    string sql =
        "SELECT 1 "
        "FROM urls "
        "WHERE url=? "
        "AND status='DONE';";

    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr)
        != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(
        stmt,
        1,
        url.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    bool exists =
        sqlite3_step(stmt)
        == SQLITE_ROW;

    sqlite3_finalize(stmt);

    return exists;
}


DynamicArray<URLDepth> Database::loadPendingURLs()
{
    DynamicArray<URLDepth> pendingURLs;

    string sql =
        "SELECT url, depth "
        "FROM urls "
        "WHERE status='PENDING';";

    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr)
        != SQLITE_OK)
    {
        return pendingURLs;
    }

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        string url =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt,0));

        int depth =
            sqlite3_column_int(stmt,1);

        pendingURLs.push_back(
            URLDepth(url,depth));
    }

    sqlite3_finalize(stmt);

    return pendingURLs;
}

DynamicArray<string> Database::loadCompletedURLs()
{
    DynamicArray<string> completedURLs;

    string sql =
        "SELECT url "
        "FROM urls "
        "WHERE status='DONE';";

    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr)
        != SQLITE_OK)
    {
        return completedURLs;
    }

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        string url =
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt,0));

        completedURLs.push_back(url);
    }

    sqlite3_finalize(stmt);

    return completedURLs;
}


string Database::getFileName(
    const string& url)
{
    string sql =
        "SELECT fileName "
        "FROM urls "
        "WHERE url=? "
        "AND status='DONE';";

    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr)
        != SQLITE_OK)
    {
        return "";
    }

    sqlite3_bind_text(
        stmt,
        1,
        url.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    string fileName = "";

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char* text =
            sqlite3_column_text(stmt,0);

        if(text != nullptr)
        {
            fileName =
                reinterpret_cast<const char*>(text);
        }
    }

    sqlite3_finalize(stmt);

    return fileName;
}


int Database::getStoredPageCount()
{
    string sql =
        "SELECT COUNT(*) "
        "FROM urls "
        "WHERE status='DONE';";

    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr)
        != SQLITE_OK)
    {
        return 0;
    }

    int count = 0;

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        count =
            sqlite3_column_int(stmt,0);
    }

    sqlite3_finalize(stmt);

    return count;
}


string Database::getURLByID(int id)
{
    string sql =
        "SELECT url "
        "FROM urls "
        "WHERE fileId=? "
        "AND status='DONE';";

    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr)
        != SQLITE_OK)
    {
        return "";
    }

    sqlite3_bind_int(
        stmt,
        1,
        id
    );

    string url = "";

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char* text =
            sqlite3_column_text(stmt,0);

        if(text != nullptr)
        {
            url =
                reinterpret_cast<const char*>(text);
        }
    }

    sqlite3_finalize(stmt);

    return url;
}