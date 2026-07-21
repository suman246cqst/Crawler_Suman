**Module:** Integration of all the crawler components
**Date:** 21-07-2026

---

# Section 1 — Specific Bug

## Runtime Observation During Complete Crawler Integration

Today's objective was not to work on an isolated module but to integrate every crawler component into a complete crawling pipeline using `Crawler::crawl()`. After completing the integration, I tested the crawler using the seed URL:

```text
http://quotes.toscrape.com
```

During testing, I observed that the crawler stored the homepage twice inside `PageStorage`.

```text
1|http://quotes.toscrape.com|0
2|http://quotes.toscrape.com/|1
```

The crawler considered both URLs different because duplicate detection relied on direct string comparison.

While verifying additional test cases, I also noticed different textual representations of the same URL reaching the crawler.

Examples:

```text
HTTP://Quotes.TOScrape.com
http://quotes.toscrape.com
```

```text
/about
./about
../about
```

These observations indicated that the crawler integration was correct, but URL normalization still needed refinement before duplicate detection.

---

# Section 2 — Failed Attempt

The first half of today's work focused on integrating all crawler modules into a single execution pipeline.

I implemented the `Crawler::crawl()` function by connecting every previously developed component. The execution flow I designed was:

```
Frontier Queue
      ↓
SeenStore
      ↓
Fetcher
      ↓
PageStorage
      ↓
LinkExtractor
      ↓
URLNormalizer
      ↓
Frontier Queue
```

While writing the integration logic, I decided the execution order carefully so that:

- every URL is fetched only once,
- only successfully fetched pages are stored,
- links are extracted only after successful fetching,
- normalized URLs are inserted back into the frontier,
- crawling stops when either the maximum depth or maximum page limit is reached.

After implementing the pipeline, I tested the crawler on a static website. The crawler was successfully downloading pages, extracting hyperlinks, storing HTML files, and continuously expanding the frontier.

However, during verification I discovered duplicate homepage entries.

Initially, I assumed the issue was inside `SeenStore`, believing duplicate URLs were being inserted into the HashMap. After reviewing the duplicate detection logic, I found that `SeenStore` was behaving correctly.

My next assumption was that `PageStorage::hasPage()` was failing to detect previously stored pages. After tracing the storage logic and checking the generated index file, I confirmed that storage was functioning correctly as well.

Finally, I traced the problem back to the normalization stage. URLs were reaching duplicate detection in different textual forms even though they referred to the same webpage.

To solve this problem, I improved the URL normalization pipeline by:

- trimming surrounding whitespace,
- rejecting unsupported schemes (`mailto`, `javascript`, `ftp`, etc.),
- removing URL fragments,
- removing query parameters,
- resolving protocol-relative URLs,
- resolving root-relative URLs,
- resolving normal relative URLs,
- resolving dot segments (`.` and `..`),
- converting the scheme and hostname to lowercase,
- removing unnecessary trailing slashes from root URLs.

I also revisited the `LinkExtractor` and verified that it correctly handled:

- uppercase and lowercase `<a>` tags,
- uppercase and lowercase `href` attributes,
- single-quoted attributes,
- double-quoted attributes,
- unquoted attribute values,
- arbitrary whitespace between HTML attributes.

After these refinements, repeated crawling of the same page through different URL representations was eliminated.

Finally, I documented the complete crawler execution flow by designing a detailed architecture diagram and memory diagram to verify that every component interacted in the intended sequence.

---

# Section 3 — Memory Diagram

After completing the crawler integration, I created a complete execution flow and memory diagram to verify the interaction between every component.

**Crawler Execution Flow & Memory Diagram**

![Crawler Flow and Memory Diagram](../images/Crawler%20Flow%20Diagram.png)

The diagram represents the complete execution of the crawler.

It illustrates:

- creation of the initial Frontier Queue,
- BFS-based crawling process,
- duplicate detection using `SeenStore`,
- page downloading through the Fetcher,
- storage of HTML pages using `PageStorage`,
- extraction of hyperlinks from HTML,
- normalization of extracted URLs,
- reinsertion of valid URLs into the Frontier Queue,
- crawler termination based on maximum depth and maximum page constraints.

Preparing this diagram also helped verify that the implementation matched the intended crawler architecture before moving to the next development stage.

---

# Section 4 — Code Reference

### Commit Hash

```text
<commit-hash-after-today's-final-commit>
```

---

### Files Worked On

```text
Crawler/src/crawler.cpp

Crawler/src/URLNormalizer.cpp

Crawler/src/LinkExtractor.cpp

Crawler/src/PageStorage.cpp

Crawler/src/SeenStore.cpp
```

---

### Major Functions

```text
Crawler::crawl()

URLNormalizer::normalize()

URLNormalizer::resolveRelativeURL()

URLNormalizer::resolveDotSegments()

URLNormalizer::removeTrailingSlash()

URLNormalizer::toLowerSchemeAndHost()

LinkExtractor::extractLinks()

LinkExtractor::isAnchorTag()

PageStorage::storePage()

PageStorage::hasPage()

SeenStore::insert()

SeenStore::contains()
```

---

# Section 5 — Learning Reflection

Today's work was different from previous development sessions because I shifted from implementing individual modules to integrating the entire crawler into a single working system.

Before writing `Crawler::crawl()`, I had developed each component independently, but they had never been executed together as one pipeline. Integrating them required deciding the exact sequence of operations and clearly defining the responsibility of each module. I realized that the crawler itself should act only as an orchestrator, while each component should remain responsible for a single task. The `Fetcher` retrieves HTML, `LinkExtractor` extracts raw hyperlinks, `URLNormalizer` converts them into canonical URLs, `SeenStore` prevents revisiting pages, and `PageStorage` persists successfully fetched pages. Keeping these responsibilities separate made the overall design much easier to reason about and debug.

Testing the integrated crawler also showed me that a system can appear to work correctly while still producing incorrect results. Pages were being downloaded successfully, links were being extracted, and files were being stored, yet duplicate homepage entries revealed that URL normalization was incomplete. This debugging process reinforced the idea that correctness depends not only on individual modules but also on how they interact with one another.

Another important lesson was understanding the role of canonical URLs in web crawling. I previously thought duplicate detection depended mainly on the HashMap implementation, but today's debugging showed that duplicate detection is only as reliable as the normalized input it receives. Two logically identical URLs become different keys if normalization is inconsistent. Because of this, URL normalization became an essential stage of the crawler rather than just a utility function.

Designing the complete crawler flowchart and memory diagram also improved my understanding of the runtime behavior of the system. Drawing the data flow between the Frontier Queue, SeenStore, Fetcher, PageStorage, LinkExtractor, and URLNormalizer allowed me to compare the implementation with the intended architecture and verify that each component communicated in the correct order.

By the end of today's work, I had completed the first fully integrated version of the crawler, verified its execution flow, resolved duplicate URL issues through improved normalization, validated link extraction across different HTML formats, and documented the architecture for future development. This session helped me transition from writing individual modules to thinking about the crawler as a complete software system rather than a collection of independent classes.