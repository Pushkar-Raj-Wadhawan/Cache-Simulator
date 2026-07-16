#pragma once
#include<bits/stdc++.h>
#include "CacheStats.h"
using namespace std;

CacheStats stats;

struct Node {
    int key, val;
    Node* next;
    Node* prev;
    Node(int key, int val) : key(key), val(val), next(NULL), prev(NULL) {}
};

class LRUCache {
private:
    int capacity;
    unordered_map<int, Node*> cache; // key -> corresponding node in the dll
    Node* head; // dummy head (MRU)
    Node* tail; // dummy tail (LRU)

    void addToFront(Node* node);
    void removeNode(Node* node);
    void moveToFront(Node* node);

public:
    LRUCache(int capacity);
    int get(int key);
    void put(int key, int value);
    
};