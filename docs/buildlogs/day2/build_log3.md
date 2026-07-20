## Session 3

**Module:** URLNormalizer Implementation
**Date:** 20-07-26  
**Duration:** 2 Hours (03:00 PM – 05:00 PM)

### Goal

Design the complete `URLNormalizer` component before implementation by identifying all possible types of hyperlinks that may be extracted from HTML pages and defining a normalization pipeline that converts equivalent URLs into a single canonical representation.

---

### Problem

The `LinkExtractor` successfully extracts hyperlinks from HTML, but the extracted links can appear in many different formats.

Examples:

```text
https://Google.COM/About
http://example.com
/about
../notes
./index.html
page.html
#section
mailto:test@gmail.com
javascript:void(0)
//cdn.example.com/file.js
https://example.com/page#section
https://example.com/search?q=test
```

Without normalization:

- Duplicate pages may be crawled multiple times.
- Relative URLs cannot be fetched directly.
- Invalid hyperlinks may enter the crawler.
- Equivalent URLs may be treated as different pages.

---

### What I Tried

Before writing code, identified every category of hyperlinks that may appear inside an HTML `href` attribute.

Discussed how browsers interpret relative URLs and how a crawler should convert them into absolute URLs.

Instead of implementing normalization directly, first designed a complete normalization workflow.

Workflow:

```text
Extracted Link
      ↓
Trim Spaces
      ↓
Reject Empty Link
      ↓
Reject Fragment-only Links
      ↓
Reject Unsupported Schemes
      ↓
Remove Fragment
      ↓
Remove Query
      ↓
Absolute URL ?
      ↓
Yes                    No
 ↓                      ↓
Normalize          Resolve Relative URL
 ↓                      ↓
Resolve Dot Segments
 ↓
Lowercase Scheme & Host
 ↓
Remove Trailing Slash
 ↓
Normalized URL
```

---

### Design Decisions

#### Decision 1

The normalizer should return a `string`.

If the extracted hyperlink is invalid, immediately return an empty string.

---

#### Decision 2

Currently support only

- HTTP
- HTTPS
- Relative URLs
- `.html` pages

Unsupported schemes are rejected.

---

#### Decision 3

Remove page fragments.

Example:

```text
https://example.com/page#section

↓

https://example.com/page
```

Reason:

The crawler downloads the complete webpage, not individual page sections.

---

#### Decision 4

Remove query parameters.

Example:

```text
https://example.com/search?q=test

↓

https://example.com/search
```

Reason:

Current crawler focuses on webpage storage rather than parameter-specific retrieval.

---

#### Decision 5

Treat HTTP and HTTPS URLs uniformly during normalization.

---

#### Decision 6

Support relative URLs.

Examples:

```text
../about

/about

./index.html

page.html
```

These must be converted into absolute URLs before insertion into the crawler.

---

#### Decision 7

Normalize every URL into a standard absolute representation before storing it inside `SeenStore`.

---

#### Decision 8

If the hyperlink is unsupported, reject it immediately.

Examples:

```text
mailto:
javascript:
tel:
ftp:
data:
```

Return an empty string.

---

#### Decision 9

Protocol-relative URLs such as

```text
//cdn.example.com/file.js
```

should inherit the scheme from the base URL.

---

#### Decision 10

Double slashes inside URL paths are not corrected.

This is left to the server because some applications intentionally use multiple slashes.

---

#### Decision 11

Convert only the URL scheme and host to lowercase.

Example:

```text
HTTPS://Google.COM/About

↓

https://google.com/About
```

The path is intentionally left unchanged because many servers are case-sensitive.

---

#### Decision 12

Explicit port normalization is not required in the current crawler implementation.

---

### Problems Encountered

#### Problem 1

Whether fragment-only links such as

```text
#top
```

should be crawled.

**Solution**

Rejected immediately because they refer to locations within the same webpage.

---

#### Problem 2

Whether query parameters should be preserved.

**Solution**

Removed them because the crawler currently stores complete webpages rather than dynamic query results.

---

#### Problem 3

Whether to convert the entire URL into lowercase.

**Solution**

Rejected this approach.

Only the scheme and host are converted to lowercase while preserving the original path.

---

#### Problem 4

Whether URLs containing unsupported schemes should be stored.

**Solution**

Rejected immediately by returning an empty string.

---

### Outcome

Completed the complete architectural design of the `URLNormalizer` component before implementation.

Defined:

- Supported URL categories.
- Invalid hyperlink handling.
- Relative URL resolution strategy.
- Canonicalization rules.
- Normalization pipeline.
- Component responsibilities.

This design served as the blueprint for implementing every helper function in the `URLNormalizer` during the next development session.