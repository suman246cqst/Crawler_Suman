# Build Log — Session 1

**Date:** July 23, 2026

**Duration:** 2 hours

**Goal:**
Revise the crawler persistence design proposal and finalize the architecture before updating the implementation.

**Problem:**
During crawler design review, I identified that the initial persistence approach was not properly separating crawler state management responsibilities.

The first implementation design treated `SeenStore` as the main source of crawler progress. Previously crawled URLs were stored and restored, but after restarting the crawler, those URLs were immediately skipped. This prevented the crawler from expanding already downloaded pages and discovering deeper links.

The earlier `PageStorage` design also had unnecessary coupling with the database layer. Initially, I designed `PageStorage` to receive `Database` as a reference parameter:

```cpp
PageStorage(path, database);
```

The intention was to keep database management outside PageStorage, but this created dependency issues because the storage component required external initialization and the crawler had to manage database ownership.

While reviewing the complete crawler workflow, I realized that persistence should be handled internally by the storage layer instead of passing database objects between components.

**What I Tried:**

* Reviewed the crawler execution flow from seed URL processing to page storage.
* Traced the restart scenario where `SeenStore` was rebuilt from persistent data.
* Analyzed why previously stored pages were not contributing new URLs after restart.
* Compared the responsibilities of:

  * `Frontier`
  * `SeenStore`
  * `PageStorage`
  * `Database`
* Reviewed the previous PageStorage design where database dependency was injected externally.
* Redesigned the component boundaries to avoid unnecessary coupling.

The updated responsibility model became:

```
Frontier
    |
    └── Stores pending URLs

SeenStore
    |
    └── Stores completed URLs

PageStorage
    |
    ├── Stores HTML files
    ├── Manages database
    └── Recovers crawler state
```

**Outcome:**
Finalized the updated crawler persistence architecture.

The design was changed from:

```
Crawler
   |
   ├── Database
   |
   └── PageStorage(Database&)
```

to:

```
Crawler
   |
   └── PageStorage
          |
          ├── Database
          |
          └── File Storage
```

The finalized design decisions were:

* `PageStorage` owns database initialization and storage management.
* `Frontier` manages pending URLs only.
* `SeenStore` manages completed URLs only.
* Database maintains persistent crawler state.
* Recovery restores:

  * pending URLs into Frontier,
  * completed URLs into SeenStore.

