# Daily Design Journal · day_5_journal.md

**Date:** July 23, 2026

---

# 1. Specific Bug / Design Problem

During the redesign of the persistent crawler architecture, I discovered that the original storage workflow could not properly support long-term persistence or crawler recovery.

The original design stored metadata inside `index.txt` in the following format:

```text
URL | FileID | FileName | Depth
```

Example:

```text
https://a.com | 1 | page1.txt | 0
https://b.com | 2 | page2.txt | 1
https://c.com | 3 | page3.txt | 2
```

Although this allowed URL-to-file mapping, several problems appeared as the crawler architecture evolved:

### Problem 1: No Efficient Lookup

Checking whether a URL already existed required scanning the entire file.

```
index.txt
    │
    ▼
Read line 1
Read line 2
Read line 3
...
Read line N
```

Duplicate detection became increasingly expensive as the number of crawled pages grew.

---

### Problem 2: Pending URLs Could Not Be Persisted

The crawler only stored successfully downloaded pages.

If the crawler stopped unexpectedly:

```
Frontier
│
├── URL_A
├── URL_B
└── URL_C
```

all pending URLs stored in memory were lost.

After restarting:

```
Frontier = Empty
```

meaning the crawler could not resume exactly where it stopped.

---

### Problem 3: Recovery Logic Was Distributed

Persistence responsibilities were scattered:

- PageStorage stored HTML files.
- SeenStore rebuilt itself independently.
- Frontier had its own duplicate detection.
- Crawler manually restored state.

There was no single component responsible for crawler persistence.

---

### Problem 4: PageStorage Had External Database Ownership

During the first redesign attempt, `Database` was passed into `PageStorage` as a constructor dependency:

```cpp
PageStorage(Database& database);
```

This introduced unnecessary coupling and complicated ownership management.

The crawler became responsible for creating and maintaining the database object even though persistence logically belonged to PageStorage.

This violated encapsulation.

---

Because of these issues, the original crawler workflow could not provide reliable persistent crawling or clean architecture.

---

# 2. Failed Attempt

My first redesign attempted to introduce SQLite while keeping the previous architecture mostly unchanged.

The idea was:

```text
Crawler
│
├── Database
│
└── PageStorage(Database&)
```

The expectation was that PageStorage would simply use the externally supplied database object.

However, several issues appeared:

### Ownership Problem

PageStorage could not function independently because it always required an external database reference.

### Initialization Problem

Crawler became responsible for:

```cpp
database.open();
database.createURLTable();
PageStorage(database);
```

which tightly coupled crawler initialization with storage initialization.

### Persistence Responsibility Problem

Persistence was still spread across multiple components rather than centralized.

### Recovery Problem

Crawler recovery logic was still partially implemented inside Crawler instead of PageStorage.

As the design evolved, it became clear that:

- PageStorage should own Database.
- PageStorage should manage crawler persistence.
- PageStorage should recover crawler state.
- Database should remain an implementation detail.

This led to the final architecture.

---

# 3. Memory / Execution Diagram

## Original Design

```text
Crawler
│
├── Frontier
├── SeenStore
└── PageStorage
        │
        ▼
     index.txt
        │
        ▼
URL | fileId | filename | depth
```

Recovery:

```text
Crawler
│
├── Read index.txt
├── Rebuild SeenStore
└── Rebuild Frontier manually
```

Persistence logic was distributed.

---

## Intermediate Attempt

```text
Crawler
│
├── Database
│
└── PageStorage(Database&)
```

Database ownership existed outside PageStorage.

---

## Final Design

```text
Crawler
│
├── Frontier
├── SeenStore
└── PageStorage
        │
        ▼
     Database
        │
        ▼
     SQLite
```

PageStorage became responsible for:

```text
Store pending URLs
Store crawled URLs
Store metadata
Store file mapping
Recover frontier
Recover seen store
Manage persistence
```

Crawler only coordinates crawling.

---

# 4. Code Reference

### Commit: `134a990`

**File:** `include/Database.h`

Implemented the SQLite database wrapper responsible for managing database connections and crawler metadata operations.

---

### Commit: `15adeb4`

**File:** `src/Database.cpp`

Implemented:

- SQLite connection management
- URL table schema
- URL insertion
- URL lookup
- URL recovery operations
- Persistent metadata storage

---

### Commit: `2d85c52`

**File:** `include/PageStorage.h`

Integrated Database directly into PageStorage and removed external database ownership.

---

### Commit: `5ce2f8b`

**File:** `src/PageStorage.cpp`

Implemented:

- Database ownership inside PageStorage
- Persistent pending URL storage
- Updated storePage workflow
- Crawler recovery through PageStorage
- Centralized persistence management

---

### Commit: `8040363`

**Files:**

- `include/Frontier.h`
- `src/Frontier.cpp`

Improved duplicate URL detection using a HashMap-backed containment check.

---

### Commit: `e2d5fb0`

**Files:**

- `include/SeenStore.h`
- `src/SeenStore.cpp`

Simplified SeenStore because crawler persistence was moved into PageStorage.

---

### Commit: `5188d2b`

**Files:**

- `include/Crawler.h`
- `src/Crawler.cpp`

Implemented the redesigned crawler workflow and integrated persistent crawler recovery.

---

# 5. Learning Reflection

Today fundamentally changed my understanding of crawler persistence.

Initially I believed persistence simply meant saving downloaded pages and rebuilding SeenStore from `index.txt`. After redesigning the entire architecture, I realized that persistence is actually a system-level responsibility rather than a storage feature.

I also learned that introducing a database does not automatically produce a good design. My first attempt exposed the Database object directly to Crawler and passed it into PageStorage. Although technically functional, it created unnecessary coupling and violated ownership boundaries.

The final design taught me an important architectural principle:

> A component should own the resources that represent its responsibility.

Since PageStorage is responsible for persistence, it should also own the Database.

I further understood that crawler recovery, duplicate detection, pending URL persistence, and metadata management are all parts of the same persistence subsystem. Separating these responsibilities across multiple components made recovery difficult and increased coupling.

The redesign improved encapsulation, reduced dependencies between components, and produced a cleaner persistent crawler architecture that more accurately reflects each component's responsibility.