#ifndef GRAPH_RECOGNITION_CUBIC_PLANAR_DUAL_ENUM_H
#define GRAPH_RECOGNITION_CUBIC_PLANAR_DUAL_ENUM_H

#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "util/maximal_planar_expansion.h"

namespace graph_recognition {
namespace detail {

// A sphere triangulation with parallel edges and loops. Darts 2e and 2e+1
// are twins; next walks a triangular face. Unlike neighbour-only rotations,
// this representation distinguishes every incidence of a multiple edge/loop.
struct CubicPlanarTriangulation {
    std::vector<int> next;
};

inline CubicPlanarTriangulation cubic_planar_triangulation(
    const PlaneTriangulation& simple) {
    const int n = static_cast<int>(simple.rotation.size());
    std::vector<std::vector<int> > dart(n, std::vector<int>(n, -1));
    CubicPlanarTriangulation g;
    for (const auto& edge : plane_triangulation_edges(simple)) {
        const int u = edge.first - 1, v = edge.second - 1;
        dart[u][v] = static_cast<int>(g.next.size());
        dart[v][u] = dart[u][v] + 1;
        g.next.resize(g.next.size() + 2);
    }
    for (int v = 0; v < n; ++v) {
        const auto& row = simple.rotation[v];
        for (std::size_t i = 0; i < row.size(); ++i)
            g.next[dart[row[i]][v]] = dart[v][row[(i + 1) % row.size()]];
    }
    return g;
}

struct CubicPlanarTriangulationVertices {
    std::vector<int> origin;
    std::vector<int> degree;
};

inline CubicPlanarTriangulationVertices cubic_planar_triangulation_vertices(
    const CubicPlanarTriangulation& g) {
    CubicPlanarTriangulationVertices result;
    result.origin.assign(g.next.size(), -1);
    for (int d = 0; d < static_cast<int>(g.next.size()); ++d) {
        if (result.origin[d] >= 0) continue;
        const int v = static_cast<int>(result.degree.size());
        result.degree.push_back(0);
        int at = d;
        do {
            result.origin[at] = v;
            ++result.degree[v];
            at = g.next[at ^ 1];
        } while (at != d);
    }
    return result;
}

// Rooted-dart BFS over twin and face successor, in both orientations.
// Its code reconstructs both permutations, so it identifies embeddings
// (including reflection), even with loops/multiple edges. O(n^2) time.
inline std::vector<int> cubic_planar_triangulation_code(
    const CubicPlanarTriangulation& g) {
    const int darts = static_cast<int>(g.next.size());
    std::vector<int> best;
    for (int root = 0; root < darts; ++root) {
        for (int mirror = 0; mirror < 2; ++mirror) {
            std::vector<int> labels(darts, -1), order(1, root), code;
            labels[root] = 0;
            bool smaller = best.empty(), worse = false;
            for (int i = 0; i < darts && !worse; ++i) {
                const int d = order[i];
                const int adjacent[] = {d ^ 1, mirror ? g.next[g.next[d]] : g.next[d]};
                for (int at : adjacent) {
                    if (labels[at] < 0) {
                        labels[at] = static_cast<int>(order.size());
                        order.push_back(at);
                    }
                    const int entry = labels[at];
                    if (!smaller) {
                        if (entry > best[code.size()]) { worse = true; break; }
                        smaller = entry < best[code.size()];
                    }
                    code.push_back(entry);
                }
            }
            if (!worse && smaller) best = std::move(code);
        }
    }
    return best;
}

// Replace the diagonal shared by two distinct triangular faces. The six
// darts must be distinct, but their origins may coincide: that is exactly
// how Figure 5 of Brinkmann--McKay (2007), section 2, creates loops.
inline CubicPlanarTriangulation cubic_planar_triangulation_flip(
    const CubicPlanarTriangulation& g, int d) {
    const int e = d ^ 1, a = g.next[d], b = g.next[a];
    const int c = g.next[e], f = g.next[c];
    CubicPlanarTriangulation child = g;
    child.next[d] = f; child.next[f] = a; child.next[a] = d;
    child.next[e] = b; child.next[b] = c; child.next[c] = e;
    return child;
}

// Each face becomes a cubic vertex. Reject loops and parallel edges in
// the DUAL, including non-facial 2-cycles that minimum primal degree 3
// alone does not exclude. Rejected maps still participate in the search.
inline bool cubic_planar_simple_dual(const CubicPlanarTriangulation& g,
    std::vector<std::pair<int, int> >& edges) {
    std::vector<int> face(g.next.size(), -1);
    int count = 0;
    for (int d = 0; d < static_cast<int>(g.next.size()); ++d) {
        if (face[d] >= 0) continue;
        face[d] = face[g.next[d]] = face[g.next[g.next[d]]] = count++;
    }
    edges.clear();
    for (int d = 0; d < static_cast<int>(g.next.size()); d += 2) {
        int u = face[d] + 1, v = face[d ^ 1] + 1;
        if (u == v) return false;
        if (u > v) std::swap(u, v);
        edges.push_back(std::make_pair(u, v));
    }
    std::sort(edges.begin(), edges.end());
    return std::adjacent_find(edges.begin(), edges.end()) == edges.end();
}

// Section 2 / Figure 5: start with all simple triangulations on n/2+2
// vertices. First flip a single edge onto an existing edge (increasing
// parallel-edge excess). Then flip non-loop edges whose opposite corners
// coincide (increasing the number of loops). Both phases retain minimum
// degree 3. The reduction argument in the paper guarantees coverage of
// all minimum-degree-3 triangulations. Sets replace plantri's canonical
// reduction/orbit pruning; no planarity or cubic recognizer is called.
template <typename Callback>
inline void cubic_planar_triangulations_enumerate(int n, Callback emit) {
    std::deque<CubicPlanarTriangulation> pending;
    std::set<std::vector<int> > seen;
    auto enqueue = [&](CubicPlanarTriangulation g) {
        if (seen.insert(cubic_planar_triangulation_code(g)).second)
            pending.push_back(std::move(g));
    };
    auto root = [&](const PlaneTriangulationCanon& g) {
        enqueue(cubic_planar_triangulation(g.graph));
    };
    maximal_planar_embedded_expansion_dfs(
        plane_triangulation_canonicalize(plane_triangulation_tetrahedron()), n / 2 + 2, root);
    while (!pending.empty()) {
        CubicPlanarTriangulation g = std::move(pending.front());
        pending.pop_front();
        emit(g);
        const auto vertices = cubic_planar_triangulation_vertices(g);
        const auto& origin = vertices.origin;
        std::map<std::pair<int, int>, int> multiplicity;
        bool has_loop = false;
        for (int d = 0; d < static_cast<int>(g.next.size()); d += 2) {
            const int u = origin[d], v = origin[d ^ 1];
            ++multiplicity[std::minmax(u, v)];
            if (u == v) has_loop = true;
        }
        for (int d = 0; d < static_cast<int>(g.next.size()); d += 2) {
            const int e = d ^ 1, a = g.next[d], b = g.next[a];
            if (a == e || b == e) continue;  // Same face on both sides.
            const int u = origin[d], v = origin[e];
            if (u == v) continue;  // Never remove a loop.
            const int x = origin[b], y = origin[g.next[g.next[e]]];
            if (x != y && (has_loop || multiplicity[std::minmax(u, v)] != 1 ||
                          !multiplicity.count(std::minmax(x, y)))) continue;
            auto child = cubic_planar_triangulation_flip(g, d);
            const auto after = cubic_planar_triangulation_vertices(child);
            if (after.degree.size() != vertices.degree.size() ||
                *std::min_element(after.degree.begin(), after.degree.end()) < 3) continue;
            enqueue(std::move(child));
        }
    }
}

// Enumerate breadth-first labelings of a connected cubic graph. Only the
// root can have three undiscovered neighbours; every later vertex has at
// most two. Sorted adjacency rows reconstruct the abstract graph, and all
// roots / orders of newly discovered neighbours are tried, so the minimum
// code is a complete isomorphism invariant independent of the embedding.
// Prefix pruning avoids most branches; the worst case is exponential.
inline void cubic_planar_abstract_code_dfs(const std::vector<std::vector<int> >& adj,
    int at, std::vector<int>& labels, std::vector<int>& order,
    std::vector<int>& code, std::vector<int>& best) {
    if (at == static_cast<int>(adj.size())) {
        if (best.empty() || code < best) best = code;
        return;
    }
    const int v = order[at];
    std::vector<int> fresh;
    for (int w : adj[v]) if (labels[w] < 0) fresh.push_back(w);
    std::sort(fresh.begin(), fresh.end());
    const std::size_t previous = order.size();
    do {
        for (int w : fresh) {
            labels[w] = static_cast<int>(order.size());
            order.push_back(w);
        }
        std::vector<int> row;
        for (int w : adj[v]) row.push_back(labels[w]);
        std::sort(row.begin(), row.end());
        code.insert(code.end(), row.begin(), row.end());
        if (best.empty() || !std::lexicographical_compare(
                best.begin(), best.begin() + code.size(), code.begin(), code.end()))
            cubic_planar_abstract_code_dfs(adj, at + 1, labels, order, code, best);
        code.resize(code.size() - row.size());
        for (int w : fresh) labels[w] = -1;
        order.resize(previous);
    } while (std::next_permutation(fresh.begin(), fresh.end()));
}

inline std::vector<int> cubic_planar_abstract_code(int n,
    const std::vector<std::pair<int, int> >& edges) {
    std::vector<std::vector<int> > adj(n);
    for (const auto& edge : edges) {
        adj[edge.first - 1].push_back(edge.second - 1);
        adj[edge.second - 1].push_back(edge.first - 1);
    }
    std::vector<int> best, labels(n, -1), order, code;
    for (int root = 0; root < n; ++root) {
        labels[root] = 0;
        order.push_back(root);
        cubic_planar_abstract_code_dfs(adj, 0, labels, order, code, best);
        order.clear();
        labels[root] = -1;
    }
    return best;
}

inline std::vector<std::vector<std::pair<int, int> > > cubic_planar_connected_duals(int n) {
    std::vector<std::vector<std::pair<int, int> > > result;
    std::set<std::vector<int> > abstract_forms;
    cubic_planar_triangulations_enumerate(n, [&](const CubicPlanarTriangulation& g) {
        std::vector<std::pair<int, int> > edges;
        if (!cubic_planar_simple_dual(g, edges)) return;
        // Below connectivity 3, distinct plane embeddings may describe
        // the same abstract graph. Deduplicate after forgetting the map.
        if (abstract_forms.insert(cubic_planar_abstract_code(n, edges)).second)
            result.push_back(std::move(edges));
    });
    return result;
}

struct CubicPlanarComponent {
    int n;
    std::vector<std::pair<int, int> > edges;
};

template <typename Callback>
inline void cubic_planar_component_multisets(int remaining, int used, std::size_t first,
    const std::vector<CubicPlanarComponent>& components,
    std::vector<std::pair<int, int> >& edges, Callback& emit) {
    if (remaining == 0) { emit(edges); return; }
    for (std::size_t i = first; i < components.size(); ++i) {
        const auto& component = components[i];
        if (component.n > remaining) break;
        const std::size_t previous = edges.size();
        for (const auto& edge : component.edges)
            edges.push_back(std::make_pair(edge.first + used, edge.second + used));
        cubic_planar_component_multisets(remaining - component.n, used + component.n,
                                        i, components, edges, emit);
        edges.resize(previous);
    }
}

// The public entry points bound the search size before calling this helper.
// Hold embedding codes/queue for one component size, connected abstract
// representatives, and component multisets. Abstract canonicalization is
// worst-case exponential; plantri's time/space guarantees do not apply.
template <typename Callback>
inline void cubic_planar_dual_enumerate(int n, bool connected_only, Callback emit) {
    if (n < 4 || n % 2) return;
    if (connected_only) {
        for (const auto& edges : cubic_planar_connected_duals(n)) emit(edges);
        return;
    }
    std::vector<CubicPlanarComponent> components;
    for (int size = 4; size <= n; size += 2) {
        if (size != n && n - size < 4) continue;
        for (auto& edges : cubic_planar_connected_duals(size))
            components.push_back(CubicPlanarComponent{size, std::move(edges)});
    }
    std::vector<std::pair<int, int> > edges;
    cubic_planar_component_multisets(n, 0, 0, components, edges, emit);
}

}  // namespace detail
}  // namespace graph_recognition

#endif
