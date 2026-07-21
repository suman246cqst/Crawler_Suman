#include "LinkExtractor.h"
#include <string>
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
        if(tolower(html[index + i]) != tolower(word[i])){
            return false;
        }
    }
    return true;
}


bool LinkExtractor::isAnchorTag(const string& html,int index){
    if(index + 2 >= html.length()){
        return false;
    }
    if(html[index] != '<' ||tolower(html[index + 1]) != 'a'){
        return false;
    }
    // After <a there must be whitespace or >
    char next = html[index + 2];
    return isWhiteSpace(next) || next == '>';
}


DynamicArray<string> LinkExtractor::extractLinks(const string& html){
    DynamicArray<string> links;
    int i = 0;
    while(i < html.length()){
        //--------------------------------------------------
        // Look for opening <a tag
        //--------------------------------------------------
        if(isAnchorTag(html, i)){
            i += 2;
            while(i < html.length() && html[i] != '>'){
                skipWhiteSpace(html, i);
                //--------------------------------------------------
                // Check for href attribute
                //--------------------------------------------------
                if(startsWith(html, i, "href")){
                    int end = i + 4;
                    // href must end with whitespace, = or >
                    if(end < html.length() &&!isWhiteSpace(html[end]) &&html[end] != '=' && html[end] != '>'){
                        i++;
                        continue;
                    }
                    i += 4;
                    skipWhiteSpace(html, i);
                    if(i >= html.length() || html[i] != '='){
                        i++;
                        continue;
                    }
                    i++;
                    skipWhiteSpace(html, i);
                    if(i >= html.length()){
                        break;
                    }
                    string url;
                    //--------------------------------------------------
                    // Double quoted value
                    //--------------------------------------------------

                    if(html[i] == '"'){
                        i++;
                        while(i < html.length() && html[i] != '"'){
                            url += html[i];
                            i++;
                        }
                        if(i < html.length()){
                            i++;
                        }
                    }

                    //--------------------------------------------------
                    // Single quoted value
                    //--------------------------------------------------

                    else if(html[i] == '\''){
                        i++;
                        while(i < html.length() && html[i] != '\''){
                            url += html[i];
                            i++;
                        }
                        if(i < html.length()){
                            i++;
                        }
                    }
                    //--------------------------------------------------
                    // Unquoted value
                    //--------------------------------------------------

                    else
                    {
                        while(i < html.length() && !isWhiteSpace(html[i]) && html[i] != '>')
                        {
                            url += html[i];
                            i++;
                        }
                    }
                    if(!url.empty()){
                        links.push_back(url);
                    }
                }
                //--------------------------------------------------
                // Skip current attribute
                //--------------------------------------------------

                else{
                    while(i < html.length() && !isWhiteSpace(html[i]) && html[i] != '>'){
                        i++;
                    }
                }
            }
        }
        i++;
    }
    return links;
}