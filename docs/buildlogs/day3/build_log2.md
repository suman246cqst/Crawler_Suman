**Module:** Modification in Link Extractor

**Date:** July 21, 2026

**Duration:** 2 Hours

## Goal
Improve the robustness of the `LinkExtractor` by handling HTML's case-insensitive nature correctly and verify that hyperlinks are extracted reliably from different forms of `<a>` tags.

---

## Problem

During integration testing, I realized that the current implementation assumed HTML tags and attributes would always be written in lowercase.

Examples like:

```html
<A HREF="page.html">
<a HREF="page.html">
<A href="page.html">
<a HrEf="page.html">
```

were either partially detected or ignored completely.

Since HTML is case-insensitive for tag names and attribute names, my extractor could miss valid hyperlinks simply because the page author used uppercase or mixed-case letters.

Another concern was implementing this support without converting the entire HTML document to lowercase, since changing the original HTML could modify the contents of attribute values.

---

## What I Tried

### Investigated HTML Case Sensitivity

Reviewed how browsers parse HTML and confirmed that:

- HTML tag names are case-insensitive.
- HTML attribute names are also case-insensitive.
- Attribute values (such as URLs) must not be modified.

This meant only comparisons should ignore case while preserving the original HTML content.

---

### Updated Anchor Tag Detection

Modified the anchor tag detection logic.

Previously the code checked:

```cpp
html[index + 1] == 'a'
```

which only detected lowercase `<a>`.

Updated the comparison using `tolower()` so that the extractor correctly recognizes:

- `<a>`
- `<A>`
- `<a>`
- `<A>`

without affecting any other part of the HTML.

---

### Updated `startsWith()` Function

The helper function used for detecting `"href"` originally performed a character-by-character case-sensitive comparison.

Modified the function to compare characters using `tolower()`.

As a result, all of the following are now treated identically:

```
href
HREF
Href
HrEf
hReF
```

This change improved every place where `startsWith()` is used without changing the stored URL itself.

---

### Tested Different HTML Variations

Created several HTML snippets using different combinations of uppercase and lowercase tag names and attribute names.

Examples included:

```html
<A HREF="one.html">
<a HREF="two.html">
<A href="three.html">
<a HrEf="four.html">
```

Verified that every hyperlink was extracted successfully.

---

### Verified URL Preservation

Ensured that only tag and attribute comparisons became case-insensitive.

The extracted URL itself remained unchanged.

For example,

```html
<A HREF="Images/Logo.PNG">
```

still produces

```
Images/Logo.PNG
```

instead of converting the path to lowercase.

---

## Outcome

Successfully updated the `LinkExtractor` to follow HTML's case-insensitive parsing rules while preserving the original attribute values.

The crawler can now recognize anchor tags and `href` attributes written in any combination of uppercase and lowercase letters, making link extraction significantly more reliable across websites.

During further integration testing, another issue became apparent: URLs such as

```
http://quotes.toscrape.com
```

and

```
http://quotes.toscrape.com/
```

were still being treated as different pages. This shifted the focus of the next development session toward improving URL normalization and eliminating duplicate URL representations.