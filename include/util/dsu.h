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
 *
 * Preconditions (not checked): the element count passed to the constructor or
 * to init() is >= 0, and every element handed to find() / unite() lies in
 * 1 .. n for the current n. A negative n makes init() request a vector of
 * size_t(n + 1) elements (std::length_error or std::bad_alloc), and an
 * element outside 1 .. n indexes the parent array out of bounds.
 */
struct DSU {
    std::vector<int> p; /**< Parent array */
    std::vector<int> r; /**< Rank array */

    /**
     * @brief Constructor
     * @param n Number of elements (1-indexed: 1 .. n); must be >= 0
     */
    DSU(int n = 0) { init(n); }

    /**
     * @brief Initialize
     * @param n Number of elements (1-indexed: 1 .. n); must be >= 0
     *
     * Discards the previous contents and makes every element of 1 .. n a
     * singleton.
     */
    void init(int n) {
        p.assign(n + 1, 0);
        r.assign(n + 1, 0);
        for (int i = 1; i <= n; ++i) p[i] = i;
    }

    /**
     * @brief Returns the representative of element x
     * @param x Element; must lie in 1 .. n
     * @return Representative of x
     */
    int find(int x) {
        return p[x] == x ? x : p[x] = find(p[x]);
    }

    /**
     * @brief Merges elements a and b into the same set
     * @param a Element a; must lie in 1 .. n
     * @param b Element b; must lie in 1 .. n
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
