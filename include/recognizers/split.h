#ifndef GRAPH_RECOGNITION_SPLIT_H
#define GRAPH_RECOGNITION_SPLIT_H

/**
 * @file split.h
 * @brief Split graph recognition
 *
 * Algorithm:
 *   - DEGREE_SEQUENCE: Check if both G and complement are chordal
 *   - HAMMER_SIMEONE: Hammer-Simeone degree sequence condition (default)
 *
 * Both variants also report the split partition itself. Hammer & Simeone
 * (1981) show that when the degree condition holds, the vertices of the
 * largest degrees are the clique side; the partition is verified before being
 * returned, so it doubles as the certificate.
 */

#include "recognizers/chordal.h"
#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "util/graph_utils.h"
#include "certificates/obstruction_extract.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for split graph recognition
 */
enum class SplitAlgorithm {
    DEGREE_SEQUENCE, /**< Complement graph + chordality check */
    HAMMER_SIMEONE   /**< Hammer-Simeone degree sequence condition (default) */
};

/**
 * @brief Result of split graph recognition
 */
struct SplitResult {
    bool is_split = false; /**< true if the graph is a split graph */
    /**
     * @brief side[v] = 1 if v is on the clique side K, 2 if on the independent side S
     *
     * Sized n+1; valid only when is_split == true. A split graph usually has
     * several valid partitions; this is the one with the largest clique.
     */
    std::vector<int> side;
    /**
     * @brief NO certificate: a TWO_K2, C4 or C5
     *
     * Those are exactly the patterns split graphs forbid (Foldes--Hammer 1977).
     * Filled by DEGREE_SEQUENCE, which goes through chordality of the graph and
     * of its complement and so has a hole to project; the default
     * HAMMER_SIMEONE decides from the degree sequence alone and leaves
     * kind == NONE, with build_split_obstruction() for callers that want one.
     * Valid only when is_split == false.
     */
    Obstruction obstruction;
};

namespace detail {

/**
 * @brief Builds the split partition from a degree-sorted vertex order
 * @return side[v] in {1, 2} (size n+1), or an empty vector if the candidate
 *         is not a split partition after all
 *
 * Hammer & Simeone (1981): for degrees d1 >= ... >= dn and
 * m = max{i : di >= i-1}, a split graph's m largest-degree vertices induce a
 * clique and the rest an independent set. The result is verified here, so a
 * caller can treat a non-empty return as a certificate rather than trusting
 * the degree arithmetic.
 */
inline std::vector<int> split_partition(const Graph& g) {
    int n = g.n;
    std::vector<int> side(n + 1, 0);
    if (n == 0) return side;

    // Counting sort of the vertices by decreasing degree.
    std::vector<int> cnt(n, 0);
    for (int v = 1; v <= n; ++v) cnt[(int)g.adj[v].size()]++;
    std::vector<int> start(n + 1, 0);
    int pos = 0;
    for (int k = n - 1; k >= 0; --k) {
        start[k] = pos;
        pos += cnt[k];
    }
    std::vector<int> order(n, 0);
    for (int v = 1; v <= n; ++v) order[start[(int)g.adj[v].size()]++] = v;

    int m_val = 0;
    for (int i = 1; i <= n; ++i) {
        if ((int)g.adj[order[i - 1]].size() < i - 1) break;
        m_val = i;
    }

    for (int i = 0; i < n; ++i) side[order[i]] = i < m_val ? 1 : 2;

    // K is a clique and S is independent, or this is not a split partition.
    for (int i = 0; i < m_val; ++i) {
        for (int j = i + 1; j < m_val; ++j) {
            if (!g.has_edge(order[i], order[j])) return std::vector<int>();
        }
    }
    for (int v = 1; v <= n; ++v) {
        if (side[v] != 2) continue;
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            if (side[g.adj[v][i]] == 2) return std::vector<int>();
        }
    }
    return side;
}

