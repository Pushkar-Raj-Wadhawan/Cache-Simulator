#include<bits/stdc++.h>
using namespace std;

struct Node {
    int key, val;
    Node* next;
    Node* prev;
    Node(int key, int val)
        : key(key), val(val), next(NULL), prev(NULL) {}
};

class LRUCache {
private:
    int capacity;
    unordered_map<int, Node*> cache; // key->node pointer
    Node* head; // dummy head (mru)
    Node* tail; // dummy tail (lru)


    void addToFront(Node* node);
    void removeNode(Node* node);
    void moveToFront(Node* node);

public:
    LRUCache(int capacity) {
        this->capacity = capacity;
        head = new Node(0,0);
        tail = new Node(0,0);
        head->prev = tail;
        tail->next = head;  
    }
    int get(int key);
    void put(int key, int value);
};

void LRUCache::addToFront(Node* node){ 
    head->prev->next = node;
    node->next = head;
    node->prev = head->prev;
    head->prev = node;
}

void LRUCache::removeNode(Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void LRUCache::moveToFront(Node* node) {
    removeNode(node);
    addToFront(node);
}

int LRUCache::get(int key) {
    auto it = cache.find(key);
    if(it == cache.end()) return -1;
    Node* node = it->second;
    moveToFront(node);
    return node->val;
}

void LRUCache::put(int key, int value) {
    auto it = cache.find(key); // finding node to corresponding key
    if(it == cache.end()) {
        // adding a new node, checking capacity constraints
        if((int)cache.size() == capacity) {
            // we gotta remove LRU Cache
            Node* LRUNode = tail->next;
            cache.erase(LRUNode->key);
            removeNode(LRUNode);
            delete LRUNode;
        }
        Node* node = new Node(key, value);
        cache[key] = node;
        addToFront(node);
    }
    else {
        // updating current node's val
        Node* node = it->second;
        moveToFront(node);
        node->val = value;
    }
}
