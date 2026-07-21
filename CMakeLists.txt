cmake_minimum_required(VERSION 3.20)

project(Fetcher)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

include_directories(include)

add_library(fetcher
    src/Fetcher.cpp
    src/Frontier.cpp
    src/SeenStore.cpp
    src/PageStorage.cpp
    src/LinkExtractor.cpp
    src/URLNormalizer.cpp
    src/crawler.cpp
    ../../DS_Library/src/HashFunction.cpp
)

target_link_libraries(fetcher
    CURL::libcurl
)

add_executable(testFetcher
    main.cpp
)

target_link_libraries(testFetcher
    fetcher
)