/** @brief Split graph recognition via chordality of G and its complement (original algorithm) */
inline SplitResult check_split_complement(const Graph& g) {
    SplitResult res;
    res.is_split = false;

    ChordalResult g_chordal = check_chordal(g);
    if (!g_chordal.is_chordal) {
        // A hole of length >= 6 is not itself a split obstruction, so it is
        // projected onto the C4, C5 or 2K2 it contains.
        res.obstruction = detail_obstruction::split_obstruction_from_hole(
            g_chordal.obstruction.vertices, false);
        return res;
    }

    Graph gc = build_complement(g);
    ChordalResult gc_chordal = check_chordal(gc);
    if (!gc_chordal.is_chordal) {
        // Read in the complement, but the projection lands a pattern of g.
        res.obstruction = detail_obstruction::split_obstruction_from_hole(
            gc_chordal.obstruction.vertices, true);
        return res;
    }

    res.side = split_partition(g);
    if (res.side.empty()) return res;
    res.is_split = true;
    return res;
}

/**
 * @brief Split graph recognition via Hammer-Simeone degree sequence condition
 *
 * For degree sequence d1 >= d2 >= ... >= dn,
 * let m = max{i : di >= i-1}, then
 * Σ_{i=1}^{m} di = m(m-1) + Σ_{i=m+1}^{n} di
 * If this holds, the graph is a split graph.
 *
 * The degree arithmetic is O(n) (counting sort), but a YES also builds and
 * verifies the (K,S) partition it returns (split_partition -> O(n+m)), so
 * the whole call is O(n+m).
 */
inline SplitResult check_split_hammer_simeone(const Graph& g) {
    SplitResult res;
    res.is_split = false;

    int n = g.n;
    // side stays a size n+1 partition even here: a YES result whose side
    // vector a caller cannot index is not the documented certificate.
    if (n == 0) { res.side.assign(1, 0); res.is_split = true; return res; }

    // Compute degrees
    std::vector<int> deg(n);
    for (int v = 1; v <= n; ++v) {
        deg[v - 1] = (int)g.adj[v].size();
    }

    // Counting sort (descending)
    std::vector<int> cnt(n, 0);
    for (int i = 0; i < n; ++i) cnt[deg[i]]++;
    std::vector<int> d(n);
    int pos = 0;
    for (int k = n - 1; k >= 0; --k) {
        for (int c = 0; c < cnt[k]; ++c) {
            d[pos++] = k;
        }
    }

    // m_val = max{i : d[i-1] >= i-1} (1-indexed)
    int m_val = 0;
    for (int i = 1; i <= n; ++i) {
        if (d[i - 1] >= i - 1) {
            m_val = i;
        } else {
            break;
        }
    }

    // Condition check: Σ_{i=1}^{m} d[i] = m(m-1) + Σ_{i=m+1}^{n} d[i]
    long long left_sum = 0;
    for (int i = 0; i < m_val; ++i) left_sum += d[i];

    long long right_sum = 0;
    for (int i = m_val; i < n; ++i) right_sum += d[i];

    long long target = (long long)m_val * (m_val - 1) + right_sum;

    if (left_sum != target) return res;

    res.side = split_partition(g);
    if (res.side.empty()) return res;
    res.is_split = true;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is a split graph
 * @param g Input graph
 * @param algo Algorithm to use (default: HAMMER_SIMEONE)
 * @return SplitResult
 *
 * A value outside the enum (a cast from a restored setting, a C ABI boundary)
 * runs the default variant: returning the default-constructed NO result would
 * report a configuration mistake as a mathematical answer, and one without the
 * partition or the obstruction a real answer carries.
 */
inline SplitResult check_split(const Graph& g,
    SplitAlgorithm algo = SplitAlgorithm::HAMMER_SIMEONE) {
    switch (algo) {
        case SplitAlgorithm::DEGREE_SEQUENCE:
            return detail::check_split_complement(g);
        case SplitAlgorithm::HAMMER_SIMEONE:
        default:
            break;
    }
    return detail::check_split_hammer_simeone(g);
}

/**
 * @brief Builds a NO certificate for a non-split graph
 * @param g Input graph
 * @return A TWO_K2, C4 or C5, or an empty obstruction if g is a split graph
 *
 * Runs the DEGREE_SEQUENCE variant, the one that reaches a hole to project.
 * The default recognizer keeps its own cost.
 */
inline Obstruction build_split_obstruction(const Graph& g) {
    return detail::check_split_complement(g).obstruction;
}

} // namespace graph_recognition

#endif
