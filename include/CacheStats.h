#pragma once
#include<bits/stdc++.h>
using namespace std;

struct CacheStats {
    long long hits = 0;
    long long misses = 0;
    long long evictions = 0;

    void recordHit() {hits++;}
    void recordMiss() {misses++;}
    void recordEviction() {evictions++;}

    double hitRatio() const {
        long long total = hits + misses;
        return total == 0 ? 0.0 : (double)hits/total;
    }

    double missRatio() const {
        long long total = hits + misses;
        return total == 0 ? 0.0 : (double)misses/total;
    }

    void print(const string& label) const {
        cout << "\n--" << label << " Stats ---\n" << 
            "Hits: " << hits << "\n" <<
            "Misses: " << misses << "\n" <<
            "Evictions: " << evictions << "\n" <<
            "Hit Ratio:" << hitRatio()*100.0 << "%\n" <<
            "Miss Ratio: " << missRatio()*100.0 << "%\n";
    }
    
};