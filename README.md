# Cache Simulator

A configurable C++ cache simulator implementing three eviction policies — **LRU**, **LFU**, and **FIFO** — with O(1) `get`/`put` operations, and an instrumented stats layer for measuring hit ratio, miss ratio, and eviction counts under different access patterns.

---

## Overview

Caches are only useful if lookups and updates stay fast regardless of how full they are. If `get`/`put` degraded to O(n) — say, by scanning a list to find the least-recently-used item — a cache would lose its entire performance advantage over just re-fetching data from the source on every access. This project implements three eviction strategies, each backed by data structures chosen specifically to guarantee **O(1) average-case operations**, and benchmarks them against a realistic, locality-weighted access pattern to quantify how eviction strategy affects cache effectiveness.

---

## Architecture

The project is split into headers (`include/`, declarations only) and source files (`src/`, implementations), following standard C++ separation of interface and implementation — this keeps each policy's public API readable independent of its internal logic, and mirrors how larger real-world C++ codebases are structured.

```
CacheSimulator/
├── include/
│   ├── LRUCache.h
│   ├── LFUCache.h
│   ├── FIFOCache.h
│   └── CacheStats.h
├── src/
│   ├── LRUCache.cpp
│   ├── LFUCache.cpp
│   ├── FIFOCache.cpp
│   └── main.cpp
└── README.md
```

### LRU (Least Recently Used)

**Structures:** `unordered_map<int, Node*>` + a doubly linked list with dummy head/tail sentinel nodes.

The hash map's real job isn't just lookup — it's giving O(1) access to a node's **exact position** inside the linked list, so that on every access the node can be unlinked and re-inserted at the front without traversal. A doubly linked list is required (not singly linked) because removing a node needs direct access to both its `prev` and `next` neighbors — a singly linked list would need an O(n) traversal to find the previous node before it could unlink anything.

Dummy (sentinel) head and tail nodes remove the need for null checks at the boundaries of the list (empty list, single-node list) — every insertion and removal follows the exact same code path regardless of list size, since `head`/`tail` always point to valid nodes, never `nullptr`.

- `head` side = most recently used
- `tail` side = least recently used
- Eviction always removes `tail->next` (or true tail-adjacent node) — the single "coldest" entry

### LFU (Least Frequently Used)

**Structures:** `unordered_map<int, pair<int,int>>` (key → {value, freq}) + `unordered_map<int, list<int>>` (frequency → ordered list of keys) + `unordered_map<int, list<int>::iterator>` (key → its exact position within its frequency bucket) + a tracked `minFreq`.

LFU is structurally harder than LRU because eviction needs **two dimensions** of ordering: lowest frequency first, and least-recently-used as a tiebreaker among equal frequencies. A single list can't represent this — so each frequency gets its own list, and `minFreq` is tracked incrementally (rather than recomputed) to keep eviction O(1): a new key always enters at `minFreq = 1`, and `minFreq` only ever increments when a bucket empties out from underneath it.

The `pos` map exists because `std::list::erase` is O(1) *if you already have an iterator* — without caching that iterator per key, removing a key would require scanning its frequency bucket, which is not O(1).

### FIFO (First In, First Out)

**Structures:** `unordered_map<int,int>` (key → value) + `queue<int>` tracking insertion order.

FIFO only ever needs to insert at one end and evict from the other — it never reorders on access, and never needs to remove from the middle of the ordering. That means a plain `queue` is sufficient; it doesn't need the list+iterator machinery LFU requires, because FIFO's ordering never changes once a key is inserted, regardless of how often it's subsequently read.

### Stats Layer

`CacheStats` is a small struct (`hits`, `misses`, `evictions`, plus ratio calculations) held as a public member inside each cache class — one independent instance per cache object, so multiple caches (or multiple test runs) never share or pollute each other's counters. Each `get`/`put` call records the relevant event directly at the point it occurs, rather than being tracked externally, which keeps instrumentation co-located with the logic it's measuring.

---

## Complexity

| Policy | `get()` | `put()` | Space |
|--------|---------|---------|-------|
| LRU    | O(1) avg | O(1) avg | O(capacity) |
| LFU    | O(1) avg | O(1) avg | O(capacity) |
| FIFO   | O(1) avg | O(1) avg | O(capacity) |

All three are **O(1) average / amortized**, not strict worst-case — `unordered_map` operations are O(1) on average, degrading only under pathological hash collisions (guarded against in practice by STL's automatic rehashing). All linked-list/queue operations are O(1) worst-case, since every policy always has a direct pointer/iterator to the node it needs — no traversal is ever required.

Space is O(capacity) for all three, though the constant factor differs: LRU needs a map + DLL nodes (~2x), LFU needs three maps plus per-frequency lists (~3x+), FIFO needs just a map + queue (~2x). All remain linear in capacity, but LFU trades extra memory overhead for frequency-aware eviction decisions.

---

## Benchmark: Locality-Weighted Access Pattern

To evaluate the three policies under realistic conditions, accesses were generated using an 80/20 distribution — 80% of accesses hit a small set of "hot" keys, 20% hit a large pool of "cold" keys — modeling the real-world principle of *locality of reference* (a small working set accounts for most memory/cache accesses in real programs).

**Setup:** 10,000 operations, 5 hot keys, up to 500 distinct cold keys, cache capacity = 10.

```
--- LRU Stats ---
Hits: 8113
Misses: 1887
Evictions: 1872
Hit Ratio: 81.13%
Miss Ratio: 18.87%

--- LFU Stats ---
Hits: 8124
Misses: 1876
Evictions: 1861
Hit Ratio: 81.24%
Miss Ratio: 18.76%

--- FIFO Stats ---
Hits: 7369
Misses: 2631
Evictions: 2616
Hit Ratio: 73.69%
Miss Ratio: 26.31%
```

**Observations:**
- LRU and LFU perform almost identically here (81.13% vs. 81.24%) — with a small, stable hot-key set and strongly skewed access frequency, recency and frequency end up tracking nearly the same working set, so the two policies converge on this particular pattern.
- FIFO trails both by ~7.5 points, since it evicts purely by arrival order and has no mechanism to recognize or protect frequently-reused keys — a cold key that happens to arrive after a hot key can evict it, even if the hot key is about to be reused.

---

## Known Limitations / Future Extensions

- `capacity == 0` is not explicitly guarded against.
- No thread-safety — not designed for concurrent access.
- No TTL/expiry mechanism for cache entries.
- Access patterns are currently generated in code; a config-driven or CLI-argument-based pattern selector (sequential, hot-key, random, scan-flood) would make the simulator more flexible for comparative benchmarking without recompiling.
- A further experiment worth adding: a "sequential scan flood" pattern (a burst of one-off, never-repeated accesses injected between repeated hot-key accesses) to demonstrate LRU's known real-world weakness — a long scan can evict an entire working set purely due to recency, even though LFU-style frequency tracking would protect it. This is a well-documented cache design trade-off (relevant to real systems like database buffer pools) and would isolate a case where LFU meaningfully outperforms LRU rather than converging with it, as it does in the benchmark above.

---

## Build & Run

```bash
g++ -std=c++17 -o simulator src/main.cpp src/LRUCache.cpp src/LFUCache.cpp src/FIFOCache.cpp -I include
./simulator
```