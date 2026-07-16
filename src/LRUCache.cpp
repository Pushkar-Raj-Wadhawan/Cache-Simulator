#include "../include/LRUCache.h"

LRUCache::LRUCache(int capacity) {
    this->capacity = capacity;
    head = new Node(0,0);
    tail = new Node(0,0);
    head->prev = tail;
    tail->next = head;  
}

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
    if(it == cache.end()) {stats.recordMiss(); return -1;}
    stats.recordHit();
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
            stats.recordEviction();
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
