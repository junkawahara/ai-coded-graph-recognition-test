#ifndef GRAPH_RECOGNITION_CAGE_H
#define GRAPH_RECOGNITION_CAGE_H

/**
 * @file cage.h
 * @brief (k,g)-graph and cage recognition
 *
 * A (k,g)-graph is a k-regular graph with girth at least g (the class
 * GENREG generates; some authors require girth exactly g — that variant
 * is `is_kg_graph && girth == g` via the always-filled girth field). A
 * (k,g)-cage is a k-regular graph of girth exactly g with the minimum
 * possible number of vertices n(k,g): the Petersen graph is the
 * (3,5)-cage, the Heawood graph the (3,6)-cage, the McGee graph the
 * (3,7)-cage. Like partial_ktree.h and degenerate.h, the class
 * parameters k and g are inputs.
 *
 * Regularity and girth are checked directly (girth by the standard
 * truncated BFS from every vertex, O(n*m)). Cage-ness additionally
 * needs minimality, which no polynomial certificate is known for; it is
 * decided exactly, by running the girth-constrained k-regular existence
 * search of `enumerators/cage_unlabeled_enum.h` (an early-exit GENREG,
 * which is why this recognizer includes an enumerator header — the
 * reverse of the usual dependency) on every feasible smaller order.
 * The search starts at the Moore bound M(k,g), so Moore-tight cages
 * (the cycles C_g, every (k,3) = K_{k+1} and (k,4) = K_{k,k}, the
 * Petersen graph and the Heawood graph) are confirmed with no search
 * at all; for the others each smaller order needs an exhaustive
 * nonexistence proof, which is where the practical range ends — the
 * McGee graph sits two orders above M(3,7) = 22, and the girth->= 7
 * search at n = 22 already exceeds minutes. Minimality against graphs
 * of girth *at least* g is equivalent to the standard exact-girth
 * definition by the strict monotonicity of n(k,g) in g
 * (Fu-Huang-Rodger 1997).
 *
 * References:
 *   Exoo, Jajcay, "Dynamic cage survey," Electron. J. Combin. DS16
 *   (definitions, Moore bound, known cage orders);
 *   Meringer, "Fast generation of regular graphs and construction of
 *   cages," J. Graph Theory 30, 1999 (the search used for minimality);
 *   Fu, Huang, Rodger, "Connectivity of cages," J. Graph Theory 24,
 *   1997 (monotonicity of n(k,g))
 */

#include "enumerators/cage_unlabeled_enum.h"
#include "util/graph.h"

#include <stdexcept>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for cage recognition
 */
enum class CageAlgorithm {
    GIRTH_AND_EXHAUSTIVE_MINIMALITY /**< BFS girth + GENREG-style existence search below n */
};

/**
 * @brief Result of (k,g)-graph / cage recognition
 */
struct CageResult {
    /**
     * @brief true if the graph is a (k,g)-cage
     *
     * k-regular with girth exactly g, and no k-regular graph with girth
     * >= g has fewer vertices (verified by exhaustive search from the
     * Moore bound up; equivalent to minimality among girth-exactly-g
     * graphs by the monotonicity of n(k,g)). Moore-tight cages are decided
     * at every order. For a non-Moore-tight candidate whose minimality would
     * require an existence search on 64 or more vertices, `check_cage`
     * throws `std::runtime_error` rather than reporting a false negative.
     */
    bool is_cage = false;
    /**
     * @brief true if the graph is a (k,g)-graph: k-regular with girth >= g
     *
     * Acyclic graphs count as girth infinity, so for k <= 1 this is
     * just k-regularity; g <= 3 is no constraint on a simple graph.
     * The exact-girth variant of the definition is
     * `is_kg_graph && girth == g`.
     */
    bool is_kg_graph = false;
    /**
     * @brief The girth of the graph (length of a shortest cycle)
     *
     * Always filled; 0 when the graph is acyclic (infinite girth).
     */
    int girth = 0;
};

namespace detail {

/**
 * @brief The girth of g by truncated BFS from every vertex
 * @return The length of a shortest cycle, or 0 if the graph is acyclic
 *
 * From each start vertex a BFS finds the shortest cycle through it as
 * the best dist[u] + dist[v] + 1 over non-tree edges (u, v); the
 * minimum over all start vertices is exact, and each BFS stops at
 * depth girth/2 since deeper layers cannot improve it. O(n*m).
 */
inline int cage_girth(const Graph& g) {
    const int n = g.n;
    int girth = n + 1;
    std::vector<int> dist(n + 1);
    std::vector<int> parent(n + 1);
    std::vector<int> queue;
    for (int s = 1; s <= n; ++s) {
        for (int v = 1; v <= n; ++v) dist[v] = -1;
        queue.clear();
        queue.push_back(s);
        dist[s] = 0;
        parent[s] = 0;
        for (std::size_t head = 0; head < queue.size(); ++head) {
            const int v = queue[head];
            if (dist[v] >= girth / 2) break; /* No further improvement possible */
            for (std::size_t j = 0; j < g.adj[v].size(); ++j) {
                const int u = g.adj[v][j];
                if (dist[u] == -1) {
                    dist[u] = dist[v] + 1;
                    parent[u] = v;
                    queue.push_back(u);
                } else if (u != parent[v]) {
                    const int len = dist[v] + dist[u] + 1;
                    if (len < girth) girth = len;
                }
            }
        }
    }
    return girth > n ? 0 : girth;
}

}  // namespace detail

/**
 * @brief Determines whether the given graph is a (k,g)-graph / a (k,g)-cage
 * @param g Input graph
 * @param k Degree parameter of the class
 * @param girth Girth parameter g of the class
 * @param algo Algorithm to use (default: GIRTH_AND_EXHAUSTIVE_MINIMALITY)
 * @return CageResult
 *
 * The membership test (is_kg_graph) is O(n*m). The cage test runs the
 * existence search on every order from the Moore bound M(k, girth) to
 * n - 1 (skipping odd n*k), so it is free for Moore-tight graphs and
 * exponential in the worst case; it is attempted only when the graph
 * is a (k,g)-graph of girth exactly `girth` (girth >= 3). Moore-tight
 * candidates need no search and therefore have no vertex-count limit.
 * The empty graph is a (0,g)-graph but never a cage; k < 0 gets
 * all-false.
 *
 * @throws std::runtime_error if exact minimality would require searching
 *         for a k-regular graph on 64 or more vertices
 */
inline CageResult check_cage(const Graph& g, int k, int girth,
    CageAlgorithm algo = CageAlgorithm::GIRTH_AND_EXHAUSTIVE_MINIMALITY) {
    (void)algo;
    CageResult res;
    const int n = g.n;
    if (k < 0) return res;
    bool regular = true;
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() != k) {
            regular = false;
            break;
        }
    }
    if (n == 0) regular = (k == 0);
    res.girth = detail::cage_girth(g);
    if (!regular) return res;
    /* girth 0 = acyclic = infinite girth: >= any bound */
    res.is_kg_graph = (res.girth == 0 || res.girth >= girth);
    if (!res.is_kg_graph || girth < 3 || res.girth != girth) {
        return res;
    }
    for (long long np = detail::cage_unlabeled_moore_bound(k, girth);
         np < n; ++np) {
        if (np * k % 2 != 0) continue;
        if (np >= 64) {
            throw std::runtime_error(
                "cage minimality search requires 64 or more vertices");
        }
        if (cage_kg_graph_exists((int)np, k, girth)) return res;
    }
    res.is_cage = true;
    return res;
}

}  // namespace graph_recognition

#endif
