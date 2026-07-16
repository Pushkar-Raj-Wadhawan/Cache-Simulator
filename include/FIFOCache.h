#pragma once
#include<bits/stdc++.h>
#include "CacheStats.h"
using namespace std;

CacheStats stats;

class FIFOCache {
private:
    int capacity;
    unordered_map<int, int> cache; // key->val
    queue<int> order;
public:
    CacheStats stats;
    FIFOCache(int capacity);  
    int get(int key);
    void put(int key, int value);
};