#ifndef GRAPH_RECOGNITION_PLANAR_EDGE_ENUM_H
#define GRAPH_RECOGNITION_PLANAR_EDGE_ENUM_H

#include <set>
#include "util/canonical_augmentation.h"
#include "util/maximal_planar_expansion.h"

namespace graph_recognition {
namespace detail {

// Reverse the canonical facial ear addition. Only edges incident with a
// triangle can undo that parent step. Distinct incident faces exclude
// bridges, preserving connectivity while allowing degrees one and two.
// One parent embedding and sibling codes give each plane map exactly once.
// O(n) candidates per map, O(n^2) work per candidate, O(n^3) search storage.
template <typename Callback>
inline void planar_edge_dfs(const PlaneGraphCanon& parent, Callback& emit) {
    emit(parent);
    const PlaneGraphFaces faces = plane_graph_faces(parent.graph);
    std::set<std::vector<int> > children;
    for (int u = 0; u < static_cast<int>(parent.graph.rotation.size()); ++u) {
        for (int v : parent.graph.rotation[u]) {
            if (u >= v) continue;
            const int a = faces.left[u][v], b = faces.left[v][u];
            if (a == b) continue;
            if (faces.boundary[a].size() != 3 && faces.boundary[b].size() != 3) continue;
            const auto child = plane_graph_canonicalize(
                plane_graph_delete_edge(parent.graph, u, v));
            if (!children.insert(child.code).second) continue;
            if (plane_graph_canonicalize(plane_graph_edge_parent(child.graph)).code != parent.code)
                continue;
            planar_edge_dfs(child, emit);
        }
    }
}

// Connected simple plane maps, including mirror identification. The
// public entry points validate n; this internal helper requires n >= 1.
template <typename Callback>
inline void planar_connected_embedded_enumerate(int n, Callback emit) {
    if (n == 1) {
        PlaneGraphCanon singleton;
        singleton.graph.rotation.resize(1);
        singleton.code.push_back(0);
        emit(singleton);
        return;
    }
    if (n <= 3) {
        PlaneGraph complete;
        complete.rotation.resize(n);
        for (int u = 0; u < n; ++u)
            for (int v = 0; v < n; ++v)
                if (u != v) complete.rotation[u].push_back(v);
        planar_edge_dfs(plane_graph_canonicalize(complete), emit);
        return;
    }
    auto visit_triangulation = [&](const PlaneTriangulationCanon& root) {
        planar_edge_dfs(root, emit);
    };
    maximal_planar_embedded_expansion_dfs(
        plane_triangulation_canonicalize(plane_triangulation_tetrahedron()),
        n, visit_triangulation);
}

// Forget the embedding ONLY at output: different embeddings of an abstract
// graph can have different descendants in the canonical map search.
// The existing exact canonicalizer has factorial worst-case cost. Its set
// lives for one connected component size; no planarity recognition is used.
template <typename Callback>
inline void planar_connected_edge_enumerate(int n, Callback emit) {
    std::set<std::vector<unsigned long long> > abstract_forms;
    planar_connected_embedded_enumerate(n, [&](const PlaneGraphCanon& map) {
        std::vector<unsigned long long> adj(n, 0);
        for (int u = 0; u < n; ++u)
            for (int v : map.graph.rotation[u]) adj[u] |= 1ULL << v;
        if (abstract_forms.insert(canonicalize_bitmask_graph(n, adj).form).second)
            emit(plane_graph_edges(map.graph));
    });
}

struct PlanarEdgeComponent {
    int n;
    std::vector<std::pair<int, int> > edges;
};

template <typename Callback>
inline void planar_edge_component_multisets(int remaining, int used, std::size_t first,
    const std::vector<PlanarEdgeComponent>& components,
    std::vector<std::pair<int, int> >& edges, Callback& emit) {
    if (remaining == 0) { emit(edges); return; }
    for (std::size_t i = first; i < components.size(); ++i) {
        const auto& component = components[i];
        if (component.n > remaining) break;
        const std::size_t previous = edges.size();
        for (const auto& edge : component.edges)
            edges.push_back(std::make_pair(edge.first + used, edge.second + used));
        // Allow the same component again, but never a smaller index.
        planar_edge_component_multisets(remaining - component.n, used + component.n,
                                       i, components, edges, emit);
        edges.resize(previous);
    }
}

// All abstract planar classes of fixed order, including isolated vertices
// and repeated components. Public APIs bound n to 63 before entering here.
// Store connected representatives below n; stream those of order n.
template <typename Callback>
inline void planar_edge_enumerate(int n, bool connected_only, Callback emit) {
    if (n == 0) {
        emit(std::vector<std::pair<int, int> >());
        return;
    }
    if (!connected_only) {
        std::vector<PlanarEdgeComponent> components;
        for (int size = 1; size < n; ++size) {
            planar_connected_edge_enumerate(size, [&](const std::vector<std::pair<int, int> >& edges) {
                components.push_back(PlanarEdgeComponent{size, edges});
            });
        }
        std::vector<std::pair<int, int> > edges;
        planar_edge_component_multisets(n, 0, 0, components, edges, emit);
    }
    planar_connected_edge_enumerate(n, emit);
}

}  // namespace detail
}  // namespace graph_recognition

#endif
