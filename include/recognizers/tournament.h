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

    /* Negative n is not a graph; reject it before any arithmetic on n
       (n - 1 overflows for INT_MIN). */
    if (n < 0) return res;
    if (n == 0) {
        /* An empty tournament has no arcs; reject stray arcs for
           consistency with check_digraph(0, ...) */
        res.is_tournament = arcs.empty();
        return res;
    }

    /* Arc count check: m = n(n-1)/2, computed in a type that holds it for
       every int n (n(n-1) < 2^62) */
    const unsigned long long expected =
        static_cast<unsigned long long>(n) * static_cast<unsigned long long>(n - 1) / 2ULL;
    if (static_cast<unsigned long long>(arcs.size()) != expected) return res;

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
 * @brief Reads a directed graph ("n m" then m arcs "u v") from an input stream
 * @param in Input stream
 * @param[out] n Number of vertices (0 when the header could not be read)
 * @param[out] arcs Arc list (on failure, the arcs read so far)
 * @param[out] ok Set to true iff the header is well-formed (n >= 0, m >= 0)
 *             and all m arc lines were read. Arc endpoints are not range
 *             checked here: the directed recognizers (check_digraph,
 *             check_poset, check_tournament) answer NO for an arc outside
 *             [1, n] or a self-loop, so such input is a well-formed
 *             description of a non-member rather than a parse error.
 *
 * Mirrors Graph::read(std::istream&, bool&): the CLIs print
 * "error: malformed graph input" and exit with status 1 when @p ok is false,
 * so empty, garbage or truncated input cannot pass as an (empty) YES.
 */
inline void read_directed(std::istream& in, int& n,
    std::vector<std::pair<int, int>>& arcs, bool& ok) {
    int m;
    n = 0;
    ok = false;
    arcs.clear();
    if (!(in >> n >> m)) { n = 0; return; }
    if (n < 0 || m < 0) { n = 0; return; }
    /* m is untrusted: cap the reserve (cf. Graph::read) */
    const int reserve_cap = 1 << 20;
    arcs.reserve(m < reserve_cap ? m : reserve_cap);
    for (int i = 0; i < m; ++i) {
        int u, v;
        if (!(in >> u >> v)) return;
        arcs.push_back(std::make_pair(u, v));
    }
    ok = true;
}

/**
 * @brief Reads a directed graph without reporting input validity
 *
 * Malformed input yields an empty or truncated arc list indistinguishable
 * from a valid one; use the overload with the @c ok flag to detect it.
 */
inline void read_directed(std::istream& in, int& n,
    std::vector<std::pair<int, int>>& arcs) {
    bool ok;
    read_directed(in, n, arcs, ok);
}

} // namespace graph_recognition

#endif
