# Crawler Design Proposal

---

# Overview

The **Crawler** component acts as the central controller of the web crawler. It coordinates all other crawler components and implements the overall crawling algorithm. Its primary responsibility is to discover web pages starting from a user-provided seed URL, download their HTML content, extract hyperlinks, normalize discovered URLs, and continue exploring newly discovered pages while respecting the specified crawl limits.

The crawler follows a **Breadth-First Search (BFS)** traversal strategy. It maintains a queue of URLs to visit, ensures that each URL is processed at most once using the Seen URL Store, stores successfully downloaded pages using the PageStorage component, and continuously expands the crawl frontier by extracting hyperlinks from downloaded pages.

The crawler itself does **not** perform networking, HTML parsing, duplicate detection, or permanent storage. Instead, these responsibilities are delegated to specialized components:

- **Fetcher** downloads the HTML of a webpage.
- **LinkExtractor** scans the HTML and extracts hyperlinks from `<a>` elements.
- **URLNormalizer** converts extracted hyperlinks into canonical absolute URLs suitable for duplicate detection and crawling.
- **SeenStore** prevents revisiting previously processed URLs.
- **PageStorage** stores successfully downloaded pages.

The crawling process terminates when either:

- the Frontier becomes empty, or
- the maximum number of pages specified by the user has been successfully stored.

---

# Section 1 – Public API

The Crawler exposes a minimal interface responsible only for coordinating the crawling process.

## Public Interface

```cpp
class Crawler
{
public:
    Crawler(int maxDepth, int maxPages);

    void crawl(const string& seedURL);

private:
    Frontier frontier;
    SeenStore seen;
    PageStorage storage;

    Fetcher fetcher;
    LinkExtractor extractor;
    URLNormalizer normalizer;

    int maxDepth;
    int maxPages;
};
```

---

## Constructor

```cpp
Crawler(int maxDepth, int maxPages)
```

Initializes the crawler with:

- Maximum crawl depth.
- Maximum number of pages to crawl.
- Internal crawler components.

---

## crawl(const string& seedURL)

Starts crawling from the specified seed URL.

The seed URL is assumed to already be normalized and is inserted into the Frontier with depth **0**.

The crawler then performs a Breadth-First Search until one of the termination conditions is reached.

---

# Section 2 – Internal Representation

## Internal Components

```text
Crawler
│
├── Frontier
├── SeenStore
├── PageStorage
├── Fetcher
├── LinkExtractor
├── URLNormalizer
├── maxDepth
└── maxPages
```

---

## Responsibilities

### Frontier

Stores URLs waiting to be crawled.

Each entry consists of:

```cpp
struct URLDepth
{
    string url;
    int depth;
};
```

The Frontier guarantees FIFO ordering, ensuring Breadth-First Search traversal.

---

### SeenStore

Stores every URL that has already been processed.

Before processing any URL, the crawler checks whether it already exists inside the SeenStore.

---

### PageStorage

Stores every successfully downloaded page together with:

- URL
- HTML content
- Crawl depth

---

### Fetcher

The Fetcher is responsible for downloading the HTML content of a web page.

The current implementation uses **libcurl** to perform HTTP requests and retrieve the HTML returned directly by the web server.

Workflow:

```text
URL
 ↓
HTTP Request (libcurl)
 ↓
Receive HTML Response
 ↓
Return HTML
```

The Fetcher is designed for **static** and **server-side rendered (SSR)** websites, where the complete HTML document is included in the server response.

The Fetcher does **not**:

- execute JavaScript,
- render client-side applications,
- wait for DOM updates,
- interact with browser APIs.

Therefore, websites whose content is generated entirely through JavaScript (such as many React, Angular, or Vue applications) may return only their initial HTML template rather than the fully rendered page.

The crawler processes the HTML exactly as returned by the server and forwards it to the LinkExtractor.

---

### LinkExtractor

The LinkExtractor is responsible for extracting hyperlinks from downloaded HTML.

Instead of using regular expressions or string pattern matching, the extractor performs a **single left-to-right scan** over the HTML document.

Whenever an opening `<a>` element is encountered, the extractor parses its attributes and extracts the value of the `href` attribute if present.

The extractor:

- scans the HTML only once,
- supports attributes in any order,
- supports both single and double quoted attribute values,
- supports unquoted attribute values,
- ignores malformed attributes that cannot be parsed,
- returns the extracted hyperlinks exactly as they appear in the HTML.

The LinkExtractor performs **only HTML parsing**.

It does **not**:

- normalize URLs,
- resolve relative URLs,
- remove duplicate URLs,
- determine whether a URL should be crawled.

---

### URLNormalizer

The URLNormalizer converts extracted hyperlinks into canonical URLs suitable for crawling.

Its responsibilities include:

- converting relative URLs into absolute URLs,
- resolving `.` and `..` path components,
- resolving protocol-relative URLs (`//example.com`),
- removing URL fragments (`#section`),
- rejecting unsupported schemes such as:
  - `javascript:`
  - `mailto:`
  - `tel:`
  - `data:`
- producing a normalized URL suitable for duplicate detection.

If a URL cannot be crawled, the normalizer returns an empty result.

---

# Crawling Workflow

