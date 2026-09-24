#ifndef GRAPH_RECOGNITION_PLANE_TRIANGULATION_H
#define GRAPH_RECOGNITION_PLANE_TRIANGULATION_H

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace detail {

// A simple sphere triangulation on at least four vertices. Entries are
// zero-based neighbours in cyclic order; reversing EVERY row mirrors it.
struct PlaneTriangulation {
    std::vector<std::vector<int> > rotation;
};

inline PlaneTriangulation plane_triangulation_tetrahedron() {
    PlaneTriangulation g;
    g.rotation = {{1, 2, 3}, {0, 3, 2}, {0, 1, 3}, {0, 2, 1}};
    return g;
}

inline std::vector<std::pair<int, int> > plane_triangulation_edges(
    const PlaneTriangulation& g) {
    std::vector<std::pair<int, int> > edges;
    for (int v = 0; v < static_cast<int>(g.rotation.size()); ++v)
        for (int w : g.rotation[v])
            if (v < w) edges.push_back(std::make_pair(v + 1, w + 1));
    std::sort(edges.begin(), edges.end());
    return edges;
}

struct PlaneTriangulationCanon {
    PlaneTriangulation graph;
    std::vector<int> code;
};

// Rooted directed-edge BFS, in both orientations (Brinkmann--McKay 2007,
// section 1.3). Neighbours receive labels on discovery; 0 separates rows.
// The code reconstructs the rotation system. At most 4m rooted scans give
// a canonical embedding, including reflection, in O(n^2) time and
// O(n) temporary space. Whitney uniqueness makes this an abstract-graph
// canonical form for simple triangulations. Minimum degree restricts the
// roots invariantly; prefixes worse than the current best are abandoned.
inline PlaneTriangulationCanon plane_triangulation_canonicalize(
    const PlaneTriangulation& g) {
    const int n = static_cast<int>(g.rotation.size());
    PlaneTriangulationCanon best;
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

// E3/E4/E5 of Brinkmann--McKay, Figure 4. The disk consists of degree-2
// consecutive triangular faces at v. Delete its internal edges and join a
// new vertex to its simple boundary (v,a,b[,c[,d]]). The only condition is
// that the degree-1 neighbours in this arc be distinct. All faces, including
// the exterior face, are treated identically. No planarity test is needed.
inline PlaneTriangulation plane_triangulation_expand(
    const PlaneTriangulation& g, int v, int first, int degree) {
    const int n = static_cast<int>(g.rotation.size());
    const int old_degree = static_cast<int>(g.rotation[v].size());
    std::vector<int> arc;
    for (int i = 0; i < degree - 1; ++i)
        arc.push_back(g.rotation[v][(first + i) % old_degree]);
    PlaneTriangulation child = g;
    child.rotation.push_back(std::vector<int>(1, v));
    child.rotation[n].insert(child.rotation[n].end(), arc.begin(), arc.end());

    std::vector<int>& around_v = child.rotation[v];
    around_v.clear();
    around_v.push_back(arc.front());
    around_v.push_back(n);
    around_v.push_back(arc.back());
    for (int i = degree - 1; i < old_degree; ++i)
        around_v.push_back(g.rotation[v][(first + i) % old_degree]);

    for (int i = 0; i < degree - 1; ++i) {
        std::vector<int>& row = child.rotation[arc[i]];
        std::vector<int>::iterator at_v = std::find(row.begin(), row.end(), v);
        if (i == 0) row.insert(at_v, n);
        else if (i == degree - 2) row.insert(at_v + 1, n);
        else *at_v = n;
    }
    return child;
}

// Delete a vertex of degree 3, 4 or 5, triangulating its hole by a fan at
// the chosen neighbour. This is a valid inverse expansion iff every new
// diagonal is absent. Equivalently, contract the edge to the fan centre
// and merge the two pairs of facial edges. Never contract a separating
// triangle into parallel edges.
inline bool plane_triangulation_reduce(
    const PlaneTriangulation& g, int v, int first, PlaneTriangulation& parent) {
    const int n = static_cast<int>(g.rotation.size());
    const int degree = static_cast<int>(g.rotation[v].size());
    if (n <= 4 || degree < 3 || degree > 5) return false;
    std::vector<int> boundary;
    for (int i = 0; i < degree; ++i)
        boundary.push_back(g.rotation[v][(first + i) % degree]);
    const int centre = boundary[0];
    for (int i = 2; i < degree - 1; ++i) {
        const std::vector<int>& row = g.rotation[centre];
        if (std::find(row.begin(), row.end(), boundary[i]) != row.end()) return false;
    }

    parent.rotation.assign(n - 1, std::vector<int>());
    for (int u = 0; u < n; ++u) {
        if (u == v) continue;
        std::vector<int>& row = parent.rotation[u - (u > v)];
        for (int w : g.rotation[u]) {
            if (w != v) row.push_back(w - (w > v));
            else if (u == centre) {
                for (int i = 2; i < degree - 1; ++i)
                    row.push_back(boundary[i] - (boundary[i] > v));
            } else if (u != boundary[1] && u != boundary[degree - 1]) {
                row.push_back(centre - (centre > v));
            }
        }
    }
    return true;
}

// Choose a reduction on the canonical embedding, first by degree, then
// by vertex and fan position. Euler's formula and the simple-triangulation
// reduction theorem guarantee one for every graph larger than K4.
inline std::vector<int> plane_triangulation_parent_code(
    const PlaneTriangulation& canonical) {
    for (int degree = 3; degree <= 5; ++degree) {
        for (int v = 0; v < static_cast<int>(canonical.rotation.size()); ++v) {
            if (static_cast<int>(canonical.rotation[v].size()) != degree) continue;
            for (int first = 0; first < degree; ++first) {
                PlaneTriangulation parent;
                if (plane_triangulation_reduce(canonical, v, first, parent))
                    return plane_triangulation_canonicalize(parent).code;
            }
        }
    }
    throw std::logic_error("simple triangulation has no E3/E4/E5 reduction");
}

}  // namespace detail
}  // namespace graph_recognition

#endif
