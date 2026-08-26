#ifndef GRAPH_RECOGNITION_HALIN_H
#define GRAPH_RECOGNITION_HALIN_H

/**
 * @file halin.h
 * @brief Halin graph recognition
 *
 * A Halin graph is formed by embedding a tree with no internal vertices of degree 2
 * in the plane, then connecting all leaves with a cycle.
 * Recognition: a 3-connected planar graph where removing the cycle edges of some face
 * yields a tree whose leaves coincide with the vertices of that face.
 */

#include "util/graph.h"
#include "recognizers/planar.h"
#include "recognizers/triconnected.h"
#include "decompositions/planar_embedding.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for Halin graph recognition
 */
enum class HalinAlgorithm {
    FACE_CHECK /**< Face check on planar embedding */
};

/**
 * @brief Result of Halin graph recognition
 */
struct HalinResult {
    bool is_halin = false; /**< true if the graph is a Halin graph */
};

/**
 * @brief Determines whether the graph is a Halin graph
 * @param g Input graph
 * @param algo Algorithm to use (default: FACE_CHECK)
 * @return HalinResult
 *
 * Halin graph iff 3-connected planar graph where for some face F,
 * removing F's edges yields a tree T with leaf set = F's vertex set,
 * and all internal vertices of T have degree >= 3.
 */
inline HalinResult check_halin(const Graph& g,
    HalinAlgorithm algo = HalinAlgorithm::FACE_CHECK) {
    (void)algo;
    HalinResult res;

    int n = g.n;
    if (n < 4) return res;

    /* Minimum degree >= 3 */
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() < 3) return res;
    }

    /* 3-connectivity check */
    TriconnectedResult tr = check_triconnected(g);
    if (!tr.is_triconnected) return res;

    /* Planarity check */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    /* Compute planar embedding */
    PlanarEmbeddingResult emb = compute_planar_embedding(g);
    if (!emb.success) return res;

    /* Edge count */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    /* For each face: check if the remainder after removing face edges satisfies conditions */
    for (size_t fi = 0; fi < emb.faces.size(); ++fi) {
        const std::vector<int>& face = emb.faces[fi];
        int fsize = (int)face.size();
        if (fsize < 3) continue;

        /* Number of face edges = fsize (cycle) */
        /* Tree edge count = m - fsize, vertices = n -> tree requires m - fsize = n - 1 */
        if ((long long)(m - fsize) != (long long)(n - 1)) continue;

        /* Build the set of face edges */
        std::vector<char> face_vertex(n + 1, 0);
        /* if v is in face_edge[u], then (u,v) is a face edge */
        std::vector<std::unordered_set<int>> face_edge(n + 1);
        for (int i = 0; i < fsize; ++i) {
            int u = face[i];
            int v = face[(i + 1) % fsize];
            face_vertex[u] = 1;
            face_edge[u].insert(v);
            face_edge[v].insert(u);
        }

        /* Compute vertex degrees in the graph with face edges removed */
        std::vector<int> tree_deg(n + 1, 0);
        for (int v = 1; v <= n; ++v) {
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (!face_edge[v].count(u)) {
                    tree_deg[v]++;
                }
            }
        }
        /* tree_deg does not double-count edges
           (counts 1 for each element of adj[v]) */

        /* Condition check:
           1. Face vertices = tree leaves (tree_deg == 1)
           2. Non-face vertices = tree interior (tree_deg >= 3)
           3. Remaining graph is a tree (connected + edge count = n-1, already checked) */
        bool valid = true;
        for (int v = 1; v <= n; ++v) {
            if (face_vertex[v]) {
                if (tree_deg[v] != 1) { valid = false; break; }
            } else {
                if (tree_deg[v] < 3) { valid = false; break; }
            }
        }
        if (!valid) continue;

        /* Connectivity check (after face edge removal) */
        std::vector<char> visited(n + 1, 0);
        std::vector<int> queue;
        queue.push_back(1);
        visited[1] = 1;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int v = queue[qi];
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (!visited[u] && !face_edge[v].count(u)) {
                    visited[u] = 1;
                    queue.push_back(u);
                }
            }
        }
        if ((int)queue.size() != n) continue;

        res.is_halin = true;
        return res;
    }

    return res;
}

} // namespace graph_recognition

#endif
