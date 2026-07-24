**Module:** Integration Test Setup

**Date:** July 21, 2026

**Duration:** 2 Hours

## Goal
Perform end-to-end testing of the fully integrated crawler, verify resume functionality, and validate that all modules work together correctly after the fixes made during integration, link extraction, and URL normalization.

---

## Problem

Although the crawler was now able to fetch pages, extract hyperlinks, and normalize URLs, it was still necessary to verify that the entire system behaved correctly over multiple executions.

The primary concerns were:

- Whether the crawler could resume from a previous crawl instead of starting from scratch.
- Whether duplicate URLs would still appear after normalization improvements.
- Whether the interaction between all modules remained stable after recent code changes.
- Whether the crawler respected the configured maximum crawl depth during resume operations.

---

## What I Tried

### Prepared an Integration Test

Created a dedicated `main.cpp` for integration testing.

Configured the crawler to execute in two stages:

**Stage 1**

- Start crawling from the seed URL.
- Set maximum depth to **1**.
- Crawl a limited portion of the website.
- Save crawler state before exiting.

**Stage 2**

- Restart the application.
- Increase maximum depth to **2**.
- Reload the previous crawler state.
- Continue crawling instead of beginning from the seed URL again.

This setup helped verify that resume functionality worked correctly.

---

### Verified Resume Mechanism

Observed the crawler after restarting the application.

Checked whether it:

- Loaded previously saved crawl information.
- Continued processing remaining URLs.
- Avoided revisiting already processed pages.
- Expanded crawling only from unfinished URLs.

Confirmed that the crawler resumed from its previous state rather than repeating the entire crawl.

---

### Tested Complete Crawling Pipeline

Repeated the complete execution multiple times while monitoring every stage of the crawler.

Verified that the execution order remained:

```
Frontier
      ↓
Visited URL Check
      ↓
Fetcher
      ↓
PageStore
      ↓
LinkExtractor
      ↓
URLNormalizer
      ↓
Frontier
```

Confirmed that each module correctly passed data to the next without breaking the crawling workflow.

---

### Validated Previous Fixes

Repeated testing after implementing:

- Case-insensitive anchor tag detection.
- Case-insensitive `href` attribute matching.
- Improved URL normalization.
- Trailing slash removal.
- Canonical URL generation.

Observed that duplicate homepage URLs were no longer inserted into the PageStore.

Also confirmed that HTML pages containing uppercase HTML tags continued to produce valid hyperlinks.

---

### Reviewed Overall Code Quality

Performed a final review of the newly integrated crawler.

Checked:

- Control flow inside `Crawler.cpp`.
- Interaction between helper classes.
- Edge cases in URL extraction.
- Overall readability of the implementation.
- Consistency of helper functions across modules.

Made minor cleanup changes where necessary to improve maintainability.

---

## Outcome

Successfully completed the first fully integrated and tested version of the crawler.

Verified that:

- Resume functionality operates correctly across multiple executions.
- The crawler respects maximum crawl depth.
- HTML links are extracted correctly regardless of tag or attribute case.
- URL normalization prevents duplicate URL representations.
- All crawler components work together as a complete system.

The crawler is now capable of performing a continuous crawl, resuming previous work, and maintaining a cleaner set of normalized URLs, providing a stable foundation for future enhancements such as improved duplicate detection, robots.txt support, and crawl optimizations.