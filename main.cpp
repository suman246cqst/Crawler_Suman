#include <iostream>

#include "../include/Crawler.h"

using namespace std;

int main()
{
    cout << "=============================================\n";
    cout << " CRAWLER RESUME INTEGRATION TEST\n";
    cout << "=============================================\n\n";

    cout << "STEP 1\n";
    cout << "Running crawler with MAX_DEPTH = 1\n\n";

    {
        Crawler crawler(1, 20);

        crawler.crawl(
            "http://quotes.toscrape.com"
        );
    }

    cout << "\n=============================================\n";
    cout << "First crawl completed.\n";
    cout << "Restart the crawler...\n";
    cout << "=============================================\n\n";

    cout << "STEP 2\n";
    cout << "Running crawler with MAX_DEPTH = 2\n\n";

    {
        Crawler crawler(2, 40);

        crawler.crawl(
            "http://quotes.toscrape.com"
        );
    }

    cout << "\n=============================================\n";
    cout << "RESUME TEST FINISHED\n";
    cout << "=============================================\n";

    return 0;
}