# Module: Crawler Resume Workflow Analysis and Frontier Persistence Investigation

**Date:** July 22, 2026

**Duration:** 2 Hours

## Goal

Analyze the crawler's resume workflow in detail and determine why crawling could not continue consistently after application restart even though the visited URL set was being restored correctly.

---

## Problem

After integrating the persistent `SeenStore`, previously downloaded pages were successfully reconstructed from `index.txt` during startup.

However, during testing I noticed the following situation:

```text
A
├── B
├── C
└── D
```

- `A` was fetched.
- Links `B`, `C`, and `D` were discovered and fetched.
- All three URLs were inserted into `SeenStore`.

After restarting the crawler and starting again from `A`:

- `A` was fetched.
- Links `B`, `C`, and `D` were extracted again.
- Since all three were already present in `SeenStore`, they were skipped and never added back into the Frontier.

As a result, the crawler could not continue exploring the descendants of `B`, `C`, and `D`, revealing that the current resume mechanism was incomplete.

---

## What I Tried

### Traced the Complete Restart Sequence

Walked through the crawler execution step by step after a restart:

```text
Seed URL
   ↓
Fetch A
   ↓
Extract B,C,D
   ↓
SeenStore contains B,C,D
   ↓
Skip enqueueing
   ↓
Frontier becomes empty
```

This confirmed that the crawler was losing traversal state between executions.

---

### Compared SeenStore and Frontier Responsibilities

Separated the responsibilities of both structures:

- **SeenStore**: records URLs that have already been fetched.
- **Frontier**: records URLs that still need to be processed or expanded.

Realized that restoring only the visited set does not tell the crawler which URLs still have unexplored outgoing links.

---

### Investigated Possible Resume Strategies

Explored several approaches:

- Re-fetching visited pages after restart.
- Persisting the entire Frontier to disk.
- Storing an additional "expanded / not expanded" state for every URL.
- Rebuilding the Frontier by analyzing stored pages.

Evaluated the trade-offs of each approach and concluded that Frontier persistence is the most reliable long-term solution.

---

### Reviewed Current Crawler Logic

Re-examined the condition:

```cpp
if(!seen.contains(normalizedURL))
{
    frontier.push(...);
}
```

Confirmed that the logic is correct for normal crawling but becomes insufficient when the Frontier state has been lost after application termination.

---

## Outcome

Identified the root cause of the inconsistent resume behaviour: **the Frontier is currently maintained only in memory and is not persisted across executions**.

The crawler can correctly remember which pages were already downloaded, but it cannot remember which pages were still pending exploration. This session established that a fully consistent resume mechanism will require persistent Frontier storage or an equivalent representation of unfinished crawl state.