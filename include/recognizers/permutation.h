#ifndef GRAPH_RECOGNITION_PERMUTATION_H
#define GRAPH_RECOGNITION_PERMUTATION_H

/**
 * @file permutation.h
 * @brief Permutation graph recognition
 *
 * Determines whether the graph is a permutation graph by checking that
 * both G and its complement are comparability graphs.
 *
 * Algorithms:
 *   - BACKTRACKING: Transitive orientation via backtracking
 *   - CLASS_BASED: Orientation by Gamma class (O(nm), default)
 *
 * build_permutation_realizer() additionally constructs the permutation
 * diagram itself (Pnueli, Lempel & Even 1971; Golumbic, Algorithmic Graph
 * Theory and Perfect Graphs, Ch. 7).
 */

#include "recognizers/comparability.h"
#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "util/graph_utils.h"
#include "certificates/obstruction_extract.h"
#include "decompositions/transitive_orientation.h"
#include <stdexcept>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for permutation graph recognition
 */
enum class PermutationAlgorithm {
    BACKTRACKING, /**< Transitive orientation via backtracking */
    CLASS_BASED   /**< Orientation by Gamma class (default) */
};

/**
 * @brief Result of permutation graph recognition
 */
struct PermutationResult {
    bool is_permutation = false; /**< true if the graph is a permutation graph */
    Obstruction obstruction; /**< NO certificate: a FORCING_CYCLE, in g or -- with
                                  in_complement set -- in its complement, whichever
                                  side fails to be a comparability graph. Left empty
                                  by the recognizer; use
                                  build_permutation_obstruction(). Valid only when
                                  is_permutation == false */
};

/**
 * @brief A permutation diagram of a permutation graph
 *
 * The vertices are written on two parallel lines; two vertices are adjacent
 * exactly when their segments cross, that is when their order differs between
 * the lines. All fields are valid only when is_permutation == true.
 */
struct PermutationRealizerResult {
    bool is_permutation = false; /**< true if the graph is a permutation graph */
    std::vector<int> pos1;       /**< pos1[v] = position of v on the first line, in [1, n] (size n+1) */
    std::vector<int> pos2;       /**< pos2[v] = position of v on the second line, in [1, n] (size n+1) */
    std::vector<int> pi;         /**< pi[i] = pos2 of the vertex at position i on the first line (size n+1) */
};

namespace detail {

/**
 * @brief Permutation graph recognition via backtracking
 */
inline PermutationResult check_permutation_backtracking(const Graph& g) {
    PermutationResult res;
    res.is_permutation = false;

    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    if (!is_comparability_graph(a)) return res;

    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);
    if (!is_comparability_graph(c)) return res;

    res.is_permutation = true;
    return res;
}

/**
 * @brief Permutation graph recognition via class-based method
 */
inline PermutationResult check_permutation_class_based(const Graph& g) {
    PermutationResult res;
    res.is_permutation = false;

    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    if (!is_comparability_graph_class_based(a)) return res;

    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);
    if (!is_comparability_graph_class_based(c)) return res;

    res.is_permutation = true;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is a permutation graph
 * @param g Input graph
 * @param algo Algorithm to use (default: CLASS_BASED)
 * @return PermutationResult
 */
inline PermutationResult check_permutation(const Graph& g,
    PermutationAlgorithm algo = PermutationAlgorithm::CLASS_BASED) {
    switch (algo) {
        case PermutationAlgorithm::BACKTRACKING:
            return detail::check_permutation_backtracking(g);
        case PermutationAlgorithm::CLASS_BASED:
            return detail::check_permutation_class_based(g);
        default:
            break;
    }
    return PermutationResult();
}

