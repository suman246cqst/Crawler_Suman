# Module: Architecture Review and Design Validation

**Date:** July 2, 2026

**Duration:** 2 Hours

## Goal

Perform a comprehensive review of the crawler's integrated architecture to validate the design decisions made during recent integration work and identify logical issues affecting the crawler's resume mechanism and storage subsystem.

---

## Problem

After integrating the crawler modules and introducing persistent storage with `PageStorage` and `SeenStore`, the crawler executed successfully under normal conditions. However, before extending the crawler further, it was necessary to verify whether the interaction between all components was logically consistent.

Particular attention was given to:

- Whether `PageStorage` maintained a consistent in-memory representation of persistent data.
- Whether `SeenStore` correctly synchronized with stored pages.
- Whether the crawler workflow handled duplicate URLs appropriately.
- Whether the overall architecture could support reliable resume functionality.

---

## What I Tried

### Reviewed PageStorage Design

Performed a detailed walkthrough of the complete `PageStorage` implementation.

Verified the logic of:

- Storage initialization.
- Page insertion.
- Metadata indexing.
- Page retrieval.
- Page counting.
- URL lookup using stored page identifiers.

Confirmed that the in-memory hash maps remain synchronized with the persistent `index.txt` file during normal execution.

---

### Reviewed SeenStore Design

Analyzed the complete lifecycle of the visited URL manager.

Verified that:

- Previously stored URLs are reconstructed correctly during startup.
- Newly downloaded pages are immediately inserted into the visited set.
- Duplicate page downloads are prevented during normal crawling.

Also confirmed that using `index.txt` as the single source of truth avoids maintaining multiple persistent metadata files.

---

### Validated Component Responsibilities

Reviewed the responsibilities of the major crawler components to ensure a clear separation of concerns.

Confirmed that:

- `Crawler` coordinates the overall crawling workflow.
- `Frontier` manages pending crawl tasks.
- `Fetcher` downloads HTML pages.
- `LinkExtractor` extracts hyperlinks.
- `URLNormalizer` produces canonical URLs.
- `SeenStore` tracks previously fetched pages.
- `PageStorage` manages persistent page storage.

This review helped verify that each module performs a well-defined responsibility without unnecessary coupling.

---

### Identified Architectural Limitation

During the design review, confirmed that the primary limitation in the current crawler architecture is not the implementation of `PageStorage` or `SeenStore`, but the absence of persistent Frontier management.

Although visited pages can be reconstructed successfully after restarting the crawler, the pending crawl state cannot, preventing consistent traversal continuation across multiple executions.

---

## Outcome

Completed a comprehensive architectural review of the integrated crawler.

Validated the logical correctness of the storage subsystem, visited URL management, and overall module interaction. The review also confirmed that the current resume limitation originates from the lack of Frontier persistence rather than errors in the storage components themselves.

This analysis provides a clear direction for the next phase of development, which will focus on designing a persistent Frontier capable of restoring unfinished crawl operations after application restart.