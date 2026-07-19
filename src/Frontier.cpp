#include<iostream>
#include "../include/Frontier.h"
#include<string>
using namespace std;
URLDepth::URLDepth(const string& url, int depth):url(url),depth(depth){}
bool URLDepth::operator==(const URLDepth& other){
    return url==other.url;
}
void Frontier::push(const URLDepth& item){
    queue.enqueue(item);
}
URLDepth Frontier::pop(){
    return queue.dequeue();
}
bool Frontier::empty(){
    return queue.isEmpty();
}
const URLDepth& Frontier::front(){
    return queue.peek();
}
int Frontier::size(){
    return queue.size();
}