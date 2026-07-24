# Daily Design Journal · day_23_journal.md

**Date:** July 22, 2026

---

# 1. Specific Bug / Design Problem

During integration testing and code review, I discovered a flaw in the crawler's resume mechanism.

The crawler stores every successfully fetched URL inside **SeenStore**. During a normal crawl this prevents duplicate downloads and works correctly. However, after restarting the crawler, **SeenStore** is rebuilt from `index.txt`, meaning every previously fetched page is immediately considered visited.

When crawling begins again from the seed URL:

- The seed page is fetched.
- Its hyperlinks are extracted.
- Child URLs that already exist in SeenStore are skipped.
- Those child pages are never fetched or processed again.
- Their outgoing hyperlinks are never discovered.

As a result, the crawler cannot continue exploring deeper levels after restarting.

Example:

```
A
├── B
│   ├── E
│   └── F
├── C
│   └── G
└── D
    └── H
```

First execution:

```
A → B → C → D
```

After restarting:

```
Fetch A
      │
      ▼
Extract B, C, D
      │
      ▼
SeenStore contains all three
      │
      ▼
Skip B
Skip C
Skip D
```

The crawler never reaches:

```
E
F
G
H
```

Although the HTML of B, C and D already exists inside PageStorage, it is never reused.

---

# 2. Failed Attempt

Initially, I assumed that rebuilding SeenStore from `index.txt` would automatically provide resume functionality.

The intended workflow was:

```
Load SeenStore
        │
        ▼
Skip Already Visited Pages
        │
        ▼
Continue Crawling
```

After tracing the crawler step by step, I found that this assumption was incorrect.

A page's outgoing links are only discovered after that page is processed.

Since previously visited pages are skipped immediately after restart, their stored HTML is never parsed again.

Consequently:

- previously downloaded pages are not expanded,
- new child pages are never discovered,
- crawling terminates prematurely.

This showed that **"visited"** and **"expanded"** represent two different states and should not be treated as the same.

---

# 3. Memory / Execution Diagram

### Current Design

```
Seed URL
    │
    ▼
Fetch HTML
    │
    ▼
Store Page
    │
    ▼
Insert into SeenStore
```

Restart

```
Load SeenStore
      │
      ▼
Fetch Seed
      │
      ▼
Extract Links
      │
      ▼
B already seen
C already seen
D already seen
      │
      ▼
Do Not Push Into Frontier
```

Traversal stops.

---

### Desired Resume Behaviour

```
Load PageStorage
        │
        ▼
Read Stored HTML
        │
        ▼
Extract Links Again
        │
        ▼
Generate Frontier
        │
        ▼
Fetch Only New Pages
```

or maintain separate crawler states:

```
Downloaded
      │
      ├── Expanded
      └── Not Expanded
```

Only pages that have already been expanded should be skipped.

---

# 4. Code Reference

### Crawler.cpp

Skipping already visited pages:

```cpp
if(!isSeed && seen.contains(temp.url)){
    continue;
}
```

Marking pages as visited immediately after fetching:

```cpp
seen.insert(temp.url);
storage.storePage(result.url, result.html, temp.depth);
```

Preventing previously visited child URLs from entering the Frontier:

```cpp
if(!seen.contains(normalizedURL)){
    frontier.push(URLDepth(normalizedURL, temp.depth + 1));
}
```

---

### SeenStore.cpp

Rebuilding SeenStore from persistent storage:

```cpp
initializeStorage();
```

Loading every stored URL into memory:

```cpp
insert(url);
```

---

### PageStorage.cpp

Persisting downloaded HTML:

```cpp
storePage(...)
```

Although PageStorage retains every downloaded page, the crawler currently does not reuse this HTML when resuming a crawl.

---

# 5. Learning Reflection

Today's work reinforced that preventing duplicate downloads is not the same as supporting resume functionality.

Initially, I believed that storing previously visited URLs would be sufficient to continue crawling after a restart. A detailed analysis of the crawler's execution revealed that this approach causes the traversal to stop prematurely because already downloaded pages are never expanded again.

The key insight is that a crawler must distinguish between pages that have been downloaded and pages that have already been fully expanded. Alternatively, it should be capable of reading previously stored HTML from PageStorage and regenerating the frontier instead of relying solely on SeenStore.

This design review improved my understanding of crawler state management and highlighted the importance of separating persistence, duplicate detection, and traversal logic when building a resumable web crawler.