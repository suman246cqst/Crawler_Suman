# Build Log — Session 2

**Date:** July 23, 2026

**Duration:** 2 hours

**Goal:**
Update the `PageStorage` implementation according to the finalized persistence architecture and remove unnecessary coupling between storage and database management.

**Problem:**
The previous `PageStorage` design required the database object to be passed externally.

The earlier design was:

```cpp
PageStorage(
    string storagePath,
    Database& database
);
```

This approach caused several design issues:

* `PageStorage` depended on another component being initialized correctly before use.
* The crawler had to manage database lifetime even though database operations were only related to storage.
* Testing became more complicated because every PageStorage test required manually creating and configuring a Database object.
* The storage component was not self-contained.

Another issue was that the persistent crawler state was not completely handled by PageStorage. It only stored HTML files, while pending URL and completed URL recovery logic was becoming distributed across multiple components.

**What I Tried:**

* Reviewed the responsibilities of PageStorage and Database.
* Removed external database ownership from the PageStorage design.
* Moved database initialization inside PageStorage.
* Updated the constructor to make PageStorage responsible for creating its own storage environment.

The previous flow:

```text
Crawler
   |
   ├── Creates Database
   |
   └── Passes Database to PageStorage
```

was replaced with:

```text
Crawler
   |
   └── Creates PageStorage
             |
             ├── Creates Storage Directory
             |
             ├── Opens Database
             |
             └── Creates Required Tables
```

Implemented storage initialization:

```cpp
PageStorage::PageStorage()
{
    createStorageDirectory();

    database.open(
        "../CrawlerStorage/crawler.db"
    );

    database.createURLTable();
}
```

Implemented persistent recovery support:

```cpp
recoverCrawlerState(
    Frontier& frontier,
    SeenStore& seenStore
);
```

The recovery workflow was designed as:

```text
Database
   |
   ├── Pending URLs
   |        |
   |        ▼
   |     Frontier
   |
   └── Completed URLs
            |
            ▼
        SeenStore
```

Also finalized the page storage format:

```
Page File

Line 1 : URL
Line 2 : Depth
Line 3+: Raw HTML
```

Example:

```
https://example.com
2
<html>
    ...
</html>
```

**Outcome:**
`PageStorage` became an independent persistent storage component.

The final implementation responsibilities were:

* Create and maintain crawler storage directory.
* Manage SQLite database internally.
* Store raw HTML pages on filesystem.
* Store page metadata in database.
* Retrieve stored HTML using URL mapping.
* Restore crawler state after restart.


