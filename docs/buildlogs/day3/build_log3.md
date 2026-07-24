**Module:** Modification in URL Normalizer

**Date:** July 21, 2026

**Duration:** 2 Hours

## Goal
Debug duplicate URL generation by improving URL normalization and ensuring that different textual representations of the same resource are converted into a single canonical URL.

---

## Problem

During end-to-end crawler testing, I observed that the same webpage was being stored multiple times in the PageStore.

For example,

```
1 | http://quotes.toscrape.com
2 | http://quotes.toscrape.com/
```

Although both URLs point to the same homepage, the crawler treated them as two different URLs because their textual representations were different.

While investigating this issue, I also noticed that URLs containing uppercase schemes or host names could also create duplicate entries.

Examples:

```
HTTP://Quotes.Toscrape.com
http://quotes.toscrape.com
```

Similarly, I reviewed how protocol-relative URLs, root-relative URLs, and relative URLs were being resolved to ensure every extracted hyperlink eventually produced a consistent canonical URL.

---

## What I Tried

### Investigated Duplicate Homepage URLs

Traced the complete URL normalization pipeline to determine where duplicate URLs were being introduced.

Followed the flow:

```
Extracted URL
        ↓
Resolve Relative URL
        ↓
Resolve Dot Segments
        ↓
Lowercase Scheme & Host
        ↓
Remove Trailing Slash
        ↓
Normalized URL
```

Compared the normalized outputs of different homepage links and confirmed that the trailing slash was responsible for creating duplicate entries.

---

### Improved Trailing Slash Handling

Reviewed the implementation of `removeTrailingSlash()`.

Initially, the function preserved URLs ending with:

```
http://example.com/
```

This caused both

```
http://example.com
```

and

```
http://example.com/
```

to coexist.

Modified the logic so that the homepage trailing slash is removed, producing a single canonical representation.

Verified that:

```
http://example.com/
```

becomes

```
http://example.com
```

while URLs containing actual paths such as

```
http://example.com/about/
```

continue to normalize correctly.

---

### Reviewed Relative URL Resolution

Tested different forms of extracted hyperlinks, including:

- Absolute URLs
- Root-relative URLs
- Relative URLs
- Protocol-relative URLs

Verified that each category was resolved into the expected absolute URL before further normalization.

Paid particular attention to homepage links such as:

```
href="/"
```

to ensure they correctly normalize to the website root without producing duplicate entries.

---

### Investigated Case Sensitivity in URLs

Studied which parts of a URL are considered case-insensitive.

Confirmed that:

- URL schemes are case-insensitive.
- Host names are case-insensitive.
- URL paths should remain unchanged because many servers treat them as case-sensitive.

Based on this understanding, updated comparison functions used for scheme detection so that URLs such as:

```
HTTP://Example.com
```

and

```
http://example.com
```

are recognized correctly.

Verified that only the scheme and host are converted to lowercase while preserving the original path.

---

### Performed End-to-End Verification

Repeated crawling on the test website after each modification.

Compared PageStore entries before and after normalization improvements.

Confirmed that duplicate homepage URLs were no longer inserted because every variation now normalized to a single canonical form.

---

## Outcome

Successfully eliminated duplicate URL generation caused by inconsistent URL representations.

The URL normalizer now produces a more consistent canonical URL by:

- Resolving relative URLs correctly.
- Handling protocol-relative and root-relative links.
- Removing unnecessary trailing slashes.
- Lowercasing only the scheme and host.
- Preserving the original path to avoid altering valid resources.

With URL normalization behaving consistently, the crawler became significantly more reliable and produced a cleaner PageStore with fewer duplicate entries, preparing the project for final integration and resume testing.