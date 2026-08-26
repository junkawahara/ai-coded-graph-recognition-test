#ifndef GRAPH_RECOGNITION_FULLERENE_H
#define GRAPH_RECOGNITION_FULLERENE_H

/**
 * @file fullerene.h
 * @brief Fullerene graph recognition
 *
 * A fullerene is a cubic (3-regular) 3-connected planar graph where every face
 * is a pentagon (5-gon) or hexagon (6-gon).
 * n must be even, n >= 20, and n != 22.
 */

#include "util/graph.h"
#include "recognizers/planar.h"
#include "recognizers/triconnected.h"
#include "decompositions/planar_embedding.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for fullerene recognition
 */
enum class FullereneAlgorithm {
    FACE_CHECK /**< Face size check on planar embedding */
};

/**
 * @brief Result of fullerene recognition
 */
struct FullereneResult {
    bool is_fullerene = false; /**< true if the graph is a fullerene */
};

/**
 * @brief Determines whether the graph is a fullerene
 * @param g Input graph
 * @param algo Algorithm to use (default: FACE_CHECK)
 * @return FullereneResult
 *
 * Fullerene iff cubic AND 3-connected AND planar AND all faces are pentagons or hexagons.
 * Necessary conditions: n is even, n >= 20, n != 22.
 */
inline FullereneResult check_fullerene(const Graph& g,
    FullereneAlgorithm algo = FullereneAlgorithm::FACE_CHECK) {
    (void)algo;
    FullereneResult res;

    int n = g.n;

    /* Necessary conditions: n is even, n >= 20, n != 22 */
    if (n < 20) return res;
    if (n % 2 != 0) return res;
    if (n == 22) return res;

    /* Cubic (3-regular) check */
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() != 3) return res;
    }

    /* Edge count check: m = 3n/2 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != 3LL * n / 2) return res;

    /* Connectivity check */
    {
        std::vector<char> visited(n + 1, 0);
        std::vector<int> queue;
        queue.push_back(1);
        visited[1] = 1;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int v = queue[qi];
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (!visited[u]) {
                    visited[u] = 1;
                    queue.push_back(u);
                }
            }
        }
        if ((int)queue.size() != n) return res;
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

    /* Face count check: Euler formula f = m - n + 2 = 3n/2 - n + 2 = n/2 + 2 */
    int expected_faces = n / 2 + 2;
    if ((int)emb.faces.size() != expected_faces) return res;

    /* Verify all faces are pentagons (5) or hexagons (6) */
    int pentagons = 0, hexagons = 0;
    for (size_t i = 0; i < emb.faces.size(); ++i) {
        int fsize = (int)emb.faces[i].size();
        if (fsize == 5) {
            ++pentagons;
        } else if (fsize == 6) {
            ++hexagons;
        } else {
            return res; /* Face is neither a pentagon nor a hexagon */
        }
    }

    /* Verify: exactly 12 pentagons */
    if (pentagons != 12) return res;
    /* Verify: hexagons = n/2 - 10 */
    if (hexagons != n / 2 - 10) return res;

    res.is_fullerene = true;
    return res;
}

} // namespace graph_recognition

#endif
