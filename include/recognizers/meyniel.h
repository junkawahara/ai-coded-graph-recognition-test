#ifndef GRAPH_RECOGNITION_MEYNIEL_H
#define GRAPH_RECOGNITION_MEYNIEL_H

/**
 * @file meyniel.h
 * @brief Meyniel graph recognition
 *
 * A Meyniel graph is a graph where every odd cycle of length 5 or more has at least
 * 2 chords.
 *
 * A superclass of chordal graphs and a subclass of perfect graphs.
 *
 * Algorithms:
 *   - DIRECT_CHECK: for each edge (u,v), enumerates simple paths from u to v
 *     via DFS backtracking, and detects obstructions (odd cycles of length >= 5
 *     with <= 1 chord).
 *
 * References:
 *   - Meyniel, "On the perfect graph conjecture," Discrete Math., 1976
 *   - Burlet, Fonlupt, "Polynomial algorithm to recognize a Meyniel
 *     graph," Annals of Discrete Math., 1984
 *   - Roussel, Rusu, "An O(n^2) algorithm to color Meyniel graphs,"
 *     Discrete Math. 235, 2001
 */

#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "certificates/obstruction_extract.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for Meyniel graph recognition
 */
enum class MeynielAlgorithm {
    DIRECT_CHECK /**< Direct definition check (DFS backtracking) */
};

/**
 * @brief Result of Meyniel graph recognition
 */
struct MeynielResult {
    bool is_meyniel = false; /**< true if the graph is a Meyniel graph */
    Obstruction obstruction; /**< NO certificate: an ODD_CYCLE_LE1_CHORD, the odd
                                  cycle of length >= 5 with at most one chord that
                                  Meyniel graphs forbid. Valid only when
                                  is_meyniel == false */
};

namespace detail_meyniel {

/**
 * @brief DFS for Meyniel obstruction search
 *
 * Searches for simple paths from start_u to target_v,
 * and when a path of even length >= 4 is found (forming an odd cycle >= 5),
 * reports an obstruction if the cycle has <= 1 chords.
 *
 * @param g graph
 * @param start_u Edge start vertex (path origin)
 * @param target_v Edge end vertex (path target)
 * @param path Current path vertex sequence
 * @param in_path Flag indicating whether a vertex is on the path
 * @param chord_count Running count of chords between path vertices
 * @param depth Number of edges from u to cur
 * @return true if an obstruction is found
 */
inline bool meyniel_obstruction_dfs(const Graph& g,
                                     int start_u, int target_v,
                                     std::vector<int>& path,
                                     std::vector<unsigned char>& in_path,
                                     int chord_count, int depth) {
    int cur = path.back();

    for (size_t i = 0; i < g.adj[cur].size(); ++i) {
        int w = g.adj[cur][i];

        if (w == target_v) {
            // Skip the direct edge u-v (when depth==0)
            if (depth == 0) continue;

            // Cycle length = depth + 2 (path's depth+1 edges + closing edge v-u)
            // Odd cycle: depth + 2 is odd -> depth is odd
            // Length >= 5: depth + 2 >= 5 -> depth >= 3
            if (depth < 3 || depth % 2 == 0) continue;

            // Cycle: path[0]=u, path[1], ..., path[depth]=cur, v, back to u
            // Edges from v to path[1..depth-1] are additional chords
            // (v-path[0]=v-u is the closing edge, v-path[depth]=v-cur is a path edge)
            int extra = 0;
            for (int a = 1; a < depth; ++a) {
                if (g.adj_set[target_v].count(path[a])) {
                    extra++;
                    if (chord_count + extra > 1) break;
                }
            }
            if (chord_count + extra <= 1) return true;
            continue;
        }

        if (in_path[w]) continue;

        // New chords when adding w to the path:
        // Edges from w to path[0..depth-1] (path[depth]=cur is adjacent=path edge)
        int new_chords = 0;
        for (int a = 0; a < depth; ++a) {
            if (g.adj_set[w].count(path[a])) {
                new_chords++;
            }
        }

        // Pruning: if running count >= 2, cannot form an obstruction
        if (chord_count + new_chords >= 2) continue;

        in_path[w] = 1;
        path.push_back(w);
        if (meyniel_obstruction_dfs(g, start_u, target_v, path, in_path,
                                     chord_count + new_chords, depth + 1)) {
            return true;
        }
        path.pop_back();
        in_path[w] = 0;
    }
    return false;
}

}  // namespace detail_meyniel

/**
 * @brief Meyniel graph recognition (direct definition check)
 *
 * For each edge (u,v), enumerates simple paths from u to v via DFS.
 * Paths of even length >= 4 form odd cycles >= 5; if chords <= 1,
 * detected as a Meyniel obstruction.
 */
inline MeynielResult check_meyniel_direct(const Graph& g) {
    MeynielResult res;
    res.is_meyniel = true;
    if (g.n <= 4) return res;

    for (int u = 1; u <= g.n; ++u) {
        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (u >= v) continue;  // Check each edge only once

            std::vector<int> path;
            path.push_back(u);
            std::vector<unsigned char> in_path(g.n + 1, 0);
            in_path[u] = 1;

            if (detail_meyniel::meyniel_obstruction_dfs(
                    g, u, v, path, in_path, 0, 0)) {
                res.is_meyniel = false;
                // The recursion returns without unwinding, so path still holds
                // u..cur; the cycle closes through v.
                std::vector<int> cycle = path;
                cycle.push_back(v);
                res.obstruction = detail_obstruction::cycle_obstruction(
                    cycle, ObstructionKind::ODD_CYCLE_LE1_CHORD);
                return res;
            }
        }
    }
    return res;
}

/**
 * @brief Meyniel graph recognition (default)
 */
inline MeynielResult check_meyniel(
    const Graph& g,
    MeynielAlgorithm algo = MeynielAlgorithm::DIRECT_CHECK) {
    (void)algo;
    return check_meyniel_direct(g);
}

}  // namespace graph_recognition

#endif
