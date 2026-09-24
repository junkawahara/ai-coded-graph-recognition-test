#ifndef GRAPH_RECOGNITION_MAXIMAL_PLANAR_EXPANSION_H
#define GRAPH_RECOGNITION_MAXIMAL_PLANAR_EXPANSION_H

#include <set>
#include "util/plane_triangulation.h"

namespace graph_recognition {
namespace detail {

// Canonical construction path over triangulations. A child is accepted
// exactly at its canonical parent's class, once per parent. Keeping sibling
// codes replaces plantri's orbit pruning; no global set of graphs is needed.
// There are O(k) candidate patches at order k and O(k^2) work per candidate
// (rooted-edge canonicalizations). Copies and sibling sets along the depth
// first path take O(n^3) space; collected output/label expansion is separate.
template <typename Callback>
inline void maximal_planar_embedded_expansion_dfs(
    const PlaneTriangulationCanon& parent, int n, Callback& emit) {
    const int k = static_cast<int>(parent.graph.rotation.size());
    if (k == n) {
        emit(parent);
        return;
    }
    std::set<std::vector<int> > children;
    for (int degree = 3; degree <= 5; ++degree) {
        for (int v = 0; v < k; ++v) {
            const int old_degree = static_cast<int>(parent.graph.rotation[v].size());
            if (old_degree < degree - 1) continue;
            for (int first = 0; first < old_degree; ++first) {
                PlaneTriangulationCanon child = plane_triangulation_canonicalize(
                    plane_triangulation_expand(parent.graph, v, first, degree));
                if (!children.insert(child.code).second) continue;
                if (plane_triangulation_parent_code(child.graph) != parent.code) continue;
                maximal_planar_embedded_expansion_dfs(child, n, emit);
            }
        }
    }
}

// The public enumerators check n first (at most 64, also bounding recursion).
// Repository convention: one empty graph, K1, K2 and K3 for n = 0..3.
template <typename Callback>
inline void maximal_planar_expansion_enumerate(int n, Callback emit) {
    if (n < 4) {
        std::vector<std::pair<int, int> > edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                edges.push_back(std::make_pair(u, v));
        emit(edges);
        return;
    }
    auto emit_edges = [&](const PlaneTriangulationCanon& g) {
        emit(plane_triangulation_edges(g.graph));
    };
    maximal_planar_embedded_expansion_dfs(
        plane_triangulation_canonicalize(plane_triangulation_tetrahedron()), n, emit_edges);
}

}  // namespace detail
}  // namespace graph_recognition

#endif
