#ifndef GRAPH_RECOGNITION_SELF_COMPLEMENTARY_H
#define GRAPH_RECOGNITION_SELF_COMPLEMENTARY_H

/**
 * @file self_complementary.h
 * @brief Self-complementary graph recognition
 *
 * Determines whether G is isomorphic to complement(G).
 * Necessary condition: n must be congruent to 0 or 1 (mod 4).
 * Checks isomorphism with the complement graph using backtracking.
 */

#include "graph.h"

#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for self-complementary graph recognition
 */
enum class SelfComplementaryAlgorithm {
    ISOMORPHISM_CHECK /**< Isomorphism check with complement graph */
};

/**
 * @brief Result of self-complementary graph recognition
 */
struct SelfComplementaryResult {
    bool is_self_complementary = false; /**< true if the graph is self-complementary */
};

namespace detail {

/**
 * @brief Graph isomorphism check via backtracking
 *
 * Searches for an isomorphism mapping from g1 to g2.
 * Includes pruning by vertex degree sequence.
 */
inline bool check_isomorphism_bt(const Graph& g1, const Graph& g2) {
    int n = g1.n;
    if (n != g2.n) return false;
    if (n == 0) return true;

    /* Degree sequence check */
    std::vector<int> deg1(n), deg2(n);
    for (int v = 1; v <= n; ++v) {
        deg1[v - 1] = (int)g1.adj[v].size();
        deg2[v - 1] = (int)g2.adj[v].size();
    }
    std::vector<int> sd1(deg1), sd2(deg2);
    std::sort(sd1.begin(), sd1.end());
    std::sort(sd2.begin(), sd2.end());
    if (sd1 != sd2) return false;

    /* Group by degree and narrow candidates */
    /* perm[i] = which vertex in g2 vertex (i+1) of g1 maps to */
    std::vector<int> perm(n + 1, 0);
    std::vector<char> used(n + 1, 0);

    /* Candidate list for each vertex (vertices with same degree) */
    std::vector<std::vector<int>> candidates(n + 1);
    for (int v = 1; v <= n; ++v) {
        for (int u = 1; u <= n; ++u) {
            if (deg2[u - 1] == deg1[v - 1]) {
                candidates[v].push_back(u);
            }
        }
    }

    /* Sort vertices by increasing number of candidates (most constrained first) */
    std::vector<int> order(n);
    for (int i = 0; i < n; ++i) order[i] = i + 1;
    std::sort(order.begin(), order.end(),
        [&candidates](int a, int b) {
            return candidates[a].size() < candidates[b].size();
        });

    /* Backtracking */
    struct State {
        const Graph& g1;
        const Graph& g2;
        std::vector<int>& perm;
        std::vector<char>& used;
        std::vector<std::vector<int>>& candidates;
        std::vector<int>& order;
        int n;

        bool solve(int depth) {
            if (depth == n) return true;
            int v = order[depth]; /* vertex v of g1 */
            const std::vector<int>& cands = candidates[v];
            for (size_t ci = 0; ci < cands.size(); ++ci) {
                int u = cands[ci]; /* try mapping to vertex u of g2 */
                if (used[u]) continue;

                /* Edge consistency check with already assigned vertices */
                bool ok = true;
                for (int d = 0; d < depth && ok; ++d) {
                    int v2 = order[d];
                    int u2 = perm[v2];
                    bool e1 = g1.has_edge(v, v2);
                    bool e2 = g2.has_edge(u, u2);
                    if (e1 != e2) ok = false;
                }
                if (!ok) continue;

                perm[v] = u;
                used[u] = 1;
                if (solve(depth + 1)) return true;
                used[u] = 0;
                perm[v] = 0;
            }
            return false;
        }
    };

    State state = {g1, g2, perm, used, candidates, order, n};
    return state.solve(0);
}

} // namespace detail

/**
 * @brief Determines whether the graph is self-complementary
 * @param g Input graph
 * @param algo Algorithm to use (default: ISOMORPHISM_CHECK)
 * @return SelfComplementaryResult
 *
 * Self-complementary <=> G is isomorphic to complement(G).
 * Necessary conditions: n = 0 or 1 (mod 4), m = n(n-1)/4.
 */
inline SelfComplementaryResult check_self_complementary(const Graph& g,
    SelfComplementaryAlgorithm algo = SelfComplementaryAlgorithm::ISOMORPHISM_CHECK) {
    (void)algo;
    SelfComplementaryResult res;

    int n = g.n;
    if (n == 0) {
        res.is_self_complementary = true;
        return res;
    }
    if (n == 1) {
        res.is_self_complementary = true;
        return res;
    }

    /* n must be 0 or 1 (mod 4) */
    if (n % 4 != 0 && n % 4 != 1) return res;

    /* Edge count check: m = n(n-1)/4 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != (long long)n * (n - 1) / 4) return res;

    /* Build complement graph */
    std::vector<std::pair<int, int>> comp_edges;
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (!g.has_edge(u, v)) {
                comp_edges.push_back(std::make_pair(u, v));
            }
        }
    }
    Graph comp(n, comp_edges);

    /* Isomorphism check between G and complement(G) */
    if (detail::check_isomorphism_bt(g, comp)) {
        res.is_self_complementary = true;
    }
    return res;
}

} // namespace graph_recognition

#endif
