# PageStorage Design Proposal

> **Component:** PageStorage
> **Project:** Web Crawler

---

# Overview

The **PageStorage** component is responsible for persistently storing every successfully crawled web page so that it can later be consumed by the **Indexer** (Project 03).

Each crawled page is stored as an individual file, while a separate **index file** maintains the mapping between URLs and their corresponding page files.

The component provides a simple interface for storing pages, retrieving pages by URL, checking whether a page exists, and reporting the number of stored pages.

---

# Responsibilities

The PageStorage component is responsible for:

* Persistently storing every crawled page.
* Creating and maintaining the storage directory.
* Creating and maintaining the index file.
* Retrieving stored pages using their URL.
* Reporting the total number of stored pages.

The PageStorage component is **not** responsible for:

* Fetching web pages.
* Extracting hyperlinks.
* Duplicate detection.
* URL validation or normalization.
* HTML parsing.
* Index generation.

---

# Storage Architecture

The storage directory contains one index file and one file for every stored page.

```text
storage/
│
├── index.txt
│
├── 1.page
├── 2.page
├── 3.page
├── 4.page
└── ...
```

The storage layout is persistent and remains available even after the crawler terminates.

---

# Design Decision

The storage mechanism follows the **one-file-per-page** approach.

Each page is assigned a unique numeric identifier beginning from **1**.

The numeric identifier is used as the filename instead of the URL.

This design avoids problems such as:

* Invalid filename characters (`/`, `?`, `:`, `*`, etc.).
* Extremely long URLs exceeding filename limits.
* Platform-specific filesystem restrictions.
* URL sanitization complexity.

---

# Index File Format

The file `index.txt` stores lightweight metadata for every stored page.

Each line has the following format:

```text
<FileID>|<URL>|<Depth>
```

Example:

```text
1|https://books.toscrape.com|0
2|https://books.toscrape.com/catalogue/page-2.html|1
3|https://books.toscrape.com/catalogue/a-light-in-the-attic_1000/index.html|2
```

The index file stores:

* File Identifier
* URL
* Crawl Depth

The HTML content is **not** stored inside the index.

---

# Page File Format

Each page is stored inside an individual file.

Example:

```
2.page
```

Contents:

```text
https://books.toscrape.com/catalogue/page-2.html
1
<!DOCTYPE html>
<html>
...
</html>
```

Every page file contains exactly three sections:

1. URL
2. Crawl Depth
3. Raw HTML Content

This format is compatible with the project requirements.

---

# Class Members

```cpp
private:

std::string storageDirectory;

std::string indexFilePath;

int pageCount;
```

The component does **not** maintain a persistent file stream.

Files are opened only when an operation requires them and are immediately closed afterwards.

---

# Constructor

```cpp
PageStorage(const std::string& storageDirectory);
```

The constructor performs the following tasks:

1. Stores the storage directory path.
2. Creates the storage directory if it does not already exist.
3. Creates `index.txt` if it does not exist.
4. Reads `index.txt`.
5. Counts the number of stored pages.
6. Initializes `pageCount`.

After construction, the object is immediately ready for use.

---

# Runtime Initialization

When the crawler starts, the runtime state is reconstructed from persistent storage.

```
Crawler Starts

        │
        ▼

PageStorage Constructor

        │
        ▼

Read index.txt

        │
        ▼

Count stored entries

        │
        ▼

Initialize pageCount

        │
        ▼

Ready
```

Since the storage is persistent, the crawler correctly resumes with the current number of stored pages even after restarting.

---

# storePage()

Algorithm:

```
FileID = pageCount + 1

        │

Create FileID.page

        │

Write:
    URL
    Depth
    HTML

        │

Append

FileID|URL|Depth

to index.txt

        │

pageCount++
```

Time Complexity:

```
O(1)
```

---

# getPage()

Algorithm:

```
Open index.txt

        │

Search for matching URL

        │

Retrieve FileID

        │

Open FileID.page

        │

Skip URL

Skip Depth

        │

Read HTML

        │

Return HTML
```

Time Complexity:

* Best Case: **O(1)**
* Average Case: **O(n)**
* Worst Case: **O(n)**

where **n** is the number of stored pages.

---

# hasPage()

Algorithm:

```
Open index.txt

        │

Search URL sequentially

        │

Found?

 ├── Yes → true
 └── No  → false
```

Time Complexity:

* Best Case: **O(1)**
* Average Case: **O(n)**
* Worst Case: **O(n)**

---

# pageCount()

The value is maintained as a member variable.

```cpp
return pageCount;
```

Time Complexity:

```
O(1)
```

---

# Program Termination

When the crawler terminates:

* All runtime variables are destroyed.
* The operating system releases all memory.
* The storage directory remains unchanged.

Persistent files:

```text
storage/
│
├── index.txt
├── 1.page
├── 2.page
├── ...
```

When the crawler starts again, the constructor rebuilds the runtime state by reading `index.txt`.

---

# Advantages

* Persistent storage across program executions.
* One page per file simplifies inspection and debugging.
* Numeric filenames eliminate URL-to-filename conversion issues.
* No filename length limitations.
* No invalid filename characters.
* Simple storage layout.
* Easy integration with the Indexer.
* Runtime state is reconstructed automatically after restart.
* Does not require any external database.

---

# Limitations

* `getPage()` performs sequential searching through `index.txt`.
* `hasPage()` also performs sequential searching.
* Retrieval performance decreases as the number of stored pages increases.
* The current implementation assumes stored pages are never deleted.
* The component is designed for single-threaded execution.

---

# Trade-off Analysis

| Option                         | Advantages                                                                                  | Disadvantages                                                                  |
| ------------------------------ | ------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------ |
| **One File Per Page (Chosen)** | Persistent, easy to inspect, simple debugging, avoids URL filename issues using numeric IDs | URL lookup requires scanning `index.txt`                                       |
| **Single Structured File**     | Compact storage, only one file                                                              | Parsing is more complex, random access is difficult                            |
| **In-Memory HashMap**          | Very fast lookup                                                                            | Data is lost when the program terminates and cannot support persistent storage |

The **one-file-per-page** strategy was selected because it provides persistent storage, a simple implementation, easy debugging, and naturally satisfies the project requirements while avoiding filesystem issues associated with URL-based filenames.

---

# Future Improvements

The current design is intentionally simple and suitable for the expected project size.

Possible future enhancements include:

* Building an in-memory URL index during startup to achieve average **O(1)** lookups.
* Supporting deletion of stored pages.
* Replacing text-based storage with a database backend.
* Supporting concurrent read and write operations.
* Compressing stored HTML to reduce disk usage.
