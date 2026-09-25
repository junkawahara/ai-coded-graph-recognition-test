#ifndef GRAPH_RECOGNITION_QUADRANGULATION_EXPANSION_H
#define GRAPH_RECOGNITION_QUADRANGULATION_EXPANSION_H

#include <set>
#include "util/plane_graph.h"

namespace graph_recognition {
namespace detail {

// Brinkmann--Greenberg--Greenhill--McKay--Thomas--Wollan, Generation of
// simple quadrangulations of the sphere, Theorem 3 and Figure 3 (2005).
// All helpers take simple sphere quadrangulations, with zero-based labels.
// Reflection is identified by the shared embedded-graph canonicalizer.

// An alternating cycle with a hub on each side; n is even and at least 8.
inline PlaneGraph quadrangulation_pseudo_double_wheel(int n) {
    PlaneGraph g;
    g.rotation.resize(n);
    const int rim = n - 2;
    for (int v = 0; v < rim; ++v) {
        const int prev = (v + rim - 1) % rim, next = (v + 1) % rim;
        if (v % 2 == 0) g.rotation[v] = {prev, rim, next};
        else g.rotation[v] = {prev, next, rim + 1};
    }
    for (int v = rim - 2; v >= 0; v -= 2) g.rotation[rim].push_back(v);
    for (int v = 1; v < rim; v += 2) g.rotation[rim + 1].push_back(v);
    return g;
}

inline int quadrangulation_successor(const PlaneGraph& g, int v, int incoming) {
    const auto& row = g.rotation[v];
    const auto pos = std::find(row.begin(), row.end(), incoming);
    if (pos == row.end()) return -1;
    return row[(pos - row.begin() + 1) % row.size()];
}

// P1 splits off the middle edge in the consecutive arc (u,t,w) at v.
// Theorem 3 permits this precisely when {u,v,w} is not a 3-cut, and v
// must have degree >= 4 so that both split vertices have degree >= 3.
inline bool quadrangulation_can_expand_p1(const PlaneGraph& g, int v, int first) {
    const int degree = static_cast<int>(g.rotation[v].size());
    if (degree < 4) return false;
    const int u = g.rotation[v][first], w = g.rotation[v][(first + 2) % degree];
    std::vector<char> seen(g.rotation.size(), 0);
    seen[u] = seen[v] = seen[w] = 1;
    int start = 0;
    while (seen[start]) ++start;
    std::vector<int> queue(1, start);
    seen[start] = 1;
    for (std::size_t i = 0; i < queue.size(); ++i) {
        for (int next : g.rotation[queue[i]]) {
            if (!seen[next]) {
                seen[next] = 1;
                queue.push_back(next);
            }
        }
    }
    return queue.size() == g.rotation.size() - 3;
}

// Requires can_expand_p1, except in tests that inspect the excluded cases.
inline PlaneGraph quadrangulation_expand_p1(const PlaneGraph& g, int v, int first) {
    const int n = static_cast<int>(g.rotation.size());
    const int degree = static_cast<int>(g.rotation[v].size());
    const int u = g.rotation[v][first];
    const int t = g.rotation[v][(first + 1) % degree];
    const int w = g.rotation[v][(first + 2) % degree];
    PlaneGraph child = g;
    child.rotation.push_back({u, t, w});
    auto& at_v = child.rotation[v];
    at_v.erase(std::find(at_v.begin(), at_v.end(), t));
    auto& at_t = child.rotation[t];
    *std::find(at_t.begin(), at_t.end(), v) = n;
    auto& at_u = child.rotation[u];
    at_u.insert(std::find(at_u.begin(), at_u.end(), v), n);
    auto& at_w = child.rotation[w];
    at_w.insert(std::find(at_w.begin(), at_w.end(), v) + 1, n);
    return child;
}

// P3 inserts an inner square and four spokes in any face, including the
// exterior face. It always preserves simplicity and 3-connectivity.
inline PlaneGraph quadrangulation_expand_p3(const PlaneGraph& g,
                                           const std::vector<int>& face) {
    const int n = static_cast<int>(g.rotation.size());
    PlaneGraph child = g;
    child.rotation.resize(n + 4);
    for (int i = 0; i < 4; ++i) {
        auto& row = child.rotation[face[i]];
        row.insert(std::find(row.begin(), row.end(), face[(i + 1) % 4]), n + i);
        child.rotation[n + i] = {face[i], n + (i + 3) % 4, n + (i + 1) % 4};
    }
    return child;
}

// In a simple sphere quadrangulation, two faces meet improperly iff they
// share an opposite vertex pair. Thus uniqueness of the opposite pairs
// is exactly 3-connectivity (the polyhedral-map characterization). This
// also rejects reductions that preserve minimum degree but create a 2-cut.
inline bool quadrangulation_is_polyhedral(const PlaneGraph& g) {
    if (g.rotation.size() < 8) return false;
    for (const auto& row : g.rotation) if (row.size() < 3) return false;
    const auto faces = plane_graph_faces(g);
    std::set<std::pair<int, int> > opposite;
    for (const auto& face : faces.boundary) {
        for (int i = 0; i < 2; ++i) {
            const int a = face[i], b = face[i + 2];
            if (!opposite.insert(std::minmax(a, b)).second) return false;
        }
    }
    return true;
}

// Delete marked vertices and close the label gaps, keeping all rotations.
inline PlaneGraph quadrangulation_remove_vertices(
    const PlaneGraph& g, const std::vector<char>& removed) {
    std::vector<int> labels(g.rotation.size(), -1);
    int n = 0;
    for (std::size_t v = 0; v < labels.size(); ++v)
        if (!removed[v]) labels[v] = n++;
    PlaneGraph reduced;
    reduced.rotation.resize(n);
    for (std::size_t v = 0; v < labels.size(); ++v) {
        if (removed[v]) continue;
        for (int w : g.rotation[v])
            if (!removed[w]) reduced.rotation[labels[v]].push_back(labels[w]);
    }
    return reduced;
}

// Contract (x,u,v,w) at {x,v}, where x has degree 3. The third neighbour
// t of x must not already be adjacent to v, and u,w must retain degree 3.
inline bool quadrangulation_reduce_p1(const PlaneGraph& g, int x, int first,
                                      PlaneGraph& parent) {
    if (g.rotation.size() <= 8 || g.rotation[x].size() != 3) return false;
    const int u = g.rotation[x][first];
    const int t = g.rotation[x][(first + 1) % 3];
    const int w = g.rotation[x][(first + 2) % 3];
    if (g.rotation[u].size() < 4 || g.rotation[w].size() < 4) return false;
    const int v = quadrangulation_successor(g, u, x);
    const auto& at_v = g.rotation[v];
    if (std::find(at_v.begin(), at_v.end(), t) != at_v.end()) return false;
    PlaneGraph reduced = g;
    auto& row_v = reduced.rotation[v];
    row_v.insert(std::find(row_v.begin(), row_v.end(), u) + 1, t);
    auto& row_t = reduced.rotation[t];
    *std::find(row_t.begin(), row_t.end(), x) = v;
    std::vector<char> removed(g.rotation.size(), 0);
    removed[x] = 1;
    parent = quadrangulation_remove_vertices(reduced, removed);
    return quadrangulation_is_polyhedral(parent);
}

// Remove a facial square of degree-3 vertices, with four distinct outer
// neighbours of degree >= 4 and the four side faces required by Figure 3.
inline bool quadrangulation_reduce_p3(const PlaneGraph& g,
    const std::vector<int>& face, PlaneGraph& parent) {
    if (g.rotation.size() < 12) return false;
    std::vector<int> outer;
    for (int i = 0; i < 4; ++i) {
        if (g.rotation[face[i]].size() != 3) return false;
        for (int v : g.rotation[face[i]]) {
            if (std::find(face.begin(), face.end(), v) == face.end()) {
                if (g.rotation[v].size() < 4) return false;
                outer.push_back(v);
            }
        }
    }
    if (outer.size() != 4 || std::set<int>(outer.begin(), outer.end()).size() != 4)
        return false;
    for (int i = 0; i < 4; ++i) {
        if (quadrangulation_successor(g, outer[i], face[i]) != outer[(i + 1) % 4] ||
            quadrangulation_successor(g, outer[i], outer[(i + 3) % 4]) != face[i])
            return false;
    }
    std::vector<char> removed(g.rotation.size(), 0);
    for (int v : face) removed[v] = 1;
    parent = quadrangulation_remove_vertices(g, removed);
    return quadrangulation_is_polyhedral(parent);
}

// An invariant parent class: try P1, then P3, in the canonical embedding's
// order. Theorem 3 guarantees a reduction except at a pseudo-double wheel.
// This tie-break is ours, not plantri's optimized reduction/orbit machinery.
inline std::vector<int> quadrangulation_parent_code(const PlaneGraph& canonical) {
    PlaneGraph parent;
    for (int v = 0; v < static_cast<int>(canonical.rotation.size()); ++v) {
        if (canonical.rotation[v].size() != 3) continue;
        for (int first = 0; first < 3; ++first)
            if (quadrangulation_reduce_p1(canonical, v, first, parent))
                return plane_graph_canonicalize(parent).code;
    }
    for (const auto& face : plane_graph_faces(canonical).boundary)
        if (quadrangulation_reduce_p3(canonical, face, parent))
            return plane_graph_canonicalize(parent).code;
    return std::vector<int>();
}

// One parent class per child and a sibling set give a canonical construction
// path without a global visited set. Whitney uniqueness identifies abstract
// isomorphism with embedding isomorphism (including reflection) in Q3.
// O(k) candidate patches at order k; canonical parent selection scans O(k)
// reductions with O(k^2) facial checks each. Canonicalization is O(k^2).
// Along a depth-first path, rotations, face tables and sibling codes use
// O(n^3) memory; the public API's retained outputs are an additional cost.
template <typename Callback>
inline void quadrangulation_expansion_dfs(const PlaneGraphCanon& parent,
                                          int n, Callback& emit) {
    const int k = static_cast<int>(parent.graph.rotation.size());
    if (k == n) {
        emit(parent);
        return;
    }
    std::set<std::vector<int> > children;
    auto visit = [&](const PlaneGraph& expanded) {
        const auto child = plane_graph_canonicalize(expanded);
        if (!children.insert(child.code).second) return;
        if (quadrangulation_parent_code(child.graph) != parent.code) return;
        quadrangulation_expansion_dfs(child, n, emit);
    };
    for (int v = 0; v < k; ++v) {
        if (parent.graph.rotation[v].size() < 4) continue;
        for (int first = 0; first < static_cast<int>(parent.graph.rotation[v].size()); ++first)
            if (quadrangulation_can_expand_p1(parent.graph, v, first))
                visit(quadrangulation_expand_p1(parent.graph, v, first));
    }
    if (n - k >= 4)
        for (const auto& face : plane_graph_faces(parent.graph).boundary)
            visit(quadrangulation_expand_p3(parent.graph, face));
}

template <typename Callback>
inline void quadrangulation_expansion_enumerate(int n, Callback emit) {
    // The cube alone is insufficient: every even-order pseudo-double wheel
    // is an irreducible root, and P1 also generates odd orders (e.g. n=11).
    for (int k = 8; k <= n; k += 2) {
        quadrangulation_expansion_dfs(
            plane_graph_canonicalize(quadrangulation_pseudo_double_wheel(k)), n, emit);
        if (n - k < 2) break;
    }
}

}  // namespace detail
}  // namespace graph_recognition

#endif
