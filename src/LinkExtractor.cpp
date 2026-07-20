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


bool LinkExtractor::isAnchorTag(const string& html,int index){
    if(index + 2 >= html.length()){
        return false;
    }
    if(html[index] != '<' ||html[index + 1] != 'a'){
        return false;
    }
    // After <a there must be whitespace or >
    char next = html[index + 2];
    return isWhiteSpace(next) || next == '>';
}