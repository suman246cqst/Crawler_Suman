#ifndef URLNORMALIZER_H
#define URLNORMALIZER_H

#include <string>

using namespace std;

class URLNormalizer{
private:
    bool isWhiteSpace(char ch);

    void trim(string& url);
    
    bool startsWith(const string& str,const string& prefix);

    bool isInvalidScheme(const string& url);

    bool isAbsoluteURL(const string& url);

    bool isProtocolRelative(const string& url);

    bool isRootRelative(const string& url);

    string removeFragment(const string& url);

    string removeQuery(const string& url);

    string removeTrailingSlash(const string& url);

    string toLowerSchemeAndHost(const string& url);

    string getScheme(const string& url);

    string getHost(const string& url);

    string getDirectory(const string& baseURL);

    string resolveRelativeURL(const string& baseURL,const string& relativeURL);

    string resolveDotSegments(const string& url);

public:
    string normalize(const string& baseURL,const string& extractedURL);
};

#endif