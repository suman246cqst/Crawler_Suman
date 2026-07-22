#include "URLNormalizer.h"
#include "../../DS_Library/include/dynamicArray.h"

bool URLNormalizer::isWhiteSpace(char ch){
    return ch == ' '  ||
           ch == '\t' ||
           ch == '\n' ||
           ch == '\r' ||
           ch == '\f';
}


void URLNormalizer::trim(string& url){
    if(url.empty()){
        return;
    }
    int left = 0;
    while(left < url.length() && isWhiteSpace(url[left])){
        left++;
    }
    int right = url.length() - 1;
    while(right >= left && isWhiteSpace(url[right])){
        right--;
    }
    url = url.substr(left,right - left + 1);
}


bool URLNormalizer::startsWith(const string& str,const string& prefix){
    if(prefix.length() > str.length())
    {
        return false;
    }

    for(int i = 0; i < prefix.length(); i++)
    {
        if(tolower(str[i]) != tolower(prefix[i])){
            return false;
        }
    }
    return true;
}

bool URLNormalizer::isAbsoluteURL(const string& url){
    return startsWith(url, "http://") || startsWith(url, "https://");
}


bool URLNormalizer::isProtocolRelative(const string& url){
    return startsWith(url, "//");
}


bool URLNormalizer::isRootRelative(const string& url){
    return !isProtocolRelative(url) && !url.empty() && url[0] == '/';
}


bool URLNormalizer::isInvalidScheme(
    const string& url)
{
    return startsWith(url, "mailto:")     ||
           startsWith(url, "javascript:") ||
           startsWith(url, "tel:")        ||
           startsWith(url, "ftp:")        ||
           startsWith(url, "data:");
}

string URLNormalizer::removeFragment(const string& url){
    string result;
    for(int i = 0; i < url.length(); i++){
        if(url[i] == '#'){
            break;
        }
        result += url[i];
    }
    return result;
}


string URLNormalizer::removeQuery(const string& url){
    string result;
    for(int i = 0; i < url.length(); i++){
        if(url[i] == '?'){
            break;
        }
        result += url[i];
    }
    return result;
}


string URLNormalizer::removeTrailingSlash(const string& url){
    if(url.empty()){
        return url;
    }
    int schemeEnd = url.find("://");
    if(schemeEnd == string::npos){
        if(url.length() > 1 && url.back() == '/'){
            return url.substr(0,url.length() - 1);
        }

        return url;
    }
    int hostStart = schemeEnd + 3;
    int firstSlash =url.find('/', hostStart);
    // No path
    if(firstSlash == string::npos){
        return url;
    }
    // URL is exactly https://abc.com/
    if(firstSlash == url.length() - 1)
    {
        return url.substr(0,url.length()-1);
    }
    if(url.back() == '/'){
        return url.substr(0,url.length() - 1);
    }
    return url;
}

string URLNormalizer::getScheme(const string& url){
    int schemeEnd = url.find("://");
    if(schemeEnd == string::npos){
        return "";
    }
    return url.substr(0,schemeEnd + 3);
}


string URLNormalizer::getHost(const string& url){
    int schemeEnd = url.find("://");
    if(schemeEnd == string::npos){
        return "";
    }
    int hostStart = schemeEnd + 3;
    int hostEnd =url.find('/', hostStart);
    if(hostEnd == string::npos){
        return url.substr(hostStart);
    }
    return url.substr(hostStart,hostEnd - hostStart);
}


string URLNormalizer::getDirectory(const string& baseURL){
    int schemeEnd =baseURL.find("://");
    if(schemeEnd == string::npos){
        return "";
    }
    int hostStart = schemeEnd + 3;
    int pathStart =baseURL.find('/', hostStart);
    // No path
    if(pathStart == string::npos){
        return baseURL + "/";
    }

    int lastSlash =baseURL.rfind('/');

    if(lastSlash < pathStart){
        return baseURL + "/";
    }
    return baseURL.substr(0,lastSlash + 1);
}

string URLNormalizer::toLowerSchemeAndHost(const string& url){
    string result = url;
    int schemeEnd = result.find("://");
    if(schemeEnd == string::npos){
        return result;
    }
    // Lowercase scheme
    for(int i = 0; i < schemeEnd; i++)
    {
        if(result[i] >= 'A' && result[i] <= 'Z'){
            result[i] =result[i] - 'A' + 'a';
        }
    }
    int hostStart = schemeEnd + 3;
    int hostEnd = result.find('/', hostStart);
    if(hostEnd == string::npos){
        hostEnd = result.length();
    }

    // Lowercase host
    for(int i = hostStart;i < hostEnd;i++){
        if(result[i] >= 'A' && result[i] <= 'Z'){
            result[i] = result[i] - 'A' + 'a';
        }
    }
    return result;
}

string URLNormalizer::resolveDotSegments(const string& url){
    int schemeEnd = url.find("://");
    if(schemeEnd == string::npos){
        return url;
    }

    int pathStart = url.find('/', schemeEnd + 3);

    if(pathStart == string::npos)
    {
        return url;
    }

    string prefix = url.substr(0, pathStart);
    string path = url.substr(pathStart);

    bool trailingSlash =!path.empty() && path.back() == '/';

    DynamicArray<string> segments;

    int i = 1;

    while(i <= path.length()){
        string segment;
        while(i < path.length() && path[i] != '/'){
            segment += path[i];
            i++;
        }
        if(segment.empty() || segment == ".")
        {
            // Ignore
        }
        else if(segment == ".."){
            if(!segments.isEmpty()){
                segments.pop_back();
            }
        }
        else{
            segments.push_back(segment);
        }
        i++;
    }
    string result = prefix;
    if(segments.isEmpty()){
        return result + "/";
    }

    for(int j = 0;j < segments.getSize();j++){
        result += "/";
        result += segments[j];
    }

    if(trailingSlash){
        result += "/";
    }
    return result;
}

string URLNormalizer::resolveRelativeURL(const string& baseURL,const string& relativeURL)
{
    //--------------------------------------------------
    // Protocol-relative
    //--------------------------------------------------

    if(isProtocolRelative(relativeURL))
    {
        return getScheme(baseURL) + relativeURL.substr(2);
    }

    //--------------------------------------------------
    // Root-relative
    //--------------------------------------------------

    if(isRootRelative(relativeURL)){
        return getScheme(baseURL) + getHost(baseURL) + relativeURL;
    }

    //--------------------------------------------------
    // Normal relative
    //--------------------------------------------------

    return getDirectory(baseURL) + relativeURL;
}

string URLNormalizer::ensureScheme(const string& url){
    if (isAbsoluteURL(url)){
        return url;
    }
    return "http://" + url;
}

string URLNormalizer::normalize(const string& baseURL,const string& extractedURL){
    string url = extractedURL;
    //--------------------------------------------------
    // Remove surrounding spaces
    //--------------------------------------------------

    trim(url);

    if(url.empty())
    {
        return "";
    }

    //--------------------------------------------------
    // Ignore page fragments
    //--------------------------------------------------

    if(url[0] == '#')
    {
        return "";
    }

    //--------------------------------------------------
    // Reject unsupported schemes like mailto:, etc.
    //--------------------------------------------------

    if(isInvalidScheme(url))
    {
        return "";
    }

    //--------------------------------------------------
    // Remove fragment like studyadda.onrender.com/home#section
    //--------------------------------------------------

    url = removeFragment(url);

    //--------------------------------------------------
    // Remove query like google.com?q=hello
    //--------------------------------------------------

    url = removeQuery(url);

    //--------------------------------------------------
    // Absolute URL, url that starts with https or http://
    //--------------------------------------------------

    if(isAbsoluteURL(url)){
        url = toLowerSchemeAndHost(url);
        url = resolveDotSegments(url);
        url = removeTrailingSlash(url);
        return url;
    }

    //--------------------------------------------------
    // Relative URL
    //--------------------------------------------------

    url = resolveRelativeURL(baseURL,url);

    url = resolveDotSegments(url);

    url = toLowerSchemeAndHost(url);

    url = removeTrailingSlash(url);

    return url;
}

string URLNormalizer::normalize(const string& extractedURL)
{
    string url = extractedURL;

    trim(url);

    if (url.empty())
        return "";

    if (url[0] == '#')
        return "";

    if (isInvalidScheme(url))
        return "";

    url = removeFragment(url);
    url = removeQuery(url);

    // Ensure the URL is absolute
    url = ensureScheme(url);

    url = toLowerSchemeAndHost(url);
    url = resolveDotSegments(url);
    url = removeTrailingSlash(url);

    return url;
}