/**
 * @brief Builds a permutation diagram of a permutation graph
 * @param g Input graph
 * @param algo Algorithm used for the two transitive orientations (default: CLASS_BASED)
 * @return PermutationRealizerResult
 * @throws std::runtime_error if the diagram built does not describe g, which
 *         would be a bug rather than a property of the input
 *
 * Pnueli, Lempel & Even (1971): if F1 transitively orients G and F2 orients
 * its complement, then F1 union F2 is a transitive tournament, and so is
 * F1-reversed union F2. Reading a vertex's rank off each tournament places it
 * on the two lines. An edge of G is oriented by F1, so it is inverted between
 * the lines; a non-edge is oriented by F2 the same way in both, so it is not.
 *
 * The ranks are taken as in-degrees rather than by sorting with a comparator:
 * a transitive tournament on n vertices has in-degrees exactly 0..n-1, so the
 * in-degrees already are the positions, and the construction never depends on
 * a comparator whose consistency rests on the theorem being applied correctly.
 */
inline PermutationRealizerResult build_permutation_realizer(const Graph& g,
    PermutationAlgorithm algo = PermutationAlgorithm::CLASS_BASED) {
    PermutationRealizerResult res;
    int n = g.n;

    TransitiveOrientationAlgorithm to_algo =
        algo == PermutationAlgorithm::BACKTRACKING
            ? TransitiveOrientationAlgorithm::BACKTRACKING
            : TransitiveOrientationAlgorithm::FORCING;

    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);

    TransitiveOrientationResult f1 = transitive_orientation_matrix(a, to_algo);
    if (!f1.is_comparability) return res;
    TransitiveOrientationResult f2 = transitive_orientation_matrix(c, to_algo);
    if (!f2.is_comparability) return res;

    res.pos1.assign(n + 1, 0);
    res.pos2.assign(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        int in1 = 0, in2 = 0;
        for (int u = 1; u <= n; ++u) {
            if (u == v) continue;
            // Line 1 follows F1 and F2; line 2 follows F1 reversed and F2.
            if (f1.dir[u][v] == 1 || f2.dir[u][v] == 1) ++in1;
            if (f1.dir[v][u] == 1 || f2.dir[u][v] == 1) ++in2;
        }
        res.pos1[v] = in1 + 1;
        res.pos2[v] = in2 + 1;
    }

    // Both rank vectors must be permutations of 1..n, which is what makes the
    // two unions transitive tournaments.
    std::vector<int> seen1(n + 2, 0), seen2(n + 2, 0);
    for (int v = 1; v <= n; ++v) {
        int p1 = res.pos1[v], p2 = res.pos2[v];
        if (p1 < 1 || p1 > n || seen1[p1]) return PermutationRealizerResult();
        if (p2 < 1 || p2 > n || seen2[p2]) return PermutationRealizerResult();
        seen1[p1] = 1;
        seen2[p2] = 1;
    }

    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            bool crossing = (res.pos1[u] < res.pos1[v]) != (res.pos2[u] < res.pos2[v]);
            if (crossing != g.has_edge(u, v)) {
                throw std::runtime_error(
                    "build_permutation_realizer: the diagram does not describe the graph");
            }
        }
    }

    res.pi.assign(n + 1, 0);
    for (int v = 1; v <= n; ++v) res.pi[res.pos1[v]] = res.pos2[v];
    res.is_permutation = true;
    return res;
}

/**
 * @brief Builds a NO certificate for a non-permutation graph
 * @param g Input graph
 * @return A FORCING_CYCLE of g or of its complement, or an empty obstruction if
 *         g is a permutation graph
 *
 * Permutation graphs are the graphs that are comparability graphs together
 * with their complements (Pnueli--Lempel--Even 1971), so the witness comes
 * from whichever of the two sides fails.
 */
inline Obstruction build_permutation_obstruction(const Graph& g) {
    Obstruction primal = detail_obstruction::find_forcing_cycle(g);
    if (primal.has_witness()) return primal;
    Obstruction dual = detail_obstruction::find_forcing_cycle(build_complement(g));
    if (dual.has_witness()) dual.in_complement = true;
    return dual;
}

} // namespace graph_recognition

#endif
