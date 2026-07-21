**Module:** Crawler Components Integration
**Date:** July 22, 2026

**Duration:** 2 Hours

## Goal
Integrate all previously developed crawler modules into a single working crawler so that the crawling process could execute automatically from a seed URL without manually invoking each component.

---

## Problem

Until this session, every module had been implemented independently, but there was no central controller responsible for coordinating the complete crawling workflow. The crawler needed to:

- Retrieve URLs from the Frontier.
- Skip URLs that had already been visited.
- Fetch HTML pages.
- Store downloaded pages.
- Extract hyperlinks from the fetched HTML.
- Normalize extracted URLs.
- Add newly discovered URLs back into the Frontier with the correct depth.

Another challenge was deciding the correct order of execution for every component so that unnecessary fetches and duplicate processing could be avoided.

---

## What I Tried

### Designed the Complete Crawling Pipeline

Implemented `Crawler.cpp` as the central coordinator responsible for managing the entire crawling process.

Designed the execution flow as:

1. Read the next URL from the URL Frontier.
2. Check whether the URL had already been visited.
3. If not visited, mark it as visited.
4. Download the HTML page using the Fetcher.
5. Save the HTML into PageStore.
6. Pass the HTML to LinkExtractor.
7. Normalize every extracted hyperlink.
8. Filter invalid URLs.
9. Insert valid URLs into the Frontier with incremented crawl depth.
10. Continue until the Frontier becomes empty or the maximum crawl depth is reached.

---

### Connected Individual Modules

Integrated the following components together:

- URL Frontier
- Fetcher
- LinkExtractor
- URLNormalizer
- VisitedURLManager
- PageStore

Verified that the output of one module became the correct input for the next module without additional manual processing.

---

### Added Depth Management

Integrated the crawler's maximum depth logic.

Before inserting newly discovered URLs into the Frontier, ensured that the next depth would not exceed the configured crawl limit.

This prevented unnecessary crawling beyond the required level.

---

### Integrated Page Storage

After every successful fetch:

- Stored the downloaded HTML page in PageStore.
- Associated every stored page with its corresponding normalized URL.

This ensured that downloaded pages could later be reused without fetching them again.

---

### Verified End-to-End Workflow

Executed the crawler on a small website and observed the complete execution sequence.

Confirmed that:

- URLs were fetched successfully.
- HTML pages were downloaded correctly.
- Hyperlinks were extracted from the downloaded HTML.
- Normalized URLs were inserted back into the Frontier.
- Crawling continued automatically until no URLs remained or the depth limit was reached.

---

## Outcome

Successfully completed the first fully integrated version of the crawler.

The crawler was able to coordinate every major component and execute the complete crawling pipeline automatically.

While testing the integrated system, I observed that some pages were being stored multiple times because different textual representations of the same URL (such as trailing slashes and case differences) were treated as separate URLs. This became the primary issue identified for investigation and debugging during the next development session.