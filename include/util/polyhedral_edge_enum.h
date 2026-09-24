#ifndef GRAPH_RECOGNITION_POLYHEDRAL_EDGE_ENUM_H
#define GRAPH_RECOGNITION_POLYHEDRAL_EDGE_ENUM_H

#include <set>
#include <stdexcept>
#include "util/maximal_planar_expansion.h"

namespace graph_recognition {
namespace detail {

struct PolyhedralFaces {
    std::vector<std::vector<int> > boundary;
    // Face to the left of each directed edge; -1 for nonedges.
    std::vector<std::vector<int> > left;
};

// Walk all faces, including the exterior face. Taking the successor of
// the incoming neighbour in the cyclic row fixes a consistent orientation.
inline PolyhedralFaces polyhedral_faces(const PlaneGraph& g) {
    const int n = static_cast<int>(g.rotation.size());
    PolyhedralFaces faces;
    faces.left.assign(n, std::vector<int>(n, -1));
    for (int u = 0; u < n; ++u) {
        for (int v : g.rotation[u]) {
            if (faces.left[u][v] >= 0) continue;
            const int id = static_cast<int>(faces.boundary.size());
            std::vector<int> boundary;
            int a = u, b = v;
            do {
                faces.left[a][b] = id;
                boundary.push_back(a);
                const auto& row = g.rotation[b];
                const std::size_t pos = std::find(row.begin(), row.end(), a) - row.begin();
                a = b;
                b = row[(pos + 1) % row.size()];
            } while (a != u || b != v);
            faces.boundary.push_back(std::move(boundary));
        }
    }
    return faces;
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
    PlaneGraph child = g;
    auto& at_u = child.rotation[u];
    auto& at_v = child.rotation[v];
    at_u.erase(std::find(at_u.begin(), at_u.end(), v));
    at_v.erase(std::find(at_v.begin(), at_v.end(), u));
    return child;
}

// On the canonical embedding choose the first missing ear diagonal a--c
// of a nontriangular face (...,a,b,c,...). Such an ear always exists
// (Brinkmann--McKay 2007, section 4). Adding it preserves 3-connectivity
// and simplicity, and repeated parent steps reach an n-vertex triangulation.
// This deterministic choice is specific to this implementation.
inline PlaneGraph polyhedral_edge_parent(const PlaneGraph& canonical) {
    const PolyhedralFaces faces = polyhedral_faces(canonical);
    for (const auto& face : faces.boundary) {
        if (face.size() < 4) continue;
        for (std::size_t i = 0; i < face.size(); ++i) {
            const int a = face[i], c = face[(i + 2) % face.size()];
            if (faces.left[a][c] >= 0) continue;
            PlaneGraph parent = canonical;
            auto& at_a = parent.rotation[a];
            auto& at_c = parent.rotation[c];
            at_a.insert(std::find(at_a.begin(), at_a.end(), face[(i + 1) % face.size()]), c);
            at_c.insert(std::find(at_c.begin(), at_c.end(), face[(i + 3) % face.size()]), a);
            return parent;
        }
    }
    throw std::logic_error("nontriangular polyhedral embedding has no missing ear diagonal");
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
