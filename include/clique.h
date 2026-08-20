#ifndef GRAPH_RECOGNITION_CLIQUE_H
#define GRAPH_RECOGNITION_CLIQUE_H

/**
 * @file clique.h
 * @brief Maximal clique enumeration and clique tree construction
 *
 * Enumerates maximal cliques using the PEO of a chordal graph
 * and constructs the clique tree.
 *
 * Algorithms:
 *   - KRUSKAL: clique tree construction via maximum weight spanning tree
 *   - INCREMENTAL: incremental construction in PEO order (default)
 */

#include "chordal.h"
#include "dsu.h"
#include "graph.h"
#include <algorithm>
#include <map>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for clique tree construction
 */
enum class CliqueTreeAlgorithm {
    KRUSKAL,     /**< construction via maximum weight spanning tree */
    INCREMENTAL  /**< incremental construction in PEO order (default) */
};

/**
 * @brief Maximal cliques enumerated in PEO order
 */
struct MaximalCliques {
    std::vector<std::vector<int>> cliques;  /**< cliques[i] = vertex set of the i-th clique */
    std::vector<std::vector<int>> member;   /**< member[v] = list of clique indices containing vertex v */
};

/**
 * @brief Enumerates maximal cliques of a chordal graph in PEO order
 *
 * Precondition: chordal.is_chordal must be true. On a non-chordal input the
 * PEO-based sweep is meaningless, so an empty result is returned instead.
 */
inline MaximalCliques enumerate_maximal_cliques(const Graph& g, const ChordalResult& chordal) {
    if (!chordal.is_chordal) return MaximalCliques();
    int n = g.n;
    const std::vector<int>& order = chordal.mcs_result.order;
    const std::vector<std::vector<int>>& later = chordal.later;

    MaximalCliques res;

    std::vector<int> mark(n + 1, 0);
    int stamp = 1;
    bool has_last = false;
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        std::vector<int> cv;
        cv.reserve(later[v].size() + 1);
        cv.push_back(v);
        for (size_t j = 0; j < later[v].size(); ++j) {
            cv.push_back(later[v][j]);
        }
        if (has_last) {
            bool subset = true;
            for (size_t j = 0; j < cv.size(); ++j) {
                if (mark[cv[j]] != stamp) { subset = false; break; }
            }
            if (subset) continue;
        }
        res.cliques.push_back(cv);
        stamp++;
        for (size_t j = 0; j < cv.size(); ++j) mark[cv[j]] = stamp;
        has_last = true;
    }

    int k = (int)res.cliques.size();
    res.member.resize(n + 1);
    for (int i = 0; i < k; ++i) {
        for (size_t j = 0; j < res.cliques[i].size(); ++j) {
            res.member[res.cliques[i][j]].push_back(i);
        }
    }

    return res;
}

/**
 * @brief Clique tree (junction tree) result
 */
struct CliqueTreeResult {
    MaximalCliques mc;                      /**< maximal cliques and membership */
    std::vector<std::vector<int>> tree;     /**< adjacency list of the clique tree */
};

namespace detail {

/**
 * @brief Clique tree construction via Kruskal's algorithm
 */
inline CliqueTreeResult build_clique_tree_kruskal(const Graph& g, const ChordalResult& chordal) {
    CliqueTreeResult res;
    res.mc = enumerate_maximal_cliques(g, chordal);
    int n = g.n;
    int k = (int)res.mc.cliques.size();

    std::map<std::pair<int,int>, int> w;
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = res.mc.member[v];
        for (size_t i = 0; i < cl.size(); ++i) {
            for (size_t j = i + 1; j < cl.size(); ++j) {
                int a = cl[i], b = cl[j];
                if (a > b) std::swap(a, b);
                w[std::make_pair(a, b)]++;
            }
        }
    }

    struct Edge {
        int w, a, b;
    };
    std::vector<Edge> edges;
    edges.reserve(w.size());
    for (std::map<std::pair<int,int>, int>::iterator it = w.begin();
         it != w.end(); ++it) {
        Edge e;
        e.w = it->second; e.a = it->first.first; e.b = it->first.second;
        edges.push_back(e);
    }
    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.w > b.w;
    });
    DSU dsu(k);
    res.tree.resize(k);
    for (size_t i = 0; i < edges.size(); ++i) {
        if (dsu.unite(edges[i].a + 1, edges[i].b + 1)) {
            res.tree[edges[i].a].push_back(edges[i].b);
            res.tree[edges[i].b].push_back(edges[i].a);
        }
    }

    return res;
}

