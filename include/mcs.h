#ifndef GRAPH_RECOGNITION_MCS_H
#define GRAPH_RECOGNITION_MCS_H

#include "graph.h"
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

// Result of Maximum Cardinality Search.
struct MCSResult {
    std::vector<int> order;   // order[i] = vertex at position i (1-indexed)
    std::vector<int> number;  // number[v] = position of vertex v (1-indexed)
};

// Compute MCS ordering of the graph.
// The resulting order[1..n] is a Perfect Elimination Ordering for chordal graphs:
// order[1] is eliminated first, order[n] last.
// For each vertex v at position number[v], the neighbors with higher position
// numbers (later[v]) form a clique in chordal graphs.
inline MCSResult mcs(const Graph& g) {
    int n = g.n;
    MCSResult res;
    res.order.resize(n + 1, 0);
    res.number.resize(n + 1, 0);

    std::vector<int> label(n + 1, 0), used(n + 1, 0);
    std::priority_queue<std::pair<int, int>> pq;
    for (int v = 1; v <= n; ++v) pq.push(std::make_pair(0, v));

    // Label from n down to 1: the first vertex picked by MCS gets position n,
    // the last gets position 1.  This produces a valid PEO where "later"
    // (higher position) neighbors form cliques.
    for (int i = n; i >= 1; --i) {
        while (!pq.empty()) {
            int v = pq.top().second;
            int l = pq.top().first;
            if (used[v] || l != label[v]) { pq.pop(); continue; }
            pq.pop();
            used[v] = 1;
            res.order[i] = v;
            res.number[v] = i;
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (!used[u]) {
                    label[u]++;
                    pq.push(std::make_pair(label[u], u));
                }
            }
            break;
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
