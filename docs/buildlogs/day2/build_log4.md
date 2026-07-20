## Session 4

**Module:** URLNormalizer Implementation
**Date:** 20-07-26  
**Duration:** 2 Hours (07:00 PM – 09:00 PM)

### Goal

Implement the complete `URLNormalizer` component based on the design prepared in the previous session and integrate it with the crawler pipeline for component-level testing.

---

### Problem

After finalizing the normalization workflow, the next challenge was implementing each stage independently while ensuring they worked together correctly.

The implementation needed to handle:

- Absolute URLs
- Relative URLs
- Root-relative URLs
- Protocol-relative URLs
- Dot segments (`.` and `..`)
- URL fragments
- Query parameters
- Host canonicalization
- Invalid URL schemes

without affecting valid URLs.

---

### What I Tried

Rather than implementing one large function, the normalizer was divided into several small helper functions, each responsible for a single task.

Implemented helper functions in the following order:

```text
Utility Functions
        ↓
URL Type Detection
        ↓
Cleanup Functions
        ↓
URL Parsing Helpers
        ↓
Canonicalization Helpers
        ↓
Relative URL Resolution
        ↓
Dot Segment Resolution
        ↓
normalize()
```

This modular design made debugging and testing significantly easier.

---

### Components Implemented

#### Utility Functions

Implemented:

- `isWhiteSpace()`
- `trim()`
- `startsWith()`

These functions are shared across the entire normalizer.

---

#### URL Classification

Implemented:

- `isAbsoluteURL()`
- `isProtocolRelative()`
- `isRootRelative()`
- `isInvalidScheme()`

These determine how an extracted hyperlink should be processed.

---

#### URL Cleanup

Implemented:

- `removeFragment()`
- `removeQuery()`
- `removeTrailingSlash()`

Examples:

```
https://example.com/page#section
↓

https://example.com/page
```

```
https://example.com/search?q=test
↓

https://example.com/search
```

```
https://example.com/about/
↓

https://example.com/about
```

The root URL

```
https://example.com/
```

is intentionally preserved.

---

#### URL Parsing Helpers

Implemented:

- `getScheme()`
- `getHost()`
- `getDirectory()`

These functions are later used while resolving relative URLs.

---

#### Host Canonicalization

Implemented:

- `toLowerSchemeAndHost()`

Discussed why only the scheme and host are converted to lowercase while leaving the path unchanged because many web servers treat paths as case-sensitive.

Example:

```
HTTPS://Google.COM/About

↓

https://google.com/About
```

---

#### Dot Segment Resolution

Implemented:

- `resolveDotSegments()`

This function resolves navigation symbols within paths.

Examples:

```
/a/b/../c

↓

/a/c
```

```
/a/./b

↓

/a/b
```

```
../../notes
```

removes parent directories one level at a time.

Discussed the purpose of

```cpp
bool trailingSlash
```

which remembers whether the original URL ended with `/` so that the reconstructed path preserves the same structure.

---

#### Relative URL Resolution

Implemented:

- `resolveRelativeURL()`

Supported:

```
../page.html

./notes

/about

page.html

//cdn.example.com/file.css
```

Each type is converted into a complete absolute URL using the base webpage.

---

#### Final Normalization Pipeline

Implemented:

```cpp
normalize()
```

Pipeline:

```
Trim
 ↓
Reject Empty URL
 ↓
Reject Fragment-only URL
 ↓
Reject Unsupported Scheme
 ↓
Remove Fragment
 ↓
Remove Query
 ↓
Absolute ?
 ↓
Resolve Relative URL
 ↓
Resolve Dot Segments
 ↓
Lowercase Scheme & Host
 ↓
Remove Trailing Slash
 ↓
Return Normalized URL
```

---

### Problems Encountered

#### Problem 1

Understanding how `removeTrailingSlash()` should behave.

Initially considered removing every trailing slash.

After discussion, decided that

```
https://example.com/
```

must remain unchanged because it represents the website root, whereas

```
https://example.com/about/
```

should become

```
https://example.com/about
```

---

#### Problem 2

Understanding why only the scheme and host are converted to lowercase.

Conclusion:

Only

```
https://GOOGLE.COM
```

becomes

```
https://google.com
```

The path remains unchanged to preserve case-sensitive resources.

---

#### Problem 3

Understanding the dot-segment algorithm.

Worked through several examples to verify that:

- `.` keeps the current directory.
- `..` removes the previous directory.
- Empty segments are ignored.

---

#### Problem 4

Verified that the overall `LinkExtractor` algorithm runs in **O(n)** time because every HTML character is examined at most a constant number of times.

---

### Integration Testing

Updated the crawler test program to verify every component implemented so far.

The execution pipeline became:

```
Fetcher
    ↓
PageStorage
    ↓
LinkExtractor
    ↓
URLNormalizer
    ↓
SeenStore
    ↓
Frontier
```

Additional improvements:

- Replaced manually created URLs with links extracted directly from fetched HTML.
- Added normalization before inserting URLs into `SeenStore`.
- Introduced `MAX_DEPTH = 2` to limit crawler expansion during testing.
- Added tests for invalid URLs, unknown pages, and duplicate detection.

---

### Documentation

Prepared detailed documentation for:

- LinkExtractor workflow
- URLNormalizer workflow
- Helper function explanations
- `removeTrailingSlash()` logic
- `resolveDotSegments()` algorithm
- Time complexity analysis
- Flowcharts explaining both LinkExtractor and URLNormalizer

Also planned a structured Git history by dividing the URLNormalizer implementation into logical commits to clearly demonstrate the project's evolution.

---

### Outcome

Successfully completed the implementation of the `URLNormalizer` component and integrated it into the crawler pipeline.

At the end of the session, the crawler workflow became:

```
Fetcher
    ↓
PageStorage
    ↓
LinkExtractor
    ↓
URLNormalizer
    ↓
SeenStore
    ↓
Frontier
```

All crawler components developed up to this stage were successfully tested together, establishing a complete end-to-end crawling pipeline ready for the next phase of development.