# PageStorage Design Proposal

> **Component:** PageStorage
> **Project:** SuperCoders – Project 02: Web Crawler

---

# Overview

The **PageStorage** component is responsible for the persistent storage of all successfully crawled web pages during a crawl session. It serves as the storage layer of the crawler by recording the URL, crawl depth, and corresponding HTML content of each downloaded page. The stored pages are intended to be consumed by later stages of the project, particularly the Indexer introduced in Project 03.

The current implementation uses a **single structured text file** as the storage medium. This approach was selected because it is simple to implement, requires no external database dependency, and allows the stored data to be inspected manually during development and debugging. Each stored page is represented as a separate record containing the URL, crawl depth, HTML content, and a custom page delimiter that separates consecutive page records.

PageStorage has a clearly defined responsibility within the crawler architecture. It is responsible only for storing and retrieving crawled pages. It does not perform duplicate URL detection, HTML parsing, URL validation, or page downloading. Duplicate detection is handled entirely by the **Seen URL Store** before a page reaches the storage component.

Although the current implementation is file-based, the public interface is intentionally designed to remain independent of the underlying storage mechanism. In future versions of the crawler, the structured text file can be replaced by a database such as MongoDB without requiring changes to the crawler or any other component that interacts with the PageStorage interface. This design promotes low coupling, maintainability, and future extensibility.

---

# Section 1 – Public API

The responsibility of the **PageStorage** component is to permanently store every successfully crawled web page so that it can later be consumed by the Indexer. The component is independent of crawling logic, duplicate detection, URL extraction, and page fetching. It only manages page persistence and retrieval.

## Public Interface

```cpp
class PageStorage {
public:
    void storePage(std::string url,
                   std::string html,
                   int depth);

    std::string getPage(std::string url);

    bool hasPage(std::string url);

    int pageCount();
};
```

## Method Descriptions

### `storePage(string url, string html, int depth)`

Stores a successfully crawled page into the storage file. The page is appended sequentially together with its URL and crawl depth.

PageStorage assumes that duplicate URLs have already been filtered by the Seen URL Store.

---

### `getPage(string url)`

Searches the storage file for the specified URL and returns the complete HTML content associated with that URL.

If the URL is not present, an empty string is returned.

---

### `hasPage(string url)`

Checks whether the specified page exists in storage.

Internally, this method calls `getPage(url)` and returns:

* `true` if HTML content is found.
* `false` otherwise.

---

### `pageCount()`

Returns the number of successfully stored pages during the current crawl session.

The count is maintained internally and therefore executes in constant time.

---

# Section 2 – Internal Representation

## Component Responsibility

PageStorage is responsible only for storing and retrieving crawled pages.

It **does not**

* detect duplicate URLs,
* validate URLs,
* download web pages,
* parse HTML,
* extract hyperlinks.

Duplicate detection is completely handled by the **Seen URL Store** before `storePage()` is invoked.

---

## Storage Medium

The current implementation stores pages inside a **single structured text file**.

Reasons for choosing this approach:

* Simple implementation.
* Easy to debug.
* Human-readable format.
* No external dependencies.
* Suitable for the current project requirements.
* Easily replaceable by a database in future.

---

## File Ownership

PageStorage completely manages the storage file.

Its responsibilities include:

* creating the storage file,
* clearing any previous crawl data before a new crawl begins,
* opening and closing the file,
* writing page records,
* reading stored page records.

The crawler never directly manipulates the storage file.

---

## Internal Data Members

```cpp
private:
    std::fstream storageFile;
    std::string storageFileName;
    int totalPages;
```

### Description

* **storageFile** – File stream used for page storage.
* **storageFileName** – Name or path of the storage file.
* **totalPages** – Number of successfully stored pages.

---

## Storage Format

Each page is stored sequentially in the following format:

```text
URL
Depth
HTML Content
<<<PAGE_END>>>
```

Example:

```text
https://example.com
0
<html>
...
</html>
<<<PAGE_END>>>

https://google.com
1
<html>
...
</html>
<<<PAGE_END>>>
```

The delimiter `<<<PAGE_END>>>` marks the end of one stored page.

### Why a Custom Delimiter?

