#ifndef GRAPH_RECOGNITION_POLYHEDRAL_EDGE_ENUM_H
#define GRAPH_RECOGNITION_POLYHEDRAL_EDGE_ENUM_H

#include <set>
#include <stdexcept>
#include "util/maximal_planar_expansion.h"

namespace graph_recognition {
namespace detail {

// Retain the existing helper names while sharing facial walks with the
// general planar generator (whose boundaries may repeat vertices).
using PolyhedralFaces = PlaneGraphFaces;

inline PolyhedralFaces polyhedral_faces(const PlaneGraph& g) {
    return plane_graph_faces(g);
}

// Exact deletion condition for an existing edge of a polyhedral embedding.
// Its two incident faces share just that edge, so their union after deletion
// has a simple boundary. All unchanged pairs of faces still meet properly.
// The result is 3-connected iff the merged face meets every other face in
// at most one vertex or a single common boundary edge (the polyhedral-map
// characterization). Minimum degree alone would miss separating vertex pairs.
// Scanning all face boundaries costs O(n), given the face table.
inline bool polyhedral_can_delete_edge(
    const PlaneGraph& g, const PolyhedralFaces& faces, int u, int v) {
    if (g.rotation[u].size() <= 3 || g.rotation[v].size() <= 3) return false;
    const int a = faces.left[u][v], b = faces.left[v][u];
    std::vector<char> merged(g.rotation.size(), 0);
    for (int w : faces.boundary[a]) merged[w] = 1;
    for (int w : faces.boundary[b]) merged[w] = 1;
    for (int f = 0; f < static_cast<int>(faces.boundary.size()); ++f) {
        if (f == a || f == b) continue;
        int first = -1, second = -1;
        for (int w : faces.boundary[f]) {
            if (!merged[w]) continue;
            if (first < 0) first = w;
            else if (second < 0) second = w;
            else return false;
        }
        if (second < 0) continue;
        const int left = faces.left[first][second];
        const int right = faces.left[second][first];
        if (!((left == f && (right == a || right == b)) ||
              (right == f && (left == a || left == b)))) return false;
    }
    return true;
}

inline PlaneGraph polyhedral_delete_edge(const PlaneGraph& g, int u, int v) {
    return plane_graph_delete_edge(g, u, v);
}

inline PlaneGraph polyhedral_edge_parent(const PlaneGraph& canonical) {
    return plane_graph_edge_parent(canonical);
}

// Reverse the canonical ear addition. Deleting only edges incident with a
// triangle suffices, since every parent step adds exactly such an edge.
// One parent class plus sibling codes removes duplicates even when a graph
// has several non-isomorphic triangulations. No global visited set is needed.
// O(n) candidates per output, O(n^2) work per candidate, O(n^3) stack/sibling
// storage; stored output and label-expansion orbits are separate costs.
template <typename Callback>
inline void polyhedral_edge_dfs(const PlaneGraphCanon& parent, Callback& emit) {
    emit(plane_graph_edges(parent.graph));
    const PolyhedralFaces faces = polyhedral_faces(parent.graph);
    std::set<std::vector<int> > children;
    for (int u = 0; u < static_cast<int>(parent.graph.rotation.size()); ++u) {
        for (int v : parent.graph.rotation[u]) {
            if (v <= u) continue;
            if (faces.boundary[faces.left[u][v]].size() != 3 &&
                faces.boundary[faces.left[v][u]].size() != 3) continue;
            if (!polyhedral_can_delete_edge(parent.graph, faces, u, v)) continue;
            const PlaneGraphCanon child = plane_graph_canonicalize(
                polyhedral_delete_edge(parent.graph, u, v));
            if (!children.insert(child.code).second) continue;
            if (plane_graph_canonicalize(polyhedral_edge_parent(child.graph)).code != parent.code)
                continue;
            polyhedral_edge_dfs(child, emit);
        }
    }
}

template <typename Callback>
inline void polyhedral_edge_enumerate(int n, Callback emit) {
    if (n < 4) return;
    auto visit_triangulation = [&](const PlaneTriangulationCanon& root) {
        polyhedral_edge_dfs(root, emit);
    };
    maximal_planar_embedded_expansion_dfs(
        plane_triangulation_canonicalize(plane_triangulation_tetrahedron()),
        n, visit_triangulation);
}

}  // namespace detail
}  // namespace graph_recognition

#endif
