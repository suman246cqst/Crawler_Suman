# SeenStore Design Proposal - Version 2

# Overview

The **SeenStore** is the completed URL tracking component of the web crawler.

Its responsibility is to maintain URLs that have already been successfully crawled, allowing the crawler to avoid downloading and processing the same page multiple times.

Each URL is inserted into the SeenStore only after the corresponding page has been successfully stored by **PageStorage**.

Before processing any URL, the crawler checks the SeenStore to determine whether that URL has already been completed.

Internally, the SeenStore is an in-memory hash table implemented using:

```cpp
HashMap<std::string, bool>
```

The boolean value is used only to represent the existence of a URL.

The SeenStore does **not** provide persistent storage.

Completed URLs are stored permanently by **PageStorage** through the SQLite database. During crawler startup, PageStorage restores previously completed URLs from persistent storage and inserts them into the SeenStore.

The SeenStore has a single responsibility:

> Maintain fast in-memory lookup of completed crawl URLs.

It does **not** perform:

- URL normalization
- URL scheduling
- HTML downloading
- HTML parsing
- Page storage
- Database operations
- Crawl recovery

These responsibilities belong to other crawler components.

---

# Section 1 — Public API

```cpp
class SeenStore
{
private:

    HashMap<std::string, bool> seenURLs;

public:

    bool contains(std::string& url);

    void insert(std::string& url);

    void clear();

    int count();
};
```

---

## Design Justification

The SeenStore intentionally exposes only four operations because completed URLs only require membership testing and insertion.

### contains()

```cpp
bool contains(std::string& url);
```

Checks whether a URL has already been successfully crawled.

The crawler performs this lookup before downloading a page.

Example:

```cpp
if(seen.contains(url))
{
    continue;
}
```

---

### insert()

```cpp
void insert(std::string& url);
```

Adds a successfully crawled URL into the SeenStore.

This operation is performed only after PageStorage successfully stores the page.

Example:

```cpp
seen.insert(current.url);
```

---

### clear()

```cpp
void clear();
```

Removes every stored URL from memory.

This operation is useful when resetting crawler state during testing.

---

### count()

```cpp
int count();
```

Returns the number of completed URLs currently stored in memory.

Useful for monitoring crawler progress.

---

The SeenStore intentionally does **not** provide:

- URL removal
- URL iteration
- URL persistence
- URL normalization

because those responsibilities belong to other crawler components.

---

# Section 2 — Internal Representation

The SeenStore is implemented using the HashMap developed in Project 01.

```
Crawler
    |
    v
SeenStore
    |
    v
HashMap<std::string,bool>
    |
    v
Bucket Array
    |
    v
Linked Lists
```

---

## Memory Layout

```
+--------------------------------------+
|             SeenStore                |
+--------------------------------------+
| HashMap<std::string,bool>            |
+--------------------------------------+
                 |
                 v

+--------------------------------------+
| Bucket Array                         |
+--------------------------------------+
| Bucket 0                             |
| Bucket 1                             |
| Bucket 2                             |
| ...                                  |
+--------------------------------------+

                 |
                 v

+--------------------------------------+
| HashMap Node                         |
+--------------------------------------+
| key   : URL                          |
| value : true                         |
| next  : pointer                      |
+--------------------------------------+
```

Each stored entry contains:

```
URL
+
Visited Flag (true)
+
Next Pointer
```

The SeenStore relies entirely on the HashMap implementation for hashing, collision handling, memory allocation, and resizing.

---

## Runtime Relationship

```
Crawler
    |
    v
SeenStore
    |
    v
HashMap
```

The crawler performs duplicate detection by querying the SeenStore before processing each URL.

---

## Persistent Recovery

The SeenStore itself performs no database operations.

During crawler startup:

```
Crawler
      |
      v
PageStorage
      |
recoverCrawlerState()
      |
      +----------------------+
      |                      |
      v                      v
Frontier               SeenStore
```

`PageStorage::recoverCrawlerState()` loads completed URLs from the SQLite database and restores them into the SeenStore before crawling resumes.

This separation keeps the SeenStore focused solely on fast in-memory duplicate detection.

---

# Section 3 — Failure Handling

## Duplicate URL

If a URL already exists:

```cpp
seen.contains(url)
```

returns

```
true
```

The crawler skips downloading the page.

No duplicate entry is inserted.

---

## Invalid URL

The SeenStore assumes every URL has already been normalized.

Validation is performed by:

```
URLNormalizer
```

before the URL reaches the SeenStore.

---

## Empty SeenStore

If no URLs have been stored:

```cpp
contains(url)
```

returns

```
false
```

No exception occurs.

---

## Memory Allocation Failure

Insertion into the underlying HashMap may require allocating memory.

If allocation fails:

- the HashMap reports the failure;
- existing entries remain unchanged.

---

## Hash Collision

Hash collisions are completely handled by the underlying HashMap implementation.

The SeenStore does not implement:

- hashing
- collision resolution
- bucket management
- rehashing

---

## Recovery Failure

If PageStorage cannot restore completed URLs during crawler startup, the SeenStore remains empty.

The crawler simply begins a fresh crawl.

---

# Section 4 — Complexity Analysis

Let

```
n = number of completed URLs
```

| Operation | Best Case | Average Case | Worst Case |
|-----------|-----------|--------------|------------|
| contains() | O(1) | O(1) | O(n) |
| insert() | O(1) | O(1) | O(n) |
| clear() | O(n) | O(n) | O(n) |
| count() | O(1) | O(1) | O(1) |

---

## Explanation

### contains()

Uses HashMap lookup.

Average:

```
O(1)
```

Worst:

```
O(n)
```

because all keys may collide into a single bucket.

---

### insert()

Uses HashMap insertion.

Average:

```
O(1)
```

Worst:

```
O(n)
```

because of collision chains.

---

### clear()

Every stored URL must be removed.

Therefore:

```
O(n)
```

---

### count()

The HashMap maintains its current size.

Therefore:

```
O(1)
```

---

## Space Complexity

```
O(n)
```

where **n** is the number of completed URLs stored in memory.

---

# Section 5 — Future Compatibility

The SeenStore is independent of the persistence mechanism.

Only the crawler communicates with the SeenStore through its public API, allowing its internal implementation to evolve without affecting other crawler components.

Possible future improvements include:

## Bloom Filter

Replace the HashMap with a Bloom Filter to reduce memory usage during very large crawls.

---

## Distributed Duplicate Detection

Multiple crawler instances may share a centralized duplicate detection service.

```
Crawler 1
      |
Crawler 2
      |
Distributed SeenStore
      |
Crawler 3
```

---

## Thread-Safe SeenStore

Future versions may synchronize operations to support concurrent crawler workers.

---

## Alternative Data Structures

The HashMap may be replaced by another lookup structure without changing the crawler interface, provided the public API remains unchanged.