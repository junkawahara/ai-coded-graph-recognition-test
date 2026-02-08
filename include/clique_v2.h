#ifndef GRAPH_RECOGNITION_CLIQUE_V2_H
#define GRAPH_RECOGNITION_CLIQUE_V2_H

#include "chordal.h"
#include "clique.h"
#include "graph.h"
#include <algorithm>
#include <map>
#include <vector>

namespace graph_recognition {

// Build a clique tree using PEO structure in O(n + sum|C_i|) time.
// Instead of computing all-pairs intersection weights in O(k^2),
// we track the latest clique per vertex and accumulate edge weights
// incrementally as we process cliques in PEO order.
inline CliqueTreeResult build_clique_tree_v2(const Graph& g, const ChordalResult& chordal) {
    CliqueTreeResult res;
    res.mc = enumerate_maximal_cliques(g, chordal);
    int n = g.n;
    int k = (int)res.mc.cliques.size();
    res.tree.resize(k);

    if (k <= 1) return res;

    const std::vector<int>& number = chordal.mcs_result.number;

    // For each vertex, find which maximal clique it first appears in
    // (the one with the smallest PEO-position representative).
    // We process cliques sorted by the minimum PEO position of their members.

    // Step 1: Determine the "representative" (min PEO position vertex) of each clique
    // and sort cliques by that position.
    std::vector<int> clique_min_pos(k);
    for (int j = 0; j < k; ++j) {
        int mn = n + 1;
        for (size_t t = 0; t < res.mc.cliques[j].size(); ++t) {
            int pos = number[res.mc.cliques[j][t]];
            if (pos < mn) mn = pos;
        }
        clique_min_pos[j] = mn;
    }

    // Sort clique indices by their representative's PEO position.
    std::vector<int> sorted_cliques(k);
    for (int j = 0; j < k; ++j) sorted_cliques[j] = j;
    std::sort(sorted_cliques.begin(), sorted_cliques.end(),
              [&](int a, int b) { return clique_min_pos[a] < clique_min_pos[b]; });

    // Step 2: Process cliques in PEO order.
    // For each vertex, track the latest clique containing it.
    // When a vertex appears in a new clique, it creates a candidate tree edge
    // between its previous clique and the new clique.
    std::vector<int> latest_clique(n + 1, -1);

    // Accumulate weights for candidate edges using a map.
    // Key: (min_clique_idx, max_clique_idx), Value: intersection size.
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

    // Step 3: Build MST from candidate edges using Kruskal's algorithm.
    // Sort edges by weight descending.
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
        if (dsu.unite(edges[i].a, edges[i].b)) {
            res.tree[edges[i].a].push_back(edges[i].b);
            res.tree[edges[i].b].push_back(edges[i].a);
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
