#ifndef GRAPH_RECOGNITION_DSU_H
#define GRAPH_RECOGNITION_DSU_H

#include <algorithm>
#include <numeric>
#include <vector>

namespace graph_recognition {

// Disjoint Set Union (Union-Find) with path compression and union by rank.
struct DSU {
    std::vector<int> p, r;

    DSU(int n = 0) { init(n); }

    void init(int n) {
        p.resize(n);
        r.assign(n, 0);
        for (int i = 0; i < n; ++i) p[i] = i;
    }

    int find(int x) {
        return p[x] == x ? x : p[x] = find(p[x]);
    }

    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (r[a] < r[b]) std::swap(a, b);
        p[b] = a;
        if (r[a] == r[b]) r[a]++;
        return true;
    }
};

} // namespace graph_recognition

#endif
