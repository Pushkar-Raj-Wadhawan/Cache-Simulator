#pragma once
#include<bits/stdc++.h>
#include "CacheStats.h"
using namespace std;

CacheStats stats;

class LFUCache {
    int capacity;
    int minFreq;
    unordered_map<int, pair<int, int>> cache; // key -> value, freq
    unordered_map<int, list<int>> freqList; // front->MRU, back->LRU
    unordered_map<int, list<int>::iterator> pos;

    void touch(int key); // increment frequency, and handling list migration + minFreq update

public:
    LFUCache(int capacity);
    int get(int key);
    void put(int key, int value);
};
