# PageStorage Design Proposal - Version 2

# Overview

The **PageStorage** component is responsible for providing the persistent storage layer of the web crawler.

It permanently stores successfully crawled web pages and maintains the crawler's persistent state so that crawling can resume after program termination or system failure.

PageStorage serves two primary purposes:

1. **Persistent Crawling**
   - Stores newly discovered URLs.
   - Restores pending and completed crawler state after restart.
   - Stores successfully crawled pages.

2. **Future Indexer Support**
   - Provides sequential access to stored pages.
   - Allows Project 03 (Indexer) to retrieve pages for indexing.

To achieve these goals, PageStorage combines two persistent storage mechanisms.

1. **SQLite Database**
   - Stores crawler metadata.
   - Maintains pending and completed URLs.
   - Maps page IDs to filenames.
   - Supports crawler recovery.

2. **File System**
   - Stores the complete page contents.
   - Each page is stored as an individual file.

Separating metadata from page content keeps the database compact while allowing very large HTML documents to be stored efficiently.

---

# Responsibilities

PageStorage is responsible for:

- Creating the crawler storage directory.
- Opening the crawler database.
- Creating required database tables.
- Restoring crawler state after restart.
- Persistently storing newly discovered URLs.
- Persistently storing successfully crawled pages.
- Mapping page IDs to stored files.
- Retrieving stored HTML pages.
- Checking whether a page has already been stored.
- Reporting the number of stored pages.
- Providing page access for the future Indexer.

PageStorage does **not** perform:

- Downloading web pages.
- URL normalization.
- HTML parsing.
- Link extraction.
- Duplicate detection.
- Crawl scheduling.

These responsibilities belong to other crawler components.

| Responsibility | Component |
|---------------|-----------|
| Download HTML | Fetcher |
| URL Normalization | URLNormalizer |
| Link Extraction | LinkExtractor |
| Duplicate Detection | SeenStore |
| Scheduling | Frontier |

---

# Section 1 — Public API

```cpp
class PageStorage
{
private:

    string storageDirectory;

    Database database;

private:

    void createStorageDirectory();

    string buildFilePath(
        const string& fileName) const;

public:

    PageStorage();

    void recoverCrawlerState(
        Frontier& frontier,
        SeenStore& seenStore);

    bool addPendingURL(
        const string& url,
        int depth);

    bool storePage(
        const string& url,
        const string& html,
        int depth);

    int pageCount();

    string getURLByID(
        int id);

    string getPage(
        const string& url);

    bool hasPage(
        const string& url);
};
```

---

## Design Justification

Unlike placing database management inside the crawler, PageStorage encapsulates all persistent storage operations.

Advantages:

- Storage implementation remains independent from crawler logic.
- The crawler communicates through a small storage API.
- Database implementation can later be replaced without affecting crawler code.
- Future Indexer uses the same interface.

---

## Constructor

```cpp
PageStorage();
```

Responsibilities:

- Creates storage directory.
- Opens SQLite database.
- Creates required tables.

Implementation:

```
Create Storage Directory

        |

        v

Open Database

        |

        v

Create URL Table
```

---

## recoverCrawlerState()

```cpp
void recoverCrawlerState(
    Frontier& frontier,
    SeenStore& seenStore);
```

Restores crawler state after restart.

Execution:

```
Load Pending URLs

        |

        v

Insert into Frontier

        |

        v

Load Completed URLs

        |

        v

Insert into SeenStore
```

This enables persistent crawling without requiring the crawler to understand database operations.

---

## addPendingURL()

```cpp
bool addPendingURL(
    const string& url,
    int depth);
```

Stores a newly discovered URL in persistent storage.

Database status:

```
PENDING
```

The URL is restored during the next crawler execution if it has not yet been processed.

---

## storePage()

```cpp
bool storePage(
    const string& url,
    const string& html,
    int depth);
```

Stores a successfully downloaded page.

Execution flow:

```
Get Next File ID

        |

        v

Create File Name

        |

        v

Create Page File

        |

        v

Write URL

        |

        v

Write Depth

        |

        v

Write HTML

        |

        v

Update Database

PENDING

        |

        v

DONE
```

Each page file follows the required Project specification.

Example:

```
1.page

https://books.toscrape.com

0

<!DOCTYPE html>
<html>
...
```

Line 1

```
URL
```

Line 2

```
Depth
```

Remaining lines

```
Raw HTML
```

---

## getPage()

```cpp
string getPage(
    const string& url);
```

