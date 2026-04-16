#ifndef GRAPH_RECOGNITION_TOURNAMENT_H
#define GRAPH_RECOGNITION_TOURNAMENT_H

/**
 * @file tournament.h
 * @brief Tournament recognition
 *
 * A tournament is a complete directed graph: for every vertex pair {u, v},
 * exactly one arc u->v or v->u exists.
 *
 * Input format: n m (number of vertices, number of arcs) followed by m arcs u v (u->v)
 */

#include <iostream>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for tournament recognition
 */
enum class TournamentAlgorithm {
    ARC_CHECK /**< Arc check for all vertex pairs */
};

/**
 * @brief Result of tournament recognition
 */
struct TournamentResult {
    bool is_tournament = false; /**< true if the graph is a tournament */
};

/**
 * @brief Determines whether the arc list of a directed graph is a tournament
 * @param n Number of vertices
 * @param arcs Arc list (u, v) = u→v
 * @param algo Algorithm to use
 * @return TournamentResult
 *
 * Tournament <=> m = n(n-1)/2 and exactly one arc for each vertex pair.
 */
inline TournamentResult check_tournament(int n,
    const std::vector<std::pair<int, int>>& arcs,
    TournamentAlgorithm algo = TournamentAlgorithm::ARC_CHECK) {
    (void)algo;
    TournamentResult res;

    if (n == 0) {
        res.is_tournament = true;
        return res;
    }

    /* Arc count check: m = n(n-1)/2 */
    long long expected = (long long)n * (n - 1) / 2;
    if ((long long)arcs.size() != expected) return res;

    /* Record arcs in adjacency matrix */
    std::vector<std::vector<char>> has_arc(n + 1, std::vector<char>(n + 1, 0));
    for (size_t i = 0; i < arcs.size(); ++i) {
        int u = arcs[i].first, v = arcs[i].second;
        if (u < 1 || u > n || v < 1 || v > n) return res;
        if (u == v) return res; /* No self-loops allowed */
        if (has_arc[u][v]) return res; /* No duplicate arcs allowed */
        has_arc[u][v] = 1;
    }

    /* Verify all vertex pairs: exactly one directional arc exists */
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            int cnt = has_arc[u][v] + has_arc[v][u];
            if (cnt != 1) return res;
        }
    }

    res.is_tournament = true;
    return res;
}

/**
 * @brief Reads tournament input from standard input
 * @param in Input stream
 * @param[out] n Number of vertices
 * @param[out] arcs Arc list
 */
inline void read_directed(std::istream& in, int& n,
    std::vector<std::pair<int, int>>& arcs) {
    int m;
    n = 0;
    arcs.clear();
    if (!(in >> n >> m)) return;
    if (n < 0 || m < 0) { n = 0; return; }
    arcs.reserve(m);
    for (int i = 0; i < m; ++i) {
        int u, v;
        if (!(in >> u >> v)) break;
        arcs.push_back(std::make_pair(u, v));
    }
}

} // namespace graph_recognition

#endif
