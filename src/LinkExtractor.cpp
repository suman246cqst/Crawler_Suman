#include "LinkExtractor.h"

bool LinkExtractor::isWhiteSpace(char ch){
    return ch == ' ' ||
           ch == '\t' ||
           ch == '\n' ||
           ch == '\r' ||
           ch == '\f';
}


void LinkExtractor::skipWhiteSpace(const string& html,int& index){
    while(index < html.length() && isWhiteSpace(html[index]))
    {
        index++;
    }
}


bool LinkExtractor::startsWith(const string& html,int index,const string& word){
    if(index + word.length() > html.length()){
        return false;
    }

    for(int i = 0; i < word.length(); i++){
        if(html[index + i] != word[i]){
            return false;
        }
    }
    return true;
}