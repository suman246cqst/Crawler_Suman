# Build Log · Session 2

**Date:** 18-07-2026

**Duration:** 2 Hours

## Goal

Implement the **Queue** data structure in the Data Structures Library and use it to build the **Frontier** component for the crawler. The objective was to provide a reliable FIFO scheduling mechanism capable of storing URLs along with their crawl depth, enabling the crawler to perform breadth-first traversal of webpages.

## Problem

During the implementation of the Queue and Frontier components, several design and integration issues were encountered. Since the Frontier was built on top of the custom Queue implementation from the Data Structures Library, correctness of the underlying queue became critical.

The Queue implementation initially had inconsistencies in function signatures, particularly regarding parameter passing and return types. Special attention was required while implementing the `dequeue()` operation to ensure that objects removed from the queue were returned safely without leaving dangling references. Additionally, the queue had to correctly handle empty states while maintaining FIFO ordering throughout multiple enqueue and dequeue operations.

While designing the Frontier, it was necessary to determine an efficient way to store both the webpage URL and its corresponding crawl depth together. This led to the introduction of the `URLDepth` class, which encapsulated both pieces of information as a single object. Equality comparison also required careful consideration so that duplicate URLs could later be identified independently of their crawl depth.

During integration with the crawler project, several compilation issues were encountered, including mismatched function declarations and definitions, incorrect use of `const`, typographical mistakes in function signatures, and multiple definition errors caused by the project's implementation strategy. These issues required careful inspection of the project structure and build process before successful compilation could be achieved.

## What I Tried

- Completed the implementation of the generic Queue class using the previously developed LinkedList as the underlying storage container.
- Implemented the core queue operations including `enqueue()`, `dequeue()`, `peek()`, `size()`, and `isEmpty()`.
- Corrected function signatures to use appropriate const references where required.
- Ensured that the dequeue operation safely returned queue elements while maintaining the integrity of the underlying data structure.
- Designed the `URLDepth` structure to store both the URL and crawl depth together.
- Implemented the Frontier component as a wrapper around the custom Queue instead of exposing queue operations directly.
- Added the Frontier member functions `push()`, `pop()`, `front()`, `empty()`, and `size()` to simplify URL scheduling.
- Corrected compilation issues caused by declaration-definition mismatches, incorrect qualifiers, and multiple definition errors.
- Performed extensive testing by inserting URLs with different depth values, verifying FIFO traversal, checking queue size after every operation, and validating the behavior of an empty Frontier.
- Reused the Frontier after it became empty to ensure that the scheduling mechanism remained stable across multiple crawling cycles.

## Outcome

Successfully completed the Queue implementation in the Data Structures Library and integrated it into the crawler through the Frontier component. The Queue maintained correct FIFO behavior under all tested scenarios, while the Frontier correctly stored URL-depth pairs and provided reliable scheduling operations. After resolving the compilation and integration issues, the component passed all functional tests and became ready for use by the crawler's Fetcher and SeenStore components in subsequent development sessions.