Although many websites return complete HTML documents, some servers may return malformed or incomplete HTML that does not contain the closing `</html>` tag.

Using a custom delimiter allows PageStorage to reliably identify the end of each stored page regardless of the content returned by the Fetcher.

---

## Internal Algorithms

### storePage()

1. Open the storage file in append mode.
2. Write the URL.
3. Write the crawl depth.
4. Write the complete HTML content.
5. Write the page delimiter `<<<PAGE_END>>>`.
6. Increment `totalPages`.
7. Flush the output stream.

---

### getPage()

1. Open the storage file.
2. Search sequentially for the requested URL.
3. Skip the stored depth.
4. Read lines until `<<<PAGE_END>>>` is encountered.
5. Return the collected HTML.
6. Return an empty string if the URL is not found.

---

### hasPage()

```cpp
return !getPage(url).empty();
```

---

### pageCount()

```cpp
return totalPages;
```

---

## Memory Representation

### PageStorage Object

```text
+--------------------------------+
|          PageStorage           |
+--------------------------------+
| storageFile                    |
| storageFileName                |
| totalPages                     |
+--------------------------------+
```

### Storage File

```text
+---------------------------------------------------+
| URL                                               |
| Depth                                             |
| HTML                                              |
| <<<PAGE_END>>>                                    |
| URL                                               |
| Depth                                             |
| HTML                                              |
| <<<PAGE_END>>>                                    |
| URL                                               |
| Depth                                             |
| HTML                                              |
| <<<PAGE_END>>>                                    |
+---------------------------------------------------+
```

---

# Section 3 – Failure Handling

## Duplicate URLs

Duplicate URL detection is **not** performed by PageStorage.

The crawler verifies every discovered URL using the Seen URL Store before calling `storePage()`. Therefore, PageStorage assumes that every incoming page is unique.

---

## Invalid URL

Invalid URLs are rejected before reaching the storage component.

PageStorage performs no URL validation.

---

## Download Failure

If the Fetcher fails to retrieve a page, `storePage()` is never called.

---

## Empty HTML

If the Fetcher returns an empty HTML response, the page is still stored because it represents the actual server response.

---

## Malformed HTML

PageStorage performs no validation or correction of HTML.

It stores exactly the data returned by the Fetcher.

---

## File Errors

If the storage file cannot be opened or written, the operation fails and an appropriate error is reported to the crawler.

---

# Section 4 – Complexity Analysis

| Operation     | Best | Average | Worst | Explanation                            |
| ------------- | ---- | ------- | ----- | -------------------------------------- |
| `storePage()` | O(1) | O(1)    | O(1)  | Sequential append operation            |
| `getPage()`   | O(1) | O(n)    | O(n)  | Sequential search through stored pages |
| `hasPage()`   | O(1) | O(n)    | O(n)  | Calls `getPage()` internally           |
| `pageCount()` | O(1) | O(1)    | O(1)  | Returns maintained counter             |

Where **n** represents the number of stored pages.

---

# Section 5 – Future Compatibility

The PageStorage interface is intentionally independent of the storage implementation.

The crawler interacts only with the following public interface:

```cpp
storePage(...)
getPage(...)
hasPage(...)
pageCount(...)
```

Currently, pages are stored in a structured text file.

In future versions of the crawler, the internal implementation can be replaced with **MongoDB** while preserving the same public interface.

```text
Current Design

Crawler
    │
    ▼
PageStorage
    │
    ▼
Structured Text File


Future Design

Crawler
    │
    ▼
PageStorage
    │
    ▼
MongoDB
```

Since only the internal implementation changes, the crawler does not require any modifications. This minimizes coupling between components and allows the storage mechanism to evolve without affecting the rest of the system.

---

# Advantages

* Simple and easy to implement.
* Human-readable storage format.
* No external database dependency.
* Easy to debug and inspect.
* Supports future migration to MongoDB.
* Clear separation of responsibilities.

---

# Limitations

* Retrieval requires sequential file traversal.
* Performance decreases as the number of stored pages grows.
* Designed for a single-threaded crawler.
* Not intended for very large-scale storage.
* Does not support concurrent read/write operations.
