# Crawler Design Proposal — Version 3

# Overview

The **Crawler** is the central coordinator of the web crawler. It orchestrates all crawler components and controls the complete crawling workflow from the initial seed URL until the crawl terminates.

Rather than implementing networking, parsing, scheduling, duplicate detection, or storage itself, the Crawler delegates these responsibilities to dedicated components while coordinating their interaction.

The crawler follows a **Breadth First Search (BFS)** traversal strategy using the **Frontier** component, ensuring that pages are processed level by level based on their crawl depth.

The crawler supports persistent execution through the **PageStorage** component. PageStorage internally manages persistent crawler state using the Database component and restores the crawler state whenever the crawler restarts.

During initialization:

- Previously completed URLs are restored into **SeenStore**.
- Previously pending crawl tasks are restored into **Frontier**.

The crawler itself never accesses the database directly.

---

## Component Responsibilities

| Component | Responsibility |
|------------|----------------|
| Fetcher | Downloads HTML documents |
| LinkExtractor | Extracts hyperlinks from HTML |
| URLNormalizer | Produces canonical URLs |
| Frontier | Maintains pending crawl tasks |
| SeenStore | Tracks completed URLs |
| PageStorage | Stores crawled pages and manages persistent crawler state |

The Crawler only coordinates these components.

It does **not** perform:

- HTTP communication
- HTML parsing
- URL normalization
- Duplicate detection
- Database management
- Page file management

---

# Section 1 — Public API

## Crawler Class

```cpp
class Crawler
{
private:

    Frontier frontier;

    SeenStore seen;

    PageStorage storage;

    Fetcher fetcherTool;

    LinkExtractor extractor;

    URLNormalizer normalizer;

    int maxDepth;

    int maxPages;

private:

    void initializeCrawler();

public:

    Crawler(
        int maxDepth,
        int maxPages);

    void crawl(
        string seedURL);
};
```

---

## Constructor

```cpp
Crawler(
    int maxDepth,
    int maxPages);
```

The constructor initializes the crawler configuration and runtime components.

Responsibilities:

- Store maximum crawl depth.
- Store maximum number of pages.
- Construct all crawler components.
- Restore previously saved crawler state.

Implementation:

```cpp
Crawler::Crawler(
    int maxDepth,
    int maxPages)
    :
    maxDepth(maxDepth),
    maxPages(maxPages)
{
    initializeCrawler();
}
```

The constructor does **not**:

- Open the database.
- Create database tables.
- Load URLs manually.

These responsibilities are delegated to **PageStorage**.

---

## initializeCrawler()

```cpp
void initializeCrawler();
```

Restores the crawler state from persistent storage.

Current implementation:

```cpp
storage.recoverCrawlerState(
    frontier,
    seen
);
```

The recovery process loads:

- Pending URLs into the Frontier.
- Completed URLs into the SeenStore.

Execution flow:

```
Crawler Starts

      |

      v

PageStorage

      |

      v

recoverCrawlerState()

      |

      +------------------------+

      |                        |

      v                        v

loadPendingURLs()       loadCompletedURLs()

      |                        |

      v                        v

Frontier                SeenStore
```

The crawler itself performs no direct persistence operations.

---

## crawl()

```cpp
void crawl(
    string seedURL);
```

Starts crawling from the supplied seed URL.

Execution flow:

1. Normalize the seed URL.
2. Ignore invalid URLs.
3. Ignore URLs already completed.
4. Ignore URLs already waiting in Frontier.
5. Store the seed URL as **PENDING**.
6. Insert the seed into Frontier.
7. Continue crawling until:
   - Frontier becomes empty, or
   - Maximum page limit is reached.

During crawling each URL is processed in FIFO order according to BFS traversal.

---

# Section 2 — Internal Representation

## Crawler Memory Layout

```
+-------------------------------------------+
|                 Crawler                   |
+-------------------------------------------+
| Frontier frontier                         |
| SeenStore seen                            |
| PageStorage storage                       |
| Fetcher fetcherTool                       |
| LinkExtractor extractor                   |
| URLNormalizer normalizer                  |
| int maxDepth                              |
| int maxPages                              |
+-------------------------------------------+
```

---

## Component Relationship

```
                     Crawler

                         |

--------------------------------------------------------

|          |             |            |               |

v          v             v            v               v

Fetcher  LinkExtractor  Frontier  SeenStore   PageStorage

                                         |

                                         v

                                  Database + Files
```

Each component owns a single responsibility and communicates only through its public interface.

---

## Runtime Crawling Workflow

```
Seed URL

    |

Normalize URL

    |

Already Seen?

    |

Already Pending?

    |

Store Pending URL

    |

Push Into Frontier

    |

============================

While Frontier Not Empty

============================

    |

Pop URLDepth

    |

Maximum Pages Reached?

    |

Fetch HTML

    |

Fetch Successful?

    |

Extract Links

    |

Normalize Links

    |

Filter Invalid URLs

    |

Filter Seen URLs

    |

Filter Pending URLs

    |

Store Pending URLs

    |

Push New URLs

    |

Store Current Page

    |

Insert Into SeenStore

    |

Repeat
```

