#ifndef GRAPH_RECOGNITION_CLIQUE_H
#define GRAPH_RECOGNITION_CLIQUE_H

#include "chordal.h"
#include "dsu.h"
#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

// Maximal cliques enumerated in PEO order.
struct MaximalCliques {
    std::vector<std::vector<int>> cliques;  // cliques[i] = vertex set of i-th clique
    std::vector<std::vector<int>> member;   // member[v] = clique indices containing v
};

// Enumerate maximal cliques of a chordal graph in PEO order.
// Precondition: the graph must be chordal (chordal.is_chordal == true).
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

// Clique tree of a chordal graph.
struct CliqueTreeResult {
    MaximalCliques mc;                      // maximal cliques and membership
    std::vector<std::vector<int>> tree;     // clique tree adjacency list
};

// Build a clique tree (junction tree) via maximum spanning forest.
// Note: when multiple MSTs exist (weight ties), the chosen tree is arbitrary.
// For algorithms that require a specific tree topology (e.g. clique path),
// use a dedicated method instead.
inline CliqueTreeResult build_clique_tree(const Graph& g, const ChordalResult& chordal) {
    CliqueTreeResult res;
    res.mc = enumerate_maximal_cliques(g, chordal);
    int n = g.n;
    int k = (int)res.mc.cliques.size();

    // Intersection sizes between cliques.
    std::vector<std::vector<int>> w(k, std::vector<int>(k, 0));
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = res.mc.member[v];
        for (size_t i = 0; i < cl.size(); ++i) {
            for (size_t j = i + 1; j < cl.size(); ++j) {
                int a = cl[i], b = cl[j];
                if (a > b) std::swap(a, b);
                w[a][b]++;
            }
        }
    }

    // Build maximum spanning forest of the clique intersection graph.
    struct Edge {
        int w, a, b;
    };
    std::vector<Edge> edges;
    edges.reserve(k * (k - 1) / 2);
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            if (w[i][j] > 0) {
                Edge e;
                e.w = w[i][j]; e.a = i; e.b = j;
                edges.push_back(e);
            }
        }
    }
    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return a.w > b.w;
    });
    DSU dsu(k);
    res.tree.resize(k);
    for (size_t i = 0; i < edges.size(); ++i) {
        if (dsu.unite(edges[i].a, edges[i].b)) {
            res.tree[edges[i].a].push_back(edges[i].b);
            res.tree[edges[i].b].push_back(edges[i].a);
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
