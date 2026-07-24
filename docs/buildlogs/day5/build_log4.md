# Build Log — Session 4

**Date:** July 23, 2026

**Duration:** 2 hours

**Goal:**
Review the complete crawler implementation, validate the persistent workflow, and prepare the project for component-wise unit testing.

**Problem:**
Although the crawler implementation was complete, I wanted to verify that every component interacted correctly with the others before starting GoogleTest development.

Since several components had been modified during the day, there was a possibility that the final implementation no longer matched the updated design proposal.

I also wanted to ensure that each module had a clear responsibility so that it could be tested independently without depending on the entire crawler.

**What I Tried:**

* Performed a complete code review of the crawler workflow from seed URL to page storage.
* Compared the implementation against the updated design proposal to ensure consistency.
* Verified that every crawler component had a single responsibility.
* Reviewed the interaction between:

  * `Crawler`
  * `PageStorage`
  * `Database`
  * `Frontier`
  * `SeenStore`
  * `Fetcher`
  * `LinkExtractor`
  * `URLNormalizer`
* Traced different crawler execution scenarios, including:

  * Normal crawl from a seed URL.
  * Duplicate URL discovery.
  * Failed page fetch.
  * Successful page storage.
  * Recovery after application restart.

The finalized crawler workflow became:

```text
Seed URL
    |
    ▼
Normalize URL
    |
    ▼
Recover Previous State
    |
    ▼
Insert Pending URL
    |
    ▼
Frontier
    |
    ▼
Fetch Page
    |
    ▼
Extract Links
    |
    ▼
Normalize Links
    |
    ▼
Duplicate Detection
    |
    ▼
Store Pending URLs
    |
    ▼
Store HTML
    |
    ▼
Update SeenStore
```

I also reviewed the dependency structure to ensure that lower-level modules could be tested independently.

```text
Crawler
   |
   ├── URLNormalizer
   ├── Fetcher
   ├── LinkExtractor
   ├── Frontier
   ├── SeenStore
   └── PageStorage
```

Finally, I prepared a testing plan for the next development session by identifying the major functionality to be validated for each component.

The planned testing sequence was:

* URLNormalizer
* Fetcher
* LinkExtractor
* Frontier
* SeenStore
* PageStorage
* Complete crawler integration

**Outcome:**
The crawler implementation was successfully aligned with the revised design proposal.

Each component now had a clearly defined responsibility and minimal coupling with other modules, making the codebase easier to understand and suitable for independent unit testing.

This completed the implementation phase of the crawler. The next development session (July 24) would focus on writing comprehensive GoogleTest test cases for every crawler component and validating the finalized implementation.