Retrieves the HTML associated with a URL.

Execution:

```
Database Lookup

        |

        v

Get File Name

        |

        v

Open File

        |

        v

Skip URL Line

        |

        v

Skip Depth Line

        |

        v

Read HTML

        |

        v

Return HTML
```

Only the raw HTML is returned.

---

## hasPage()

```cpp
bool hasPage(
    const string& url);
```

Returns whether the specified page has already been stored.

Checks:

1. Database mapping.
2. Physical file existence.

---

## pageCount()

```cpp
int pageCount();
```

Returns the total number of successfully stored pages.

Used by the future Indexer.

---

## getURLByID()

```cpp
string getURLByID(
    int id);
```

Returns the URL corresponding to a stored page ID.

Allows sequential traversal of stored pages.

---

# Section 2 — Internal Representation

## Overall Architecture

```
                  Crawler

                     |

                     v

               PageStorage

            +---------------+

            |               |

            v               v

      SQLite Database   File System
```

---

## Database

```
urls

+--------------------------------+

url

fileId

fileName

depth

status

+--------------------------------+
```

Example

```
https://example.com

1

1.page

0

DONE
```

Pending page

```
https://example.com/page2

NULL

NULL

1

PENDING
```

---

## File Storage

Each page is stored separately.

```
CrawlerStorage

|

+-- crawler.db

|

+-- 1.page

|

+-- 2.page

|

+-- 3.page
```

Example file

```
https://example.com

1

<!DOCTYPE html>
<html>
...
```

---

## Runtime Relationship

```
Crawler

      |

      v

PageStorage

      |

+-------------+

|             |

v             v

Database   Page Files
```

---

# Section 3 — Failure Handling

## Database Open Failure

If the database cannot be opened, storage initialization fails and persistent operations cannot continue.

---

## Directory Creation Failure

If the storage directory cannot be created, page storage fails.

---

## Duplicate URL

Duplicate prevention is handled before page storage.

Protection layers:

- SeenStore
- Frontier
- SQLite PRIMARY KEY

---

## Invalid URL

Invalid URLs are rejected by URLNormalizer before reaching PageStorage.

---

## File Creation Failure

If page creation fails,

```
return false;
```

Database remains unchanged.

---

## Database Update Failure

If database update fails after file creation,

```
Delete created file

Return false
```

This prevents orphan page files.

---

## Missing Stored File

If the database contains a filename but the file has been deleted,

```
return ""
```

---

## Missing URL

If the URL does not exist,

```
return ""
```

---

## Empty HTML

Empty HTML is still stored.

PageStorage stores the fetched result exactly as received.

---

# Section 4 — Complexity Analysis

Let

```
f

=

HTML size
```

| Operation | Best | Average | Worst |
|-----------|------|----------|--------|
| addPendingURL() | O(1) | O(1) | O(n) |
| recoverCrawlerState() | O(n) | O(n) | O(n) |
| storePage() | O(f) | O(f) | O(f) |
| getPage() | O(f) | O(f) | O(f) |
| hasPage() | O(1) | O(1) | O(n) |
| pageCount() | O(1) | O(1) | O(1) |
| getURLByID() | O(1) | O(1) | O(n) |

Space Complexity

```
Database

O(n)

Files

O(total HTML)
```

---

# Section 5 — Future Compatibility

Project 03 (Indexer) will consume PageStorage directly.

Required interface:

```cpp
storePage(url, html, depth);

getPage(url);

hasPage(url);

getURLByID(id);

pageCount();
```

Indexer workflow

```
count = pageCount()

        |

        v

for(id = 1; id <= count; id++)

        |

        v

url = getURLByID(id)

        |

        v

html = getPage(url)

        |

        v

Build Search Index
```

Because every stored page has a unique sequential ID, the Indexer can process pages efficiently without scanning the entire database.

---

# Advantages

- Fully persistent crawler.
- Supports crawler recovery after restart.
- Keeps metadata separate from page content.
- Sequential page IDs simplify indexing.
- Compatible with Project 03 requirements.
- Human-readable page files.
- Simple storage layout.
- Efficient page lookup.

---

# Limitations

- Single-threaded implementation.
- No page deletion.
- No HTML compression.
- Uses local filesystem storage.
- No transactional storage across file system and database.

---

# Future Improvements

Possible future enhancements include:

- Database transactions for atomic storage.
- Compressed page files.
- Concurrent page storage.
- Page deletion support.
- Database indexing optimizations.
- Distributed storage backend.
- Incremental page updates.