```text
Insert Seed URL (Depth = 0)
            │
            ▼
──────────────────────────────────────────────
while Frontier is not empty
and PageStorage.pageCount() < maxPages
──────────────────────────────────────────────
            │
            ▼
Remove URL from Frontier
            │
            ▼
Already Seen ?
      │
 ┌────┴────┐
 │         │
Yes       No
 │         │
 │         ▼
 │    Insert into SeenStore
 │         │
 │         ▼
 │    Fetch HTML
 │         │
 │         ▼
 │  Fetch Successful ?
 │
 ├────────No────────► Continue
 │
 ▼
Store Page
 │
 ▼
Extract Raw Links
 │
 ▼
Normalize Every URL
 │
 ▼
Valid URL ?
 │
 ├── No → Ignore
 │
 └── Yes
        │
        ▼
Already Seen ?
 │
 ├── Yes → Ignore
 │
 └── No
       │
       ▼
Depth + 1 <= MaxDepth ?
       │
 ┌─────┴─────┐
 │           │
No          Yes
 │           │
 │           ▼
 │     Insert into Frontier
 │
 ▼
Continue
```

---

## Crawling Algorithm

1. Insert the seed URL into the Frontier with depth **0**.
2. Repeat while:
   - Frontier is not empty.
   - `storage.pageCount() < maxPages`.
3. Remove one URL-Depth pair from the Frontier.
4. Check whether the URL already exists in the SeenStore.
5. If already processed, continue.
6. Otherwise, insert it into the SeenStore.
7. Download the page using the Fetcher.
8. If fetching fails or returns an empty document, continue.
9. Store the downloaded page.
10. Pass the HTML to the LinkExtractor.
11. Receive all extracted hyperlinks.
12. For every extracted hyperlink:
    - Normalize the URL.
    - If normalization fails, ignore it.
    - If already present in the SeenStore, ignore it.
    - Otherwise, if `depth + 1 <= maxDepth`, insert it into the Frontier.
13. Continue until the crawl terminates.

---

## Memory Representation

```text
+------------------------------------------------+
|                   Crawler                      |
+------------------------------------------------+
| Frontier frontier                              |
| SeenStore seen                                 |
| PageStorage storage                            |
| Fetcher fetcher                                |
| LinkExtractor extractor                        |
| URLNormalizer normalizer                       |
| int maxDepth                                   |
| int maxPages                                   |
+------------------------------------------------+
```

---

# Section 3 – Failure Handling

## Duplicate URLs

Duplicate detection is delegated entirely to the SeenStore.

If a URL already exists, it is skipped immediately.

---

## Fetch Failure

If downloading a page fails, the crawler ignores the page and continues processing remaining URLs.

A single failed request never terminates the crawl.

---

## Empty HTML

If the Fetcher returns an empty document, the page is discarded and not stored.

---

## Link Extraction Failure

Malformed HTML may produce zero extracted hyperlinks.

The page is still stored successfully.

---

## Invalid Hyperlinks

Malformed anchor tags or malformed `href` attributes are ignored by the LinkExtractor.

Extraction continues for the remaining HTML.

---

## Unsupported URL Schemes

URLs using unsupported schemes such as

- `javascript:`
- `mailto:`
- `tel:`
- `data:`

are rejected during normalization and are never inserted into the Frontier.

---

## Maximum Depth

URLs whose depth exceeds the configured maximum depth are ignored.

---

## Maximum Pages

The crawler terminates immediately when

```cpp
storage.pageCount() >= maxPages
```

Only successfully stored pages contribute toward this limit.

---

## Empty Frontier

The crawler terminates normally when no URLs remain to be explored.

---

# Section 4 – Complexity Analysis

Assume:

- **N** = Number of successfully crawled pages.
- **L** = Average number of hyperlinks extracted per page.

| Operation | Complexity |
|-----------|------------|
| Frontier enqueue | O(1) |
| Frontier dequeue | O(1) |
| SeenStore lookup | Depends on implementation |
| Fetch page | Network dependent |
| Store page | Depends on PageStorage |
| Extract hyperlinks | O(size of HTML) |
| Normalize URL | O(length of URL) |
| Process extracted URLs | O(L) |

Overall crawler performance is primarily dominated by:

- network latency,
- number of crawled pages,
- number of hyperlinks discovered,
- efficiency of the SeenStore.

---

# Section 5 – Future Compatibility

The crawler communicates with supporting components exclusively through their public interfaces.

Future improvements can therefore be incorporated without modifying the crawling algorithm itself.

Possible enhancements include:

- replacing file-based PageStorage with MongoDB,
- replacing SeenStore with a database-backed implementation,
- supporting concurrent or multi-threaded crawling,
- robots.txt compliance,
- crawl politeness and request delays,
- distributed crawling,
- more advanced RFC-compliant URL normalization,
- Extending the Fetcher with a browser-based rendering engine (such as the Chrome DevTools Protocol) to support JavaScript-rendered websites.
- extracting additional resources such as:
  - `<img src>`
  - `<script src>`
  - `<link href>`
  - `<iframe src>`

without modifying the crawler's BFS algorithm.

---

# Advantages

- Clear separation of responsibilities.
- Breadth-First Search traversal guarantees level-wise crawling.
- Lightweight HTTP-based Fetcher implemented using libcurl.
- Suitable for crawling static and server-side rendered (SSR) websites.
- Link extraction is independent of networking.
- URL normalization is centralized.
- Duplicate pages are efficiently avoided.
- Crawl depth and page limits prevent unbounded crawling.
- Easily extensible for future enhancements.

---

# Limitations

- Single-threaded implementation.
- Network latency dominates execution time.
- No retry mechanism for failed downloads.
- No distributed crawling.
- Does not implement robots.txt compliance.
- Does not implement crawl politeness.
- The Fetcher retrieves only the HTML returned directly by the web server.
- Client-side JavaScript is not executed.
- Fully dynamic websites may not expose all hyperlinks in the retrieved HTML.