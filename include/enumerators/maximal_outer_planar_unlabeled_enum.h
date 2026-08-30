#ifndef GRAPH_RECOGNITION_MAXIMAL_OUTER_PLANAR_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_MAXIMAL_OUTER_PLANAR_UNLABELED_ENUM_H

/**
 * @file maximal_outer_planar_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic maximal outerplanar graphs
 *
 * Enumerates one representative per isomorphism class of maximal
 * outerplanar graphs (= simple 2-trees for n >= 2) on n vertices via the
 * dedicated polygon-triangulation construction.
 *
 * A maximal outerplanar graph on n >= 3 vertices is exactly a
 * triangulation of a convex n-gon: 2-connected, with a Hamiltonian outer
 * cycle and every inner face a triangle. A 2-connected outerplanar graph
 * has a *unique* Hamiltonian cycle, so every graph isomorphism between two
 * polygon triangulations maps outer cycle to outer cycle and is therefore
 * a symmetry of the n-gon. Hence isomorphism classes of maximal
 * outerplanar graphs correspond bijectively to orbits of polygon
 * triangulations under the dihedral group D_n (n rotations x reflection),
 * and no general-graph canonicalization is needed.
 *
 * Algorithm:
 *   1. Enumerate all Catalan(n-2) triangulations of the convex polygon
 *      1, 2, ..., n by the standard recursion: the triangle on chord (i, j)
 *      has a unique apex k with i < k < j, so choosing the apex and
 *      recursing on both sub-polygons visits each triangulation exactly
 *      once (diagonals are accumulated along the way).
 *   2. Emit a triangulation iff its diagonal set is the lexicographic
 *      minimum among its 2n images under D_n (canonical-orbit-
 *      representative test; O(n^2 log n) per triangulation).
 *
 * Non-isomorphic counts: OEIS A000207 (triangulations of an n-gon under
 * rotations and reflections), for n = 3, 4, 5, ...:
 *   1, 1, 1, 3, 4, 12, 27, 82, 228, 733, 2282, 7528, ...
 * K0, K1 and K2 are emitted as the trivial members for n <= 2.
 * Every maximal outerplanar graph is connected, so there is no
 * `connected_only` flag.
 *
 * The survey's O(1)-per-graph bound (fan-decomposition construction,
 * Bodirsky--Fusy--Kang--Vigerske 2007) does not apply to this
 * implementation: the total work is Theta(Catalan(n-2) * n^2 log n) over
 * about Catalan(n-2)/(2n) output classes, which is practical to about
 * n = 16 (Catalan(14) = 2,674,440 triangulations).
 *
 * References:
 *   Bodirsky, Fusy, Kang, Vigerske, "Enumeration and asymptotic properties
 *   of unlabeled outerplanar graphs," Electron. J. Combin. 14, 2007;
 *   Brinkmann, McKay, "Fast generation of planar graphs," 2007 (plantri's
 *   dual route to 2-connected outerplanar graphs);
 *   OEIS A000207
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic maximal outerplanar enumeration
 */
enum class MaximalOuterPlanarUnlabeledEnumAlgorithm {
    POLYGON_TRIANGULATION /**< Polygon triangulations modulo the dihedral group */
};

/**
 * @brief An enumerated maximal outerplanar graph
 */
struct MaximalOuterPlanarUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic maximal outerplanar enumeration
 */
