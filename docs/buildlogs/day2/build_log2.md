## Session 2

**Modeule:** Link Extractor Implementation
**Date:** 20-07-26  
**Duration:** 2 Hours 

### Goal

Complete the implementation of the `LinkExtractor` component to accurately extract hyperlinks (`href` values) from HTML pages fetched by the crawler.

---

### Problem

The crawler could successfully download HTML pages using the `Fetcher`, but it had no mechanism to discover new webpages from the downloaded HTML.

The major challenge was designing a parser that could correctly identify anchor (`<a>`) tags while avoiding false matches caused by malformed HTML or similar tag names.

---

### What I Tried

Initially designed the extraction algorithm as a sequential HTML scanner.

The workflow decided was:

```text
HTML
 ↓
Scan Character by Character
 ↓
Detect <a Tag
 ↓
Read Attributes
 ↓
Find href Attribute
 ↓
Extract URL
 ↓
Store URL
```

Rather than using regular expressions or external HTML parsing libraries, decided to implement a lightweight parser manually because the crawler only requires hyperlink extraction.

---

### Design Decisions

#### Decision 1

An opening anchor tag should only be accepted if it satisfies

```html
<a
```

or

```html
<a>
```

Examples:

Valid

```html
<a href="page.html">
<a class="btn">
<a>
```

Invalid

```html
<abc>
<anchor>
```

To implement this safely, introduced the helper function

```cpp
isAnchorTag()
```

which verifies that the character immediately following `<a` is either whitespace or `>`.

---

#### Decision 2

While scanning attributes, whitespace may appear anywhere.

Examples:

```html
<a     href="page.html">

<a
href="page.html">

<a     class="x"      href="page.html">
```

Implemented

```cpp
skipWhiteSpace()
```

to simplify attribute parsing.

---

#### Decision 3

Support all valid HTML representations of the `href` attribute.

Examples:

```html
href="page.html"

href='page.html'

href=page.html
```

All three formats are extracted.

---

#### Decision 4

Prevent false matches while searching for `href`.

Examples rejected:

```html
hrefabc="..."

href123="..."

hrefxyz=
```

After matching the word `href`, the following character must be one of:

- whitespace
- `=`
- `>`

Otherwise the match is rejected.

---

#### Decision 5

Unknown attributes should simply be skipped.

Example:

```html
<a class="btn" id="x" target="_blank" href="page.html">
```

The parser ignores unrelated attributes until `href` is encountered.

---

### Problems Encountered

#### Problem 1

The initial implementation accepted tags such as

```html
<abc>
```

because it only checked the first two characters.

**Solution**

Introduced `isAnchorTag()` to ensure that `<a` is followed only by whitespace or `>`.

---

#### Problem 2

The parser incorrectly accepted strings beginning with

```text
href
```

such as

```text
hrefabc
```

because only prefix matching was performed.

**Solution**

Added boundary validation immediately after matching `href`.

---

#### Problem 3

Discussed why, after rejecting an invalid `href` match, the parser advances only one character:

```cpp
i++;
continue;
```

instead of skipping the whole token.

**Reason**

Advancing one character prevents accidentally skipping overlapping patterns.

Example:

```text
hrefhref=
```

Moving only one character guarantees that every possible starting position is examined.

---

#### Problem 4

Discussed HTML whitespace handling.

Verified that spaces, tabs, carriage returns, line feeds, and form feeds should all be treated as whitespace during parsing.

Implemented

```cpp
isWhiteSpace()
```

to centralize this logic.

---

### Outcome

Successfully completed the implementation of the `LinkExtractor` component.

Implemented helper functions:

- `isWhiteSpace()`
- `skipWhiteSpace()`
- `startsWith()`
- `isAnchorTag()`

Completed the complete hyperlink extraction workflow.

The extractor now:

- Detects only valid `<a>` tags.
- Correctly identifies valid `href` attributes.
- Supports double-quoted, single-quoted, and unquoted URLs.
- Ignores malformed attributes.
- Ignores non-anchor HTML tags.
- Extracts hyperlinks into a `DynamicArray<string>` for further processing by the crawler.

Additionally, analysed the algorithm and concluded that the overall time complexity of the `LinkExtractor` is **O(n)**, where *n* is the size of the HTML document, since each character is processed at most a constant number of times.