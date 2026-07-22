# Module: Persistent Storage Optimization in PageStorage

**Date:** July 22, 2026

**Duration:** 2 Hours

## Goal

Improve the efficiency of the crawler's persistent storage by replacing repeated scans of the storage index with in-memory hash-based indexes while preserving the crawler's ability to resume from previously stored pages.

---

## Problem

The initial implementation of `PageStorage` relied on reading `index.txt` whenever operations such as checking whether a page existed or retrieving a stored page were performed. As the number of crawled pages increased, these repeated file scans became increasingly inefficient.

Another challenge was ensuring that after replacing file-based lookups with in-memory data structures, the crawler could still recover its state correctly after restarting without introducing inconsistencies between memory and persistent storage.

---

## What I Tried

### Studied Storage Access Pattern

Reviewed how `PageStorage` interacted with `index.txt` and identified that the file was repeatedly scanned to perform simple lookup operations.

Mapped the existing workflow to determine which information needed to remain available in memory throughout crawler execution.

---

### Designed In-Memory Metadata Indexes

Introduced two hash-based indexes inside `PageStorage`.

- `pageIndex` to associate normalized URLs with their corresponding page metadata.
- `idToURL` to map stored page identifiers back to their original URLs.

These indexes were designed to serve as the primary lookup structures after initialization.

---

### Implemented Storage Initialization

Updated `initializeStorage()` to rebuild the complete storage metadata during application startup.

For every entry present in `index.txt`, the initialization process now:

- Reads the stored page ID.
- Reads the normalized URL.
- Reads the crawl depth.
- Inserts the information into the corresponding hash maps.
- Reconstructs the total number of stored pages.

This allows the crawler to recover previously stored metadata without rescanning the index file during normal execution.

---

### Refactored Lookup Operations

Modified storage-related functions to use the in-memory indexes instead of repeatedly traversing `index.txt`.

The following operations were updated:

- Page existence check.
- HTML page retrieval.
- URL retrieval using page ID.

This reduced unnecessary disk access while keeping the persistent storage format unchanged.

---

## Outcome

Successfully redesigned the storage layer to use in-memory indexes for metadata retrieval while continuing to use `index.txt` as the persistent source of storage information.

The crawler can now rebuild its storage metadata during startup and perform subsequent lookup operations in constant time without repeatedly scanning the storage index. This optimization improved the efficiency of the storage subsystem and prepared the crawler for further work on resume functionality.