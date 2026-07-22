#include <iostream>

#include "../include/Crawler.h"
#include "../include/PageStorage.h"

using namespace std;





int main()
{
    cout << "=============================================\n";
    cout << " CRAWLER RESUME INTEGRATION TEST\n";
    cout << "=============================================\n";


    /*
        FIRST CRAWL
    */

    cout << "\nSTEP 1\n";
    cout << "Running crawler MAX_DEPTH = 1\n";
    Crawler crawler(2,40);
    // crawler.crawl("http://quotes.toscrape.com");
    crawler.crawl("https://studyadda.onrender.com");



    cout << "\nAfter First Crawl\n";

    

    return 0;
}