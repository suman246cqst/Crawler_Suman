# Build Log · Session 1

**Date:** 18-07-2026

**Duration:** 2 Hours

## Goal

Design and implement the **Fetcher** component for the web crawler. The objective was to create a reusable module capable of downloading the HTML source of a webpage and returning all necessary information required by the crawler, including the response status, downloaded HTML, and any network-related errors.

## Problem

Since the crawler relies entirely on the Fetcher to obtain webpage content, it was necessary to build a reliable implementation that could communicate with web servers using HTTP requests. The Fetcher needed to correctly handle successful responses, HTTP redirects, network failures, invalid URLs, and server timeouts. Another challenge was collecting the incoming HTML data efficiently because libcurl delivers the response in multiple chunks through a callback function rather than returning it directly.

## What I Tried

- Explored the libcurl library and understood its initialization, configuration, request execution, and cleanup process.
- Designed the `FetchResult` structure to encapsulate all relevant information including the requested URL, downloaded HTML, HTTP status code, success flag, and descriptive error message.
- Implemented the Fetcher constructor to initialize the global libcurl environment and the destructor to release allocated resources.
- Implemented the write callback function responsible for continuously appending received data into a `std::string` as the webpage was being downloaded.
- Configured the HTTP request using `curl_easy_setopt()` with the required options such as target URL, callback function, callback data, automatic redirect following, timeout duration, automatic compression support, and a custom User-Agent.
- Retrieved the HTTP response code after the request completed successfully using `curl_easy_getinfo()`.
- Added comprehensive error handling to report network failures, DNS resolution errors, connection failures, and request timeouts without causing the crawler to terminate unexpectedly.
- Tested the implementation using multiple publicly accessible websites to verify correct HTML retrieval and response handling.
- Verified that the Fetcher correctly returned meaningful error messages when invalid or unreachable URLs were supplied.

## Outcome

Successfully completed the implementation of the Fetcher component for static web pages. The module is now capable of downloading HTML content, returning HTTP status codes, reporting detailed error information, and handling redirects and network failures reliably. This implementation provides the foundation upon which the remaining crawler components such as the LinkExtractor, Frontier, and PageStorage can operate during the crawling process.