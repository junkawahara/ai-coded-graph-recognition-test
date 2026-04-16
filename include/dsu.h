#ifndef GRAPH_RECOGNITION_DSU_H
#define GRAPH_RECOGNITION_DSU_H

/**
 * @file dsu.h
 * @brief Union-Find (Disjoint Set Union data structure)
 *
 * Disjoint Set Union with path compression and union by rank.
 */

#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief Disjoint Set Union (Union-Find)
 *
 * Implements path compression and union by rank.
 * 1-indexed: elements 1 .. n.
 */
struct DSU {
    std::vector<int> p; /**< Parent array */
    std::vector<int> r; /**< Rank array */

    /**
     * @brief Constructor
     * @param n Number of elements (1-indexed: 1 .. n)
     */
    DSU(int n = 0) { init(n); }

    /**
     * @brief Initialize
     * @param n Number of elements
     */
    void init(int n) {
        p.assign(n + 1, 0);
        r.assign(n + 1, 0);
        for (int i = 1; i <= n; ++i) p[i] = i;
    }

    /**
     * @brief Returns the representative of element x
     * @param x Element
     * @return Representative of x
     */
    int find(int x) {
        return p[x] == x ? x : p[x] = find(p[x]);
    }

    /**
     * @brief Merges elements a and b into the same set
     * @param a Element a
     * @param b Element b
     * @return true if a merge was performed, false if already in the same set
     */
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
