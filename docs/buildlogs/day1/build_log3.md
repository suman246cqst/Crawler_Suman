# Build Log · Session 3

**Date:** 18-07-2026

**Duration:** 2 Hours

## Goal

Implement the **SeenStore** component responsible for tracking previously visited URLs during the crawling process. The objective was to prevent duplicate webpage visits by maintaining a fast lookup mechanism using the custom `HashMap` developed in the Data Structures Library. The component was also integrated with the Fetcher and Frontier to perform an initial end-to-end validation of the crawler workflow.

## Problem

The primary challenge was designing an efficient mechanism to determine whether a URL had already been visited. Since the crawler may encounter the same webpage through multiple hyperlinks, duplicate detection had to be performed quickly to avoid unnecessary network requests and repeated processing.

Another challenge was integrating the SeenStore with the existing HashMap implementation while ensuring that URLs were inserted only once and duplicate lookups produced consistent results. During integration, linker errors related to the `HashFunction` implementation were encountered because the required source files were not correctly included in the build configuration. Additionally, multiple definition errors occurred due to the implementation strategy used in the Data Structures Library, where template implementation files were included through headers while also being compiled separately.

Finally, the interaction between the SeenStore, Frontier, and Fetcher needed to be validated to ensure that newly discovered URLs were scheduled only if they had not been visited previously.

## What I Tried

- Designed the SeenStore component as a wrapper around the custom `HashMap<string, bool>`.
- Implemented the `insert()` function to record newly visited URLs.
- Implemented the `contains()` function to efficiently determine whether a URL had already been crawled.
- Added `count()` to track the total number of stored URLs.
- Implemented `clear()` to reset the SeenStore for testing purposes.
- Investigated linker errors related to the `HashFunction` implementation and corrected the project build configuration.
- Resolved multiple definition errors caused by compiling template implementation files separately.
- Developed an integration test combining the Fetcher, Frontier, and SeenStore.
- Inserted a seed URL into both the Frontier and SeenStore before beginning the crawl simulation.
- Simulated extracted hyperlinks using a predefined URL list and verified that duplicate URLs were rejected while unseen URLs were inserted into both the SeenStore and Frontier.
- Tested the `clear()` function and verified that the SeenStore could be reused after being reset.
- Verified correct handling of invalid URLs during the integration test.

## Outcome

Successfully completed the implementation of the SeenStore component and integrated it with the crawler. The component correctly detected duplicate URLs, prevented repeated insertions, maintained an accurate count of visited pages, and supported resetting through the `clear()` function. The integration test confirmed that the Fetcher, Frontier, and SeenStore worked together as expected, providing a functional foundation for the next stage of development, where extracted hyperlinks will be generated dynamically by the LinkExtractor instead of using simulated data.