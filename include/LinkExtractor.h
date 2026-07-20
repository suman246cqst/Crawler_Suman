#ifndef LINKEXTRACTOR_H
#define LINKEXTRACTOR_H

#include <string>

#include "../../DS_Library/include/dynamicArray.h"

using namespace std;

class LinkExtractor{
private:
    bool isWhiteSpace(char ch);
    void skipWhiteSpace(const string& html, int& index);
    bool startsWith(const string& html, int index, const string& word);
    bool isAnchorTag(const std::string& html,int index);
public:
    DynamicArray<string> extractLinks(const string& html);
};
#endif