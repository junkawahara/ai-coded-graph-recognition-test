#ifndef GRAPH_RECOGNITION_BIPARTITE_H
#define GRAPH_RECOGNITION_BIPARTITE_H

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

// Result of bipartite graph recognition.
struct BipartiteResult {
    bool is_bipartite;
    // color[v] is 0 or 1 for each vertex when is_bipartite == true.
    std::vector<int> color;
};

// Check whether a graph is bipartite using BFS 2-coloring.
inline BipartiteResult check_bipartite(const Graph& g) {
    BipartiteResult res;
    res.is_bipartite = true;
    res.color.assign(g.n + 1, -1);

    std::queue<int> q;
    for (int s = 1; s <= g.n; ++s) {
        if (res.color[s] != -1) continue;
        res.color[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (res.color[u] == -1) {
                    res.color[u] = 1 - res.color[v];
                    q.push(u);
                    continue;
                }
                if (res.color[u] == res.color[v]) {
                    res.is_bipartite = false;
                    return res;
                }
            }
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
