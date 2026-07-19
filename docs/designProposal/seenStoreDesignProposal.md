# SeenStore Design Proposal

## Overview

The **SeenStore** is responsible for maintaining the collection of URLs that have already been discovered by the crawler. Its primary purpose is to prevent the crawler from visiting the same web page multiple times during the crawling process.

Whenever the crawler extracts hyperlinks from a downloaded page, each URL is first normalized and then queried against the SeenStore. If the URL has not been encountered before, it is inserted into the SeenStore and scheduled by the Frontier. Otherwise, the duplicate URL is ignored.

Efficient duplicate detection is one of the most important operations in a web crawler because the same hyperlink may appear on hundreds or even thousands of different pages. Performing a linear search for every discovered URL would significantly reduce the crawler's performance.

To achieve fast lookup and insertion, the SeenStore reuses the **HashMap** developed in Project 01. The HashMap stores normalized URLs as keys. Since the crawler is interested only in whether a URL has already been discovered, each key is associated with a boolean value.

The SeenStore has a single responsibility: **tracking previously discovered URLs**. It does not download web pages, normalize URLs, schedule crawl order, parse HTML documents, or store page contents. These responsibilities belong to other crawler components.

---

# Section 1 – Public API

## Proposed API

```cpp
class SeenStore
{
public:
    bool contains(const std::string& url) const;

    void insert(const std::string& url);

    void clear();

    int count() const;

private:
    HashMap<std::string, bool> seenURLs;
};
```

### API Justification

The SeenStore exposes only the operations required for duplicate detection during crawling.

- `contains()` checks whether a URL has already been discovered.
- `insert()` records a newly discovered URL.
- `clear()` removes every stored URL when starting a new crawl.
- `count()` returns the total number of unique URLs currently stored.

Operations such as deletion of individual URLs, iteration over all URLs, or random access are intentionally omitted because they are not required by the crawler.

### HashMap Integration

The SeenStore internally composes the **HashMap** implementation developed in Project 01.

```cpp
HashMap<std::string, bool> seenURLs;
```

where

- **Key** → Normalized URL
- **Value** → `true`

The boolean value is stored only because the HashMap stores key-value pairs. The crawler is interested only in determining whether a URL already exists.

The SeenStore does **not** modify or extend the HashMap implementation. Instead, it provides a simplified interface specialized for duplicate URL detection while reusing the existing HashMap implementation without modification.

### Design Justification

A HashMap provides near constant-time lookup and insertion, making it well suited for duplicate detection.

Alternative data structures such as arrays or linked lists require linear searching, causing duplicate detection to become increasingly expensive as more URLs are discovered.

Since duplicate detection is performed for every extracted hyperlink, the HashMap provides significantly better performance.

---

# Section 2 – Internal Representation

The SeenStore is implemented by reusing the **HashMap** developed in Project 01.

```
Crawler
    │
    ▼
SeenStore
    │
    ▼
HashMap<string, bool>
```

The SeenStore does not implement its own storage mechanism. Instead, it delegates all storage, lookup, insertion, collision handling, rehashing, and memory management to the existing HashMap implementation.

Each normalized URL is stored as the **key** in the HashMap. Since the crawler only needs to determine whether a URL has already been discovered, each key is associated with a boolean value.

The internal memory layout of the HashMap, including its bucket array, linked lists, collision resolution mechanism, and node representation, remains identical to the implementation submitted in **Project 01** and is not modified for the crawler.


---

# Section 3 – Failure Handling

The SeenStore is responsible only for storing previously discovered URLs. URL validation and normalization are handled by other crawler components.

### Duplicate URL

If `insert()` is called with a URL that already exists in the SeenStore, no new entry is inserted.

The crawler continues execution normally.

### Invalid URL

The SeenStore assumes every URL has already been normalized and validated before insertion.

No validation is performed inside this component.

### Empty Store

Calling `contains()` on an empty SeenStore simply returns `false`.

No exception is generated.

### Memory Allocation Failure

If the underlying HashMap cannot allocate memory while inserting a new URL, the insertion fails and the exception is propagated to the crawler.

### Hash Collisions

Hash collisions are handled entirely by the HashMap implementation developed in Project 01.

The SeenStore delegates collision handling to the underlying HashMap and performs no additional processing.

---

# Section 4 – Complexity Analysis

The complexity of SeenStore operations depends directly on the underlying HashMap implementation from Project 01.

| Operation | Best | Average | Worst |
|-----------|------|----------|--------|
| `contains()` | O(1) | O(1) | O(n) |
| `insert()` | O(1) | O(1) | O(n) |
| `clear()` | O(n) | O(n) | O(n) |
| `count()` | O(1) | O(1) | O(1) |

### Justification

- `contains()` delegates the lookup operation to the HashMap.
- `insert()` delegates insertion to the HashMap.
- Hash collisions may increase the length of a collision chain, resulting in a worst-case complexity of **O(n)**.
- `clear()` removes all entries stored in the HashMap.
- `count()` returns the maintained size of the HashMap without traversing it.

---

# Section 5 – Future Compatibility

The SeenStore is designed as an independent duplicate-detection component.

Future improvements may include:

- Replacing the HashMap with a Bloom Filter for memory-efficient duplicate detection.
- Persistent SeenStore stored on disk for resumable crawling.
- Thread-safe implementation for concurrent crawlers.
- Distributed SeenStore shared across multiple crawler nodes.

These enhancements can be introduced without changing the public interface because the crawler interacts only through the SeenStore API. The current abstraction hides the underlying HashMap implementation, allowing the internal storage mechanism to be replaced in future versions without affecting the crawler logic.