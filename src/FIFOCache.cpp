#include "..\include\FIFOCache.h"

FIFOCache::FIFOCache(int capacity) {
    this->capacity = capacity;
}

int FIFOCache::get(int key) {
    auto it = cache.find(key);
    if(it == cache.end()) {stats.recordMiss(); return -1;}
    stats.recordHit();
    return it->second;
}

void FIFOCache::put(int key, int value) {
    auto it = cache.find(key);
    if(it != cache.end()) {
        it->second = value;
        return;
    }
    // we need to put this new key
    if(capacity == (int)cache.size()) {
        // we gotta remove fifo first
        stats.recordEviction();
        int FIFOKey = order.front();
        order.pop();
        cache.erase(FIFOKey);
    }
    order.push(key);
    cache[key] = value;
}