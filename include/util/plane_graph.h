#ifndef GRAPH_RECOGNITION_PLANE_GRAPH_H
#define GRAPH_RECOGNITION_PLANE_GRAPH_H

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace detail {

// A connected simple sphere embedding. Entries are zero-based neighbours
// in cyclic order; reversing EVERY row mirrors it. The canonicalizer below
// requires at least one edge and identifies embeddings, not arbitrary
// abstract planar graphs (Whitney uniqueness applies when 3-connected).
struct PlaneGraph {
    std::vector<std::vector<int> > rotation;
};

inline std::vector<std::pair<int, int> > plane_graph_edges(
    const PlaneGraph& g) {
    std::vector<std::pair<int, int> > edges;
    for (int v = 0; v < static_cast<int>(g.rotation.size()); ++v)
        for (int w : g.rotation[v])
            if (v < w) edges.push_back(std::make_pair(v + 1, w + 1));
    std::sort(edges.begin(), edges.end());
    return edges;
}

struct PlaneGraphCanon {
    PlaneGraph graph;
    std::vector<int> code;
};

struct PlaneGraphFaces {
    // Facial walks can repeat vertices and traverse bridges in both directions.
    std::vector<std::vector<int> > boundary;
    // Face to the left of each directed edge; -1 for nonedges.
    std::vector<std::vector<int> > left;
};

// Walk all faces, including the exterior face. Taking the successor of
// the incoming neighbour fixes a consistent orientation. Requires an edge.
inline PlaneGraphFaces plane_graph_faces(const PlaneGraph& g) {
    const int n = static_cast<int>(g.rotation.size());
    PlaneGraphFaces faces;
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

inline PlaneGraph plane_graph_delete_edge(const PlaneGraph& g, int u, int v) {
    PlaneGraph child = g;
    auto& at_u = child.rotation[u];
    auto& at_v = child.rotation[v];
    at_u.erase(std::find(at_u.begin(), at_u.end(), v));
    at_v.erase(std::find(at_v.begin(), at_v.end(), u));
    return child;
}

// Every connected simple plane graph on n >= 3 that is not a triangulation
// has a facial walk (...,a,b,c,...) with distinct a,b,c and missing edge a--c
// (Brinkmann--McKay 2007, section 4). Add the first such ear in the canonical
// embedding. This remains valid at cut vertices: a walk can repeat vertices,
// so a == c must be skipped and the two insertion corners must be retained.
// The parent increases the edge count and preserves simplicity/connectivity;
// iteration reaches an n-vertex triangulation. The tie-break is ours.
inline PlaneGraph plane_graph_edge_parent(const PlaneGraph& canonical) {
    const PlaneGraphFaces faces = plane_graph_faces(canonical);
    for (const auto& face : faces.boundary) {
        if (face.size() < 4) continue;
        for (std::size_t i = 0; i < face.size(); ++i) {
            const int a = face[i], c = face[(i + 2) % face.size()];
            if (a == c || faces.left[a][c] >= 0) continue;
            PlaneGraph parent = canonical;
            auto& at_a = parent.rotation[a];
            auto& at_c = parent.rotation[c];
            at_a.insert(std::find(at_a.begin(), at_a.end(), face[(i + 1) % face.size()]), c);
            at_c.insert(std::find(at_c.begin(), at_c.end(), face[(i + 3) % face.size()]), a);
            return parent;
        }
    }
    throw std::logic_error("plane embedding has no missing ear diagonal");
}

// Rooted directed-edge BFS, in both orientations (Brinkmann--McKay 2007,
// section 1.3). Neighbours receive labels on discovery; 0 separates rows.
// The code reconstructs the rotation system. At most 4m rooted scans give
// a canonical embedding, including reflection, in O(n^2) time and
// O(n) temporary space. Whitney uniqueness makes this an abstract-graph
// canonical form for simple 3-connected plane graphs. Minimum degree restricts the
// roots invariantly; prefixes worse than the current best are abandoned.
inline PlaneGraphCanon plane_graph_canonicalize(
    const PlaneGraph& g) {
    const int n = static_cast<int>(g.rotation.size());
    PlaneGraphCanon best;
    std::size_t min_degree = g.rotation[0].size();
    for (const auto& row : g.rotation) min_degree = std::min(min_degree, row.size());
    for (int root = 0; root < n; ++root) {
        if (g.rotation[root].size() != min_degree) continue;
        for (int first = 0; first < static_cast<int>(g.rotation[root].size()); ++first) {
            for (int direction = -1; direction <= 1; direction += 2) {
                std::vector<int> labels(n, -1), anchors(n, 0), order(1, root);
                labels[root] = 0;
                anchors[root] = first;
                std::vector<int> code;
                code.reserve(7 * n);
                bool smaller = best.code.empty(), worse = false;
                for (int i = 0; i < n && !worse; ++i) {
                    const int v = order[i];
                    const int degree = static_cast<int>(g.rotation[v].size());
                    for (int j = 0; j < degree; ++j) {
                        const int pos = (anchors[v] + direction * j + degree) % degree;
                        const int w = g.rotation[v][pos];
                        if (labels[w] < 0) {
                            labels[w] = static_cast<int>(order.size());
                            order.push_back(w);
                            anchors[w] = static_cast<int>(
                                std::find(g.rotation[w].begin(), g.rotation[w].end(), v)
                                - g.rotation[w].begin());
                        }
                        const int entry = labels[w] + 1;
                        if (!smaller) {
                            if (entry > best.code[code.size()]) { worse = true; break; }
                            smaller = entry < best.code[code.size()];
                        }
                        code.push_back(entry);
                    }
                    if (worse) break;
                    if (!smaller) smaller = best.code[code.size()] > 0;
                    code.push_back(0);
                }
                if (!worse && smaller) best.code = std::move(code);
            }
        }
    }
    best.graph.rotation.resize(n);
    int row = 0;
    for (int entry : best.code) {
        if (entry == 0) ++row;
        else best.graph.rotation[row].push_back(entry - 1);
    }
    return best;
}

}  // namespace detail
}  // namespace graph_recognition

#endif