---

## Runtime Coordination

```
                Crawler

                    |

-------------------------------------------------

|          |          |          |              |

v          v          v          v              v

Fetcher  Extractor  Frontier  SeenStore   PageStorage

                                   |

                                   v

                            Database + Files
```

The Crawler coordinates all operations while individual components remain independent.

---

# Section 3 — Failure Handling

## Invalid URL

Every discovered URL is normalized before scheduling.

If

```cpp
normalize(...)
```

returns an empty string,

the URL is ignored.

---

## Duplicate URL

Duplicate detection occurs at three different levels.

### Already Crawled

The crawler checks

```cpp
seen.contains(url)
```

If true,

the URL is skipped.

---

### Already Scheduled

The crawler checks

```cpp
frontier.contains(url)
```

URLs already waiting inside the Frontier are not scheduled again.

---

### Persistent Duplicate

When a new URL is discovered,

```cpp
storage.addPendingURL(...)
```

attempts to insert the URL into persistent storage.

If insertion fails,

the URL is not pushed into Frontier.

---

## Download Failure

If

```cpp
fetcherTool.fetch(...)
```

fails,

the crawler skips the page.

The URL remains incomplete and is not inserted into SeenStore.

---

## Storage Failure

If

```cpp
storage.storePage(...)
```

returns false,

the crawler does not insert the URL into SeenStore.

The page is therefore not considered successfully crawled.

---

## Maximum Depth

Before scheduling new URLs,

the crawler checks

```cpp
current.depth + 1 > maxDepth
```

URLs exceeding the configured depth are ignored.

---

## Maximum Pages

Before processing each URL,

the crawler checks

```cpp
storage.pageCount()
```

When the configured page limit is reached,

the crawl terminates normally.

---

## Empty Frontier

When

```cpp
frontier.empty()
```

returns true,

there are no remaining pending crawl tasks and the crawler terminates normally.

---

# Section 4 — Complexity Analysis

Let:

- **N** = number of stored URLs
- **H** = HTML document size
- **L** = number of extracted hyperlinks
- **m** = URL length

| Operation | Best Case | Average Case | Worst Case |
|-----------|-----------|--------------|------------|
| URL normalization | O(m) | O(m) | O(m) |
| Frontier push | O(1) | O(1) | O(N) |
| Frontier pop | O(1) | O(1) | O(N) |
| Frontier contains | O(1) | O(1) | O(N) |
| SeenStore lookup | O(1) | O(1) | O(N) |
| SeenStore insert | O(1) | O(1) | O(N) |
| Link extraction | O(H) | O(H) | O(H) |
| Page storage | O(H) | O(H) | O(H) |
| Fetch operation | Network-dependent | Network-dependent | Network-dependent |

## Explanation

### URL Normalization

Normalization processes each character of the URL once.

Therefore,

```
O(m)
```

---

### Frontier Operations

Queue operations require constant time.

HashMap duplicate checking is constant on average.

Worst-case complexity occurs because of collision chains.

---

### SeenStore Operations

SeenStore uses HashMap lookup and insertion.

Average complexity:

```
O(1)
```

Worst case:

```
O(N)
```

---

### Link Extraction

The HTML document is scanned once.

Therefore,

```
O(H)
```

---

### Page Storage

Writing HTML to disk dominates the operation.

Therefore,

```
O(H)
```

---

### Overall Performance

Crawler execution time is mainly influenced by:

- Network latency
- HTML document size
- Number of discovered hyperlinks
- Disk I/O
- Database operations

---

# Section 5 — Future Compatibility

The crawler communicates only through component interfaces.

Internal implementations can therefore evolve without modifying crawler logic.

Possible future improvements include:

## Multi-threaded Crawling

Multiple worker threads can process Frontier entries concurrently.

---

## Distributed Crawling

The Frontier and PageStorage can be replaced by distributed implementations shared across multiple crawler instances.

---

## Advanced Storage

PageStorage may later support:

- Cloud object storage
- Distributed file systems
- Database-only storage

without requiring any modifications to the Crawler.

---

## Improved Duplicate Detection

SeenStore may internally use:

- Bloom Filters
- Distributed HashMaps
- Persistent caches

while preserving the same public API.

---

## Dynamic Website Support

Fetcher can later support:

- JavaScript rendering
- Headless browsers
- Chrome DevTools Protocol (CDP)

without requiring any modifications to the crawling algorithm.

---

# Advantages

- Clear separation of responsibilities.
- Persistent crawler recovery.
- Breadth First Search traversal.
- Modular component architecture.
- Duplicate URL prevention.
- Independent component testing.
- Easy future extensibility.
- Database interactions completely encapsulated inside PageStorage.

---

# Limitations

- Single-threaded implementation.
- No retry scheduling for failed downloads.
- No robots.txt support.
- No crawl-delay mechanism.
- JavaScript-heavy websites require future Fetcher enhancements.
- Scalability is limited by local storage and in-memory scheduling.