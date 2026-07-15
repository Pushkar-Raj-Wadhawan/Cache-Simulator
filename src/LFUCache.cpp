#include<bits/stdc++.h>
using namespace std;

class LFUCache {
    int capacity;
    int minFreq;
    unordered_map<int, pair<int, int>> cache; // key -> value, freq
    unordered_map<int, list<int>> freqList; // front->MRU, back->LRU
    unordered_map<int, list<int>::iterator> pos;

    void touch(int key); // increment frequency, and handling list migration + minFreq update

public:
    LFUCache(int capacity) {
        this->capacity = capacity;
        minFreq = 0;
    }
    int get(int key);
    void put(int key, int value);
};

void LFUCache::touch(int key) {
    auto it = cache.find(key);
    int freq = it->second.second;
    freqList[freq].erase(pos[key]);
    if(minFreq == freq && freqList[freq].size() == 0) minFreq++;
    freq++;
    freqList[freq].push_front(key);
    pos[key] = freqList[freq].begin();
    cache[key].second = freq;
}

int LFUCache::get(int key) {
    auto it = cache.find(key);
    if(it == cache.end()) return -1;
    touch(key);
    return it->second.first;
}

void LFUCache::put(int key, int value) {
    auto it = cache.find(key);
    if(it == cache.end()) {
        // we need to make this after ensuring space for it
        if(cache.size() == capacity) {
            int LFUVal = freqList[minFreq].back();
            freqList[minFreq].pop_back();
            cache.erase(LFUVal);
            pos.erase(LFUVal);
        }
        cache[key] = {value, 1};
        freqList[1].push_front(key);
        pos[key] = freqList[1].begin();
        minFreq = 1;
    }
    else {
        touch(key);
        cache[key].first = value;
    }
} 
