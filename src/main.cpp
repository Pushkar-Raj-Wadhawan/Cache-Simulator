#include<bits/stdc++.h>
#include "..\include\LRUCache.h"
#include "..\include\LFUCache.h"
#include "..\include\FIFOCache.h"

using namespace std;


vector<int> generateAccessPattern(int totalAccesses, int hotKeyRange, int coldKeyRange) {
    vector<int> pattern;
    random_device rd;
    mt19937 gen(rd());
    
    // 80% of accesses hit a small "hot" set of keys, 20% hit a large "cold" range
    // this mimics real-world locality of reference
    uniform_int_distribution<int> hotDist(1, hotKeyRange);       // e.g., keys 1-5
    uniform_int_distribution<int> coldDist(hotKeyRange+1, coldKeyRange); // e.g., keys 6-200
    uniform_int_distribution<int> chooseDist(1, 100);

    for (int i = 0; i < totalAccesses; i++) {
        if (chooseDist(gen) <= 80) pattern.push_back(hotDist(gen));
        else pattern.push_back(coldDist(gen));
    }
    return pattern;
}

int main() {
    int capacity = 15;

    // simulated access pattern: hot keys (1,2) + cold one-off keys (3,4,5,6,7)
    vector<int> accessPattern = generateAccessPattern(10000, 5, 500);

    LRUCache lru(capacity);
    LFUCache lfu(capacity);
    FIFOCache fifo(capacity);

    for(int key: accessPattern) {
        if(lru.get(key) == -1) lru.put(key, key*100);
        if(lfu.get(key) == -1) lfu.put(key, key*100);
        if(fifo.get(key) == -1) fifo.put(key, key*100);
    }

        lru.stats.print("LRU");
        lfu.stats.print("LFU");
        fifo.stats.print("FIFO");

}