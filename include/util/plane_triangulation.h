#ifndef GRAPH_RECOGNITION_PLANE_TRIANGULATION_H
#define GRAPH_RECOGNITION_PLANE_TRIANGULATION_H

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "util/plane_graph.h"

namespace graph_recognition {
namespace detail {

// A simple sphere triangulation on at least four vertices. Entries are
// zero-based neighbours in cyclic order; reversing EVERY row mirrors it.
using PlaneTriangulation = PlaneGraph;

inline PlaneTriangulation plane_triangulation_tetrahedron() {
    PlaneTriangulation g;
    g.rotation = {{1, 2, 3}, {0, 3, 2}, {0, 1, 3}, {0, 2, 1}};
    return g;
}

// Retain the triangulation helpers while sharing rotation storage and
// canonicalization with the polyhedral edge-deletion search.
using PlaneTriangulationCanon = PlaneGraphCanon;

inline std::vector<std::pair<int, int> > plane_triangulation_edges(
    const PlaneTriangulation& g) {
    return plane_graph_edges(g);
}

inline PlaneTriangulationCanon plane_triangulation_canonicalize(
    const PlaneTriangulation& g) {
    return plane_graph_canonicalize(g);
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
