#ifndef GRAPH_RECOGNITION_CHORDAL_H
#define GRAPH_RECOGNITION_CHORDAL_H

/**
 * @file chordal.h
 * @brief Chordal graph recognition
 *
 * Algorithms:
 *   - MCS_PEO: priority queue MCS + PEO verification O(n+m log n)
 *   - BUCKET_MCS_PEO: bucket sort MCS + PEO verification O(n+m) (default)
 *   - LEXBFS_PEO: LexBFS + PEO verification O(n+m)
 */

#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "util/lexbfs.h"
#include "util/mcs.h"
#include "certificates/obstruction_extract.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for chordal graph recognition
 */
enum class ChordalAlgorithm {
    MCS_PEO,        /**< priority queue MCS + PEO verification O(n+m log n) */
    BUCKET_MCS_PEO, /**< bucket sort MCS + PEO verification O(n+m) (default) */
    LEXBFS_PEO      /**< LexBFS + PEO verification O(n+m) */
};

/**
 * @brief Result of chordal graph recognition
 */
struct ChordalResult {
    bool is_chordal = false;                          /**< true if the graph is chordal */
    MCSResult mcs_result;                     /**< MCS result */
    std::vector<int> parent;                  /**< parent[v]: parent of v in PEO (0 if root) */
    std::vector<std::vector<int>> later;      /**< later[v]: adjacent vertices after v */
    Obstruction obstruction;                  /**< NO certificate: a HOLE. Valid only when
                                                   is_chordal == false; filled by every
                                                   algorithm variant */
};

namespace detail {

/**
 * @brief Verifies PEO from MCS result and constructs ChordalResult (common processing)
 *
 * The Tarjan--Yannakakis test fails at a triple (v, parent[v], u) of a vertex
 * and two of its non-adjacent later-neighbours, which is exactly the seed a
 * hole extraction needs; the failing ordering therefore doubles as the NO
 * certificate.
 */
inline ChordalResult verify_peo(const Graph& g, const MCSResult& mcs_res) {
    ChordalResult res;
    int n = g.n;
    res.mcs_result = mcs_res;
    const std::vector<int>& number = res.mcs_result.number;

    res.later.resize(n + 1);
    for (int v = 1; v <= n; ++v) {
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (number[v] < number[u]) res.later[v].push_back(u);
        }
    }

    res.parent.resize(n + 1, 0);
    res.is_chordal = true;
    for (int v = 1; v <= n; ++v) {
        int best = n + 1;
        for (size_t i = 0; i < res.later[v].size(); ++i) {
            int u = res.later[v][i];
            if (number[u] < best) {
                best = number[u];
                res.parent[v] = u;
            }
        }
        if (res.parent[v] == 0) continue;
        for (size_t i = 0; i < res.later[v].size(); ++i) {
            int u = res.later[v][i];
            if (u == res.parent[v]) continue;
            if (!g.has_edge(res.parent[v], u)) {
                res.is_chordal = false;
                res.obstruction = detail_obstruction::cycle_obstruction(
                    detail_obstruction::hole_from_failed_peo(g, v, res.parent[v], u),
                    ObstructionKind::HOLE);
                return res;
            }
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is a chordal graph
 * @param g Input graph
 * @param algo Algorithm to use (default: BUCKET_MCS_PEO)
 * @return ChordalResult
 *
 * Computes PEO candidates via MCS and verifies that the later-adjacent vertices
 * of each vertex form a clique. If chordal, also returns PEO, parent, and later structures.
 */
inline ChordalResult check_chordal(const Graph& g,
    ChordalAlgorithm algo = ChordalAlgorithm::BUCKET_MCS_PEO) {
    switch (algo) {
        case ChordalAlgorithm::MCS_PEO:
            return detail::verify_peo(g, mcs(g, MCSAlgorithm::PQ_MCS));
        case ChordalAlgorithm::BUCKET_MCS_PEO:
            return detail::verify_peo(g, mcs(g, MCSAlgorithm::BUCKET_MCS));
        case ChordalAlgorithm::LEXBFS_PEO:
            return detail::verify_peo(g, lexbfs(g));
        default:
            break;
    }
    return ChordalResult();
}

} // namespace graph_recognition

#endif
