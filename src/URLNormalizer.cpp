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
        if(str[i] != prefix[i])
        {
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
        return url;
    }
    if(url.back() == '/'){
        return url.substr(0,url.length() - 1);
    }
    return url;
}