/**
 * @brief Clique tree construction via incremental method in PEO order
 */
inline CliqueTreeResult build_clique_tree_incremental(const Graph& g, const ChordalResult& chordal) {
    CliqueTreeResult res;
    res.mc = enumerate_maximal_cliques(g, chordal);
    int n = g.n;
    int k = (int)res.mc.cliques.size();
    res.tree.resize(k);

    if (k <= 1) return res;

    const std::vector<int>& number = chordal.mcs_result.number;

    std::vector<int> clique_min_pos(k);
    for (int j = 0; j < k; ++j) {
        int mn = n + 1;
        for (size_t t = 0; t < res.mc.cliques[j].size(); ++t) {
            int pos = number[res.mc.cliques[j][t]];
            if (pos < mn) mn = pos;
        }
        clique_min_pos[j] = mn;
    }

    /* Process cliques in descending order of their representative (the
       clique's minimum MCS number): this is the order in which MCS
       discovers them (a perfect sequence, Blair & Peyton 1993). In that
       order the running intersection property holds: each clique's
       intersection with the union of the earlier cliques is contained in
       a single earlier clique, and attaching to any such clique yields a
       valid clique tree. The previous weight-splitting heuristic based on
       latest_clique produced trees violating the clique-intersection
       property. */
    std::vector<int> sorted_cliques(k);
    for (int j = 0; j < k; ++j) sorted_cliques[j] = j;
    std::sort(sorted_cliques.begin(), sorted_cliques.end(),
              [&](int a, int b) { return clique_min_pos[a] > clique_min_pos[b]; });

    std::vector<char> seen(n + 1, 0);     /* vertex covered by earlier cliques */
    std::vector<char> processed(k, 0);
    std::vector<char> sep_mark(n + 1, 0); /* scratch: marks the current separator */

    for (int si = 0; si < k; ++si) {
        int j = sorted_cliques[si];
        const std::vector<int>& cj = res.mc.cliques[j];

        /* Separator = C_j (cap) union of earlier cliques */
        std::vector<int> sep;
        for (size_t t = 0; t < cj.size(); ++t) {
            if (seen[cj[t]]) sep.push_back(cj[t]);
        }

        if (!sep.empty()) {
            /* Attach to an earlier clique containing the whole separator.
               Candidates: cliques containing sep[0]. Membership is tested by
               marking the separator once and counting marked vertices per
               candidate; a candidate contains the separator iff the count
               reaches |sep| (cliques have no duplicate vertices). This keeps
               the scratch memory O(n) instead of a k x (n+1) matrix. */
            for (size_t t = 0; t < sep.size(); ++t) sep_mark[sep[t]] = 1;
            int parent = -1;
            const std::vector<int>& cand = res.mc.member[sep[0]];
            for (size_t c = 0; c < cand.size() && parent == -1; ++c) {
                int i = cand[c];
                if (!processed[i]) continue;
                const std::vector<int>& ci = res.mc.cliques[i];
                size_t hit = 0;
                for (size_t t = 0; t < ci.size(); ++t) {
                    if (sep_mark[ci[t]]) ++hit;
                }
                if (hit == sep.size()) parent = i;
            }
            for (size_t t = 0; t < sep.size(); ++t) sep_mark[sep[t]] = 0;
            if (parent >= 0) {
                res.tree[j].push_back(parent);
                res.tree[parent].push_back(j);
            }
        }

        for (size_t t = 0; t < cj.size(); ++t) seen[cj[t]] = 1;
        processed[j] = 1;
    }

    return res;
}

} // namespace detail

/**
 * @brief Constructs the clique tree of a chordal graph
 * @param g Input graph
 * @param chordal Result of check_chordal(); chordal.is_chordal must be true
 * @param algo Algorithm to use (default: INCREMENTAL)
 * @return CliqueTreeResult (empty if chordal.is_chordal is false, since a
 *         clique tree only exists for chordal graphs)
 */
inline CliqueTreeResult build_clique_tree(const Graph& g, const ChordalResult& chordal,
    CliqueTreeAlgorithm algo = CliqueTreeAlgorithm::INCREMENTAL) {
    if (!chordal.is_chordal) return CliqueTreeResult();
    switch (algo) {
        case CliqueTreeAlgorithm::KRUSKAL:
            return detail::build_clique_tree_kruskal(g, chordal);
        case CliqueTreeAlgorithm::INCREMENTAL:
            return detail::build_clique_tree_incremental(g, chordal);
        default:
            break;
    }
    return CliqueTreeResult();
}

} // namespace graph_recognition

#endif
