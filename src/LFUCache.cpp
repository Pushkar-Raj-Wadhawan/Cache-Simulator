#include<bits/stdc++.h>
using namespace std;

struct Node {
    int key, val;
    Node* prev;
    Node* next;
};

class LFUCache {
    int capacity;
    int minFreq;
    unordered_map<int, pair<int, int>> cache; // key -> value, freq
    unordered_map<int, list<int>> freqList;
    unordered_map<int, list<int>::iterator> pos;

    void touch(int key); // increment frequency, and handling list migration + minFreq update

public:
    LFUCache(int capacity) {
        this->capacity = capacity;
        minFreq = 0;
        int get(int key);
        void put(int key);
    }
};

