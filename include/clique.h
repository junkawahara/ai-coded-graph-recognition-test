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
 */
inline MaximalCliques enumerate_maximal_cliques(const Graph& g, const ChordalResult& chordal) {
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

    std::vector<int> sorted_cliques(k);
    for (int j = 0; j < k; ++j) sorted_cliques[j] = j;
    std::sort(sorted_cliques.begin(), sorted_cliques.end(),
              [&](int a, int b) { return clique_min_pos[a] < clique_min_pos[b]; });

    std::vector<int> latest_clique(n + 1, -1);
    std::map<std::pair<int,int>, int> edge_weight;

    for (int si = 0; si < k; ++si) {
        int j = sorted_cliques[si];
        for (size_t t = 0; t < res.mc.cliques[j].size(); ++t) {
            int u = res.mc.cliques[j][t];
            int prev = latest_clique[u];
            if (prev != -1 && prev != j) {
                int a = prev, b = j;
                if (a > b) std::swap(a, b);
                edge_weight[std::make_pair(a, b)]++;
            }
            latest_clique[u] = j;
        }
    }

    struct Edge {
        int w, a, b;
    };
    std::vector<Edge> edges;
    edges.reserve(edge_weight.size());
    for (std::map<std::pair<int,int>, int>::iterator it = edge_weight.begin();
         it != edge_weight.end(); ++it) {
        Edge e;
        e.w = it->second;
        e.a = it->first.first;
        e.b = it->first.second;
        edges.push_back(e);
    }
    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.w > b.w;
    });

    DSU dsu(k);
    for (size_t i = 0; i < edges.size(); ++i) {
        if (dsu.unite(edges[i].a + 1, edges[i].b + 1)) {
            res.tree[edges[i].a].push_back(edges[i].b);
            res.tree[edges[i].b].push_back(edges[i].a);
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief Constructs the clique tree of a chordal graph
 * @param g Input graph
 * @param chordal Result of check_chordal()
 * @param algo Algorithm to use (default: INCREMENTAL)
 * @return CliqueTreeResult
 */
inline CliqueTreeResult build_clique_tree(const Graph& g, const ChordalResult& chordal,
    CliqueTreeAlgorithm algo = CliqueTreeAlgorithm::INCREMENTAL) {
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
