#ifndef GRAPH_RECOGNITION_CUBIC_GRAPH_CANONICAL_H
#define GRAPH_RECOGNITION_CUBIC_GRAPH_CANONICAL_H

#include <algorithm>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace detail {

// Enumerate breadth-first labelings of a connected cubic graph. Only the
// root can have three undiscovered neighbours; every later vertex has at
// most two. Sorted adjacency rows reconstruct the abstract graph, and all
// roots / orders of newly discovered neighbours are tried, so the minimum
// code is a complete isomorphism invariant independent of the embedding.
// Prefix pruning avoids most branches; the worst case is exponential.
inline void cubic_graph_canonical_code_dfs(const std::vector<std::vector<int> >& adj,
    int at, std::vector<int>& labels, std::vector<int>& order,
    std::vector<int>& code, std::vector<int>& best) {
    if (at == static_cast<int>(adj.size())) {
        if (best.empty() || code < best) best = code;
        return;
    }
    const int v = order[at];
    std::vector<int> fresh;
    for (int w : adj[v]) if (labels[w] < 0) fresh.push_back(w);
    std::sort(fresh.begin(), fresh.end());
    const std::size_t previous = order.size();
    do {
        for (int w : fresh) {
            labels[w] = static_cast<int>(order.size());
            order.push_back(w);
        }
        std::vector<int> row;
        for (int w : adj[v]) row.push_back(labels[w]);
        std::sort(row.begin(), row.end());
        code.insert(code.end(), row.begin(), row.end());
        if (best.empty() || !std::lexicographical_compare(
                best.begin(), best.begin() + code.size(), code.begin(), code.end()))
            cubic_graph_canonical_code_dfs(adj, at + 1, labels, order, code, best);
        code.resize(code.size() - row.size());
        for (int w : fresh) labels[w] = -1;
        order.resize(previous);
    } while (std::next_permutation(fresh.begin(), fresh.end()));
}

inline std::vector<int> cubic_graph_canonical_code(int n,
    const std::vector<std::pair<int, int> >& edges) {
    std::vector<std::vector<int> > adj(n);
    for (const auto& edge : edges) {
        adj[edge.first - 1].push_back(edge.second - 1);
        adj[edge.second - 1].push_back(edge.first - 1);
    }
    std::vector<int> best, labels(n, -1), order, code;
    for (int root = 0; root < n; ++root) {
        labels[root] = 0;
        order.push_back(root);
        cubic_graph_canonical_code_dfs(adj, 0, labels, order, code, best);
        order.clear();
        labels[root] = -1;
    }
    return best;
}

}  // namespace detail
}  // namespace graph_recognition

#endif
