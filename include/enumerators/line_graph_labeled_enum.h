#ifndef GRAPH_RECOGNITION_LINE_GRAPH_ENUM_H
#define GRAPH_RECOGNITION_LINE_GRAPH_ENUM_H

/**
 * @file line_graph_labeled_enum.h
 * @brief Enumeration of line graphs (reverse search)
 *
 * Enumerates all labeled line graphs.
 *
 * parent(G) = remove the vertex with the largest label from G
 * (Line graph is characterized by Beineke's 9 forbidden induced subgraphs;
 *  always valid since it is a hereditary class)
 *
 * Vertices are added in order 1, 2, ..., n, and at each step all possible
 * neighborhood subsets are enumerated and filtered by line graph recognition.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "recognizers/line_graph.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for line graph enumeration
 */
enum class LineGraphLabeledEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of line graph enumeration
 */
struct LineGraphLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated line graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct LineGraphLabeledEnumState {
    int total_n;
    int alive_count;  /**< Alive vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit LineGraphLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief DFS for line graph reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood.line graph children are pruned.
 */
inline void line_graph_labeled_enum_dfs(LineGraphLabeledEnumState& state,
                                std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    graph.edges.push_back(std::make_pair(u, v));
        out->push_back(graph);
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    // L10: silently stops enumeration; 64+ vertices would overflow 2^k
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    // M23: pre-extract base edges (between vertices 1..k, excluding x)
    std::vector<std::pair<int, int>> base_edges;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                base_edges.push_back(std::make_pair(u, v));

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
        }
        state.alive_count = x;

        std::vector<std::pair<int, int>> edges = base_edges;
        for (int u = 1; u <= k; ++u)
            if (state.adj[x][u])
                edges.push_back(std::make_pair(u, x));
        Graph g(x, edges);
        LineGraphResult res = check_line_graph(g);

        if (res.is_line_graph) {
            line_graph_labeled_enum_dfs(state, out);
        }

        for (int u = 1; u <= k; ++u) {
            state.adj[x][u] = 0;
            state.adj[u][x] = 0;
        }
        state.alive_count = k;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled Line graph on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return LineGraphLabeledEnumerationResult
 *
 * Uses reverse search. parent(G) is obtained by removing the vertex
 * with the largest label from G.line graph is a hereditary class,
 * so the property is preserved under any vertex removal.
 */
inline LineGraphLabeledEnumerationResult
enumerate_line_graphs_reverse_search(int n,
    LineGraphLabeledEnumAlgorithm algo =
        LineGraphLabeledEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    LineGraphLabeledEnumerationResult result;
    if (n < 0) return result;
    detail::LineGraphLabeledEnumState root(n);
    detail::line_graph_labeled_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