struct MaximalOuterPlanarUnlabeledEnumerationResult {
    std::vector<MaximalOuterPlanarUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/**
 * @brief Tests whether a diagonal set is its dihedral orbit's representative
 * @param n Polygon size (>= 3)
 * @param diags Diagonals of the triangulation, endpoints in [0, n)
 * @return true iff the normalized diagonal set is lexicographically <= all
 *         of its 2n images under the dihedral group D_n
 */
inline bool maximal_outer_planar_unlabeled_is_canonical(
    int n, const std::vector<std::pair<int, int> >& diags) {
    std::vector<std::pair<int, int> > base = diags;
    for (std::size_t i = 0; i < base.size(); ++i) {
        if (base[i].first > base[i].second) {
            std::swap(base[i].first, base[i].second);
        }
    }
    std::sort(base.begin(), base.end());

    std::vector<std::pair<int, int> > image(diags.size());
    for (int refl = 0; refl < 2; ++refl) {
        for (int rot = 0; rot < n; ++rot) {
            if (refl == 0 && rot == 0) continue;  // identity: equal to base
            for (std::size_t i = 0; i < diags.size(); ++i) {
                int a = diags[i].first;
                int b = diags[i].second;
                if (refl) {
                    a = (n - a) % n;
                    b = (n - b) % n;
                }
                a = (a + rot) % n;
                b = (b + rot) % n;
                if (a > b) std::swap(a, b);
                image[i] = std::make_pair(a, b);
            }
            std::sort(image.begin(), image.end());
            if (image < base) return false;
        }
    }
    return true;
}

/**
 * @brief Builds the output graph (outer cycle + diagonals, 1-indexed)
 */
inline MaximalOuterPlanarUnlabeledEnumeratedGraph
maximal_outer_planar_unlabeled_build_graph(
    int n, const std::vector<std::pair<int, int> >& diags) {
    MaximalOuterPlanarUnlabeledEnumeratedGraph g;
    g.n = n;
    g.edges.reserve(static_cast<std::size_t>(n) + diags.size());
    for (int i = 0; i < n; ++i) {
        int u = i + 1;
        int v = (i + 1) % n + 1;
        if (u > v) std::swap(u, v);
        g.edges.push_back(std::make_pair(u, v));
    }
    for (std::size_t i = 0; i < diags.size(); ++i) {
        int u = diags[i].first + 1;
        int v = diags[i].second + 1;
        if (u > v) std::swap(u, v);
        g.edges.push_back(std::make_pair(u, v));
    }
    std::sort(g.edges.begin(), g.edges.end());
    return g;
}

/**
 * @brief DFS over polygon triangulations
 * @param n Polygon size
 * @param work Stack of chords (i, j), j - i >= 2, whose sub-polygon
 *             i, i+1, ..., j is still to be triangulated
 * @param diags Diagonals chosen so far (endpoints in [0, n))
 * @param results Storage for results
 *
 * Pops one open chord (i, j) and tries every apex k with i < k < j for the
 * triangle resting on it; the sub-chords (i, k) and (k, j) that span more
 * than one polygon edge become new diagonals and new open chords. Each
 * completed diagonal set is a distinct triangulation (the apex on every
 * chord is unique per triangulation), and it is emitted iff it is its
 * dihedral orbit's canonical representative.
 */
inline void maximal_outer_planar_unlabeled_enum_dfs(
    int n, std::vector<std::pair<int, int> >& work,
    std::vector<std::pair<int, int> >& diags,
    std::vector<MaximalOuterPlanarUnlabeledEnumeratedGraph>& results) {
    if (work.empty()) {
        if (maximal_outer_planar_unlabeled_is_canonical(n, diags)) {
            results.push_back(
                maximal_outer_planar_unlabeled_build_graph(n, diags));
        }
        return;
    }
    const std::pair<int, int> chord = work.back();
    work.pop_back();
    const int i = chord.first;
    const int j = chord.second;
    for (int k = i + 1; k < j; ++k) {
        const std::size_t work_mark = work.size();
        const std::size_t diag_mark = diags.size();
        if (k - i >= 2) {
            work.push_back(std::make_pair(i, k));
            diags.push_back(std::make_pair(i, k));
        }
        if (j - k >= 2) {
            work.push_back(std::make_pair(k, j));
            diags.push_back(std::make_pair(k, j));
        }
        maximal_outer_planar_unlabeled_enum_dfs(n, work, diags, results);
        work.resize(work_mark);
        diags.resize(diag_mark);
    }
    work.push_back(chord);
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic maximal outerplanar graphs on n vertices
 * @param n Number of vertices
 * @param algo Algorithm to use (default: POLYGON_TRIANGULATION)
 * @return MaximalOuterPlanarUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A000207(n) graphs for
 * n >= 3 (1, 1, 1, 3, 4, 12, 27, 82, 228, 733, ...), and the single
 * trivial member K0 / K1 / K2 for n = 0, 1, 2. Negative n yields nothing.
 * All emitted graphs are connected (the class contains no disconnected
 * graph), so there is no `connected_only` flag.
 *
 * @note The search walks all Catalan(n-2) polygon triangulations and keeps
 *       about one in 2n of them, so the enumeration is practical to about
 *       n = 16 (2,674,440 triangulations, 83,898 classes, about 2 s).
 */
inline MaximalOuterPlanarUnlabeledEnumerationResult
enumerate_maximal_outer_planar_unlabeled_graphs(
    int n,
    MaximalOuterPlanarUnlabeledEnumAlgorithm algo =
        MaximalOuterPlanarUnlabeledEnumAlgorithm::POLYGON_TRIANGULATION) {
    (void)algo;
    MaximalOuterPlanarUnlabeledEnumerationResult result;
    if (n < 0) return result;
    if (n <= 2) {
        MaximalOuterPlanarUnlabeledEnumeratedGraph g;
        g.n = n;
        if (n == 2) g.edges.push_back(std::make_pair(1, 2));
        result.graphs.push_back(g);
        return result;
    }
    std::vector<std::pair<int, int> > work;
    std::vector<std::pair<int, int> > diags;
    work.push_back(std::make_pair(0, n - 1));
    detail::maximal_outer_planar_unlabeled_enum_dfs(n, work, diags,
                                                    result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
