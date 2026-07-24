# Frontier Design Proposal — Version 2

# Overview

The **Frontier** is the scheduling component of the web crawler responsible for maintaining URLs that have been discovered but not yet crawled.

Each scheduled crawl task is represented by a `URLDepth` object containing:

- The normalized URL.
- The crawl depth of that URL.

Keeping the URL and its depth together ensures that scheduling information always remains consistent during Breadth First Search (BFS) crawling.

The Frontier follows a **FIFO (First-In-First-Out)** scheduling strategy. The first discovered URL is always processed first, naturally implementing BFS traversal.

Internally, the Frontier maintains two synchronized data structures:

1. **Queue<URLDepth>**
   - Maintains the crawling order.
   - Stores complete URL-depth pairs.

2. **HashMap<string, bool>**
   - Tracks URLs currently waiting inside the Frontier.
   - Prevents duplicate pending URLs.

The Frontier is responsible only for scheduling pending crawl tasks.

It does **not** perform:

- URL normalization.
- HTML downloading.
- HTML parsing.
- Page storage.
- Persistence.
- Completed URL tracking.

These responsibilities belong to other crawler components:

- URLNormalizer
- Fetcher
- LinkExtractor
- PageStorage
- SeenStore

During crawler initialization, **PageStorage** restores the crawler state by loading pending URLs from persistent storage and inserting them into the Frontier.

---

# Section 1 — Public API

## URLDepth Class

```cpp
class URLDepth
{
public:

    std::string url;

    int depth;


    URLDepth(
        const std::string& url,
        int depth);


    bool operator==(
        const URLDepth& other) const;
};
```

## Design Justification

`URLDepth` represents one complete crawl task.

A crawler cannot process a URL without knowing its crawl depth because depth determines whether newly discovered links should continue to be scheduled.

Therefore, URL and depth are stored together as one object.

The equality operator compares only URLs because two entries with the same URL represent the same crawl target regardless of depth.

---

## Frontier API

```cpp
class Frontier
{
private:

    Queue<URLDepth> pendingQueue;

    HashMap<std::string, bool> pendingURLs;

public:

    void push(URLDepth& item);

    URLDepth pop();

    const URLDepth& front();

    bool contains(std::string& url);

    bool empty() const;

    int size() const;
};
```

---

## API Explanation

### push()

```cpp
void push(URLDepth& item);
```

Schedules a new crawl task.

Before insertion, the Frontier checks whether the URL already exists inside `pendingURLs`.

If the URL is already waiting inside the Frontier:

- The insertion is ignored.
- The queue remains unchanged.

Otherwise:

1. The complete `URLDepth` object is inserted into the FIFO queue.
2. The URL is inserted into the HashMap.

Both structures remain synchronized.

---

### pop()

```cpp
URLDepth pop();
```

Removes and returns the next scheduled crawl task.

After removing the front element from the queue, the corresponding URL is also removed from `pendingURLs`.

This guarantees that both internal structures always represent the same set of pending URLs.

---

### front()

```cpp
const URLDepth& front();
```

Returns the next scheduled crawl task without removing it.

Useful for inspection and debugging.

---

### contains()

```cpp
bool contains(std::string& url);
```

Checks whether a URL is currently waiting inside the Frontier.

This operation prevents duplicate pending crawl tasks.

---

### empty()

```cpp
bool empty() const;
```

Returns whether the Frontier currently contains any pending crawl tasks.

---

### size()

```cpp
int size() const;
```

Returns the total number of pending crawl tasks.

---

# Section 2 — Internal Representation

The Frontier maintains two synchronized in-memory structures.

```
                Frontier

        +-----------------------+
        |                       |
        | Queue<URLDepth>       |
        |                       |
        +-----------+-----------+
                    |
                    |
                    v

Front

+-----------------------------+
| Node<URLDepth>              |
+-----------------------------+
| URLDepth                    |
|-----------------------------|
| url                         |
| depth                       |
|-----------------------------|
| next pointer                |
+-----------------------------+

                    |

                    v

+-----------------------------+
| Node<URLDepth>              |
+-----------------------------+
| URLDepth                    |
|-----------------------------|
| url                         |
| depth                       |
|-----------------------------|
| next pointer                |
+-----------------------------+

                    |

                    v

                  Rear
```

Each queue node stores one complete crawl task.

Memory representation:

```
+----------------------+
| URLDepth             |
+----------------------+
| std::string url      |
| int depth            |
+----------------------+
```

---

## Pending URL Tracking

The Frontier maintains a HashMap.

```
HashMap<std::string, bool>


+-----------------------------+
| URL                         |
|-----------------------------|
| https://example.com/page1   |
| true                        |
+-----------------------------+

+-----------------------------+
| URL                         |
|-----------------------------|
| https://example.com/page2   |
| true                        |
+-----------------------------+
```

The boolean value is always `true`.

It exists only because the HashMap stores key-value pairs.

The HashMap is **not** used as a visited store.

It only tracks URLs currently waiting inside the Frontier.

---

## Synchronization Invariant

The Frontier guarantees that the Queue and HashMap always contain the same set of pending URLs.

During insertion:

```
Queue.enqueue()

↓

HashMap.insert()
```

During removal:

```
Queue.dequeue()

↓

HashMap.remove()
```

This synchronization ensures:

- Constant-time duplicate checking.
- FIFO scheduling.
- No stale pending entries.

---

## Runtime Relationship

```
Crawler
   |
   v
Frontier
   |
   +----------------+
   |                |
   v                v

Queue          HashMap

Ordering       Duplicate
               Prevention
```

---

# Section 3 — Failure Handling

## Duplicate URL

If a URL already exists inside `pendingURLs`:

Example:

```
https://example.com/about
```

The insertion is ignored.

The existing scheduled crawl task remains unchanged.

---

## Empty Frontier

Calling

```cpp
pop()
```

or

```cpp
front()
```

on an empty Frontier is invalid.

The underlying Queue handles this condition.

The crawler normally prevents this by checking

```cpp
while(!frontier.empty())
```

before removing elements.

---

## Memory Allocation Failure

Insertion may require memory allocation for:

- Queue node.
- HashMap entry.

If allocation fails:

- The underlying data structure reports the failure.
- Existing Frontier contents remain unchanged.

---

## Invalid URL

The Frontier does not validate URLs.

Invalid URLs are rejected before reaching the Frontier by:

- URLNormalizer.

---

## Already Crawled URL

The Frontier does not maintain completed URLs.

Completed URLs are tracked separately by:

```
SeenStore
```

Relationship:

```
Pending URLs

      |
      v

   Frontier


Completed URLs

      |
      v

   SeenStore
```

---

## Persistence

The Frontier itself is entirely an in-memory scheduling component.

It performs no file or database operations.

During crawler startup, **PageStorage** restores crawler state by loading pending URLs from persistent storage and inserting them into the Frontier.

```
PageStorage

      |
      v

recoverCrawlerState()

      |
      v

Frontier.push()
```

This separation keeps the Frontier independent from the persistence mechanism.

---

# Section 4 — Complexity Analysis

Let:

- **n** = number of pending URLs.
- **m** = number of entries stored in the HashMap.

| Operation | Best Case | Average Case | Worst Case |
|-----------|-----------|--------------|------------|
| push() | O(1) | O(1) | O(n) |
| pop() | O(1) | O(1) | O(n) |
| front() | O(1) | O(1) | O(1) |
| contains() | O(1) | O(1) | O(n) |
| empty() | O(1) | O(1) | O(1) |
| size() | O(1) | O(1) | O(1) |

## Explanation

### push()

Queue insertion:

```
O(1)
```

HashMap insertion:

Average:

```
O(1)
```

Worst:

```
O(n)
```

due to hash collisions.

---

### pop()

Queue removal:

```
O(1)
```

HashMap removal:

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

---

## Space Complexity

```
O(n)
```

where **n** is the number of pending crawl tasks.

---

# Section 5 — Future Compatibility

The Frontier exposes only scheduling operations.

Its implementation can change without modifying crawler logic.

Possible future improvements include:

## Priority-Based Crawling

Current implementation:

```
FIFO Queue
```

Future implementation:

```
Priority Queue
```

Possible priorities:

- Page importance.
- Domain priority.
- Link popularity.
- Crawl policies.

---

## Disk-Backed Frontier

Current implementation:

```
In-memory Queue
```

Future implementation:

```
Disk-backed Queue
```

allowing extremely large crawl jobs.

---

## Distributed Frontier

Multiple crawler instances may share a centralized scheduling service.

```
Crawler 1
      |
Crawler 2
      |
Distributed Frontier
      |
Crawler 3
```

---

## Thread-Safe Frontier

Future versions may synchronize queue operations to support concurrent crawler workers.

The current interface allows these improvements without changing the crawler API.