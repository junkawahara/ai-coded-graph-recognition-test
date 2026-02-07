#ifndef GRAPH_RECOGNITION_THRESHOLD_H
#define GRAPH_RECOGNITION_THRESHOLD_H

#include "graph.h"
#include <vector>

namespace graph_recognition {

// Result of threshold graph recognition.
struct ThresholdResult {
    bool is_threshold;
};

// Check whether a graph is a threshold graph.
// Characterization used:
//   A graph is threshold iff repeatedly removing an isolated or universal
//   vertex eventually deletes all vertices.
inline ThresholdResult check_threshold(const Graph& g) {
    ThresholdResult res;
    res.is_threshold = true;

    int n = g.n;
    std::vector<std::vector<int>> neighbors(n + 1);
    std::vector<int> degree(n + 1, 0);
    std::vector<unsigned char> alive(n + 1, 1);

    for (int v = 1; v <= n; ++v) {
        neighbors[v].reserve(g.adj_set[v].size());
        for (std::unordered_set<int>::const_iterator it = g.adj_set[v].begin();
             it != g.adj_set[v].end(); ++it) {
            neighbors[v].push_back(*it);
        }
        degree[v] = (int)neighbors[v].size();
    }

    int alive_count = n;
    for (int step = 0; step < n; ++step) {
        int pick = 0;
        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] == 0 || degree[v] == alive_count - 1) {
                pick = v;
                break;
            }
        }
        if (pick == 0) {
            res.is_threshold = false;
            return res;
        }

        alive[pick] = 0;
        alive_count--;
        for (size_t i = 0; i < neighbors[pick].size(); ++i) {
            int u = neighbors[pick][i];
            if (alive[u]) degree[u]--;
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
