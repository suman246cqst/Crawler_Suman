## Session 1

**Module:** PageStorage Implementation
**Date:** 20-07-26  
**Duration:** 2 Hours 

### Goal

Complete the implementation of the `PageStorage` component so that fetched webpages can be permanently stored on disk and retrieved later using their corresponding URLs.

---

### Problem

The crawler required a persistent storage system capable of:

- Saving fetched HTML pages.
- Retrieving stored pages using URLs.
- Preventing duplicate storage.
- Keeping track of the total number of stored pages even after restarting the crawler.

Another important design problem was deciding whether the **URL itself should be used as the filename** for storing HTML pages.

---

### What I Tried

Initially considered storing webpages using their URLs as filenames.

Example:

```text
https://studyadda.onrender.com/about

↓

https://studyadda.onrender.com/about.html
```

This approach was rejected because:

- URLs can become very long.
- URLs contain characters such as `/`, `:`, `?`, and `#` that are invalid in filenames.
- Different operating systems impose different filename restrictions.
- Multiple URL representations may refer to the same webpage after normalization.

Instead, decided to assign every stored page a unique numeric identifier.

Example:

```text
Page ID : 0

↓

page_0.html
```

The mapping between URLs and stored HTML files is maintained separately inside an `index.txt` file.

---

### Design Decisions

#### Decision 1

Store every webpage using a sequential numeric filename.

Example:

```text
page_0.html
page_1.html
page_2.html
```

instead of URL-based filenames.

---

#### Decision 2

Maintain an `index.txt` file containing:

- URL
- Stored HTML filename
- Crawl depth

This provides an efficient mapping from URLs to stored pages.

---

#### Decision 3

During `PageStorage` initialization, read the existing `index.txt` file and calculate the current page count automatically.

Workflow:

```text
Program Starts
      ↓
Read index.txt
      ↓
Count Stored URLs
      ↓
Initialize pageCount
```

This eliminates the need for maintaining a separate `nextPageId`.

---

#### Decision 4

`pageCount()` should always represent the actual number of stored pages rather than relying on a separately stored counter.

---

#### Decision 5

If a requested URL does not exist,

```cpp
getPage(url)
```

returns an empty string instead of throwing an exception.

---

#### Decision 6

`hasPage(url)` performs lookup through the URL index rather than scanning stored HTML files.

---

### Problems Encountered

#### Problem 1

How should the crawler continue numbering pages after restarting?

**Solution**

Read the contents of `index.txt` during construction and initialize `pageCount` using the number of stored entries.

---

#### Problem 2

Initially considered maintaining a separate `nextPageId`.

**Solution**

Rejected this approach because the next page number can always be derived from the number of entries already present in `index.txt`.

---

#### Problem 3

Whether URLs should be used directly as filenames.

**Solution**

Rejected URL-based filenames because they:

- Can exceed filesystem limits.
- Contain invalid filename characters.
- Depend on URL formatting rather than unique storage identifiers.

Numeric filenames provide a much more reliable storage mechanism.

---

### Outcome

Successfully completed the implementation of the `PageStorage` component.

Implemented and tested:

- `storePage()`
- `getPage()`
- `hasPage()`
- `pageCount()`

The component now:

- Stores HTML pages permanently on disk.
- Maintains URL-to-file mappings using `index.txt`.
- Restores page count automatically after application restart.
- Supports efficient page lookup without rescanning stored HTML files.
- Provides the persistent storage layer required by the crawler before beginning work on the `LinkExtractor` component.