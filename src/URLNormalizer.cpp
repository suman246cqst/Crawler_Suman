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