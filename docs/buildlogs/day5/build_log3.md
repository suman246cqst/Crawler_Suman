# Build Log — Session 3

**Date:** July 23, 2026

**Duration:** Approximately 2 hours

**Goal:**
Integrate the updated `PageStorage` design with crawler components (`Frontier`, `SeenStore`, and `Crawler`) and finalize the persistent crawling workflow.

**Problem:**
After updating `PageStorage`, the remaining challenge was ensuring that the crawler correctly coordinated in-memory state and persistent state.

The earlier implementation mainly handled crawling in memory:

```text
Crawler
   |
   ├── Frontier
   |
   └── SeenStore
```

However, after introducing persistence, crawler state existed in two places:

```text
Memory State
    |
    ├── Frontier
    └── SeenStore


Persistent State
    |
    ├── Pending URLs
    └── Stored Pages
```

Without proper synchronization, problems could occur:

* A URL could exist in Frontier but not in the database.
* A URL could be stored in the database but missing from memory after restart.
* Duplicate URLs could enter the crawling pipeline.
* Restarting the crawler could lose pending work.

**What I Tried:**

* Reviewed the complete crawler execution flow.
* Updated the URL insertion workflow so persistent storage happens before adding URLs to Frontier.

The updated pending URL flow became:

```text
New URL Discovered
        |
        ▼
Normalize URL
        |
        ▼
Check SeenStore
        |
        ▼
Check Frontier
        |
        ▼
Database Insert Pending URL
        |
        ▼
Frontier Push
```

Updated crawler logic:

```cpp id="5umr7c"
if(storage.addPendingURL(
        normalizedURL,
        nextDepth))
{
    URLDepth nextURL(
        normalizedURL,
        nextDepth
    );

    frontier.push(nextURL);
}
```

---

### Frontier Integration

Reviewed the role of Frontier after persistence was added.

Frontier responsibilities:

* Maintain crawl order.
* Hold URLs waiting for processing.
* Prevent duplicate pending URLs.

The implementation continued using:

```cpp id="3p98pn"
contains(url)

push(url)

pop()

front()

size()
```

When a URL is removed:

```text
Frontier Pop
      |
      ▼
Fetch URL
      |
      ▼
Process Result
```

---

### SeenStore Integration

Reviewed the correct point to mark URLs as completed.

Earlier assumption:

```text
URL Added To SeenStore
        |
        ▼
Fetch Started
```

This could mark failed requests as completed.

The updated approach:

```text
Fetch URL
    |
    ▼
Successful Fetch
    |
    ▼
Store Page
    |
    ▼
Insert Into SeenStore
```

Implementation flow:

```cpp id="u7l6ap"
if(storage.storePage(
        current.url,
        result.html,
        current.depth))
{
    seen.insert(current.url);
}
```

---

### Duplicate Handling Strategy

The crawler now performs duplicate checks at multiple stages:

Before crawling:

```text
SeenStore
    |
    ▼
Already completed?
```

Before adding new work:

```text
Frontier
    |
    ▼
Already pending?
```

Before persistence:

```text
Database
    |
    ▼
Already stored?
```

This provides protection against duplicate crawling during both normal execution and restart scenarios.

**Outcome:**
The crawler workflow was finalized with proper synchronization between memory and persistent storage.

The final execution model became:

```text
Seed URL
    |
    ▼
URL Normalization
    |
    ▼
Persistent Pending URL Storage
    |
    ▼
Frontier
    |
    ▼
Fetcher
    |
    ▼
Link Extraction
    |
    ▼
URL Normalization
    |
    ▼
Duplicate Checks
    |
    ▼
Store New Pending URLs
    |
    ▼
Store HTML Page
    |
    ▼
Mark Completed In SeenStore
```

This session completed the integration of the crawler's core components and prepared the final implementation for validation and testing.
