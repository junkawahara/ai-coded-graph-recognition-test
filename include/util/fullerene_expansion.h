#ifndef GRAPH_RECOGNITION_FULLERENE_EXPANSION_H
#define GRAPH_RECOGNITION_FULLERENE_EXPANSION_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "util/plane_triangulation.h"

namespace graph_recognition {
namespace detail {

// Brinkmann--Goedgebeur--McKay (2012), Figures 2--3 and Theorem 2.1.
// Work entirely with embedded dual fullerenes: simple sphere triangulations
// with twelve degree-5 vertices and all other degrees 6. The code below is
// an independent patch implementation, not a dependency on buckygen.
using FullereneUnlabeledFace = std::array<int, 3>;

inline std::vector<FullereneUnlabeledFace> fullerene_unlabeled_faces(
    const PlaneGraph& graph) {
    std::vector<FullereneUnlabeledFace> result;
    for (const auto& face : plane_graph_faces(graph).boundary) {
        if (face.size() != 3) throw std::logic_error("fullerene dual is not triangulated");
        result.push_back({{face[0], face[1], face[2]}});
    }
    return result;
}

// Reassemble oriented triangles after a disk replacement. Duplicate darts,
// split vertex links and incorrect degrees reject a reduction whose new
// diagonals would conflict with the part of the embedding outside the disk.
inline bool fullerene_unlabeled_from_faces(
    int n, const std::vector<FullereneUnlabeledFace>& faces, PlaneGraph& graph) {
    if (n < 12 || faces.size() != 2 * static_cast<std::size_t>(n) - 4) return false;
    std::vector<std::map<int, int> > next(n);
    for (const auto& face : faces) {
        for (int i = 0; i < 3; ++i) {
            const int a = face[i], b = face[(i + 1) % 3], c = face[(i + 2) % 3];
            if (a < 0 || a >= n || b < 0 || b >= n || c < 0 || c >= n
                || a == b || a == c) return false;
            if (!next[b].insert(std::make_pair(a, c)).second) return false;
        }
    }
    graph.rotation.assign(n, std::vector<int>());
    int pentagons = 0;
    for (int v = 0; v < n; ++v) {
        const std::size_t degree = next[v].size();
        if (degree != 5 && degree != 6) return false;
        pentagons += degree == 5;
        int w = next[v].begin()->first;
        for (std::size_t i = 0; i < degree; ++i) {
            if (std::find(graph.rotation[v].begin(), graph.rotation[v].end(), w)
                != graph.rotation[v].end()) return false;
            graph.rotation[v].push_back(w);
            const auto at = next[v].find(w);
            if (at == next[v].end() || next[w].count(v) == 0) return false;
            w = at->second;
        }
        if (w != graph.rotation[v][0]) return false;
    }
    return pentagons == 12;
}

// C20 has two five-rings between its poles. Three or more rings give the
// irreducible (5,0) nanotubes C30, C40, ...; adding a ring is operation F.
inline PlaneGraph fullerene_unlabeled_nanotube(int rings) {
    const int n = 5 * rings + 2, bottom = n - 1;
    std::vector<FullereneUnlabeledFace> faces;
    for (int i = 0; i < 5; ++i) {
        const int j = (i + 1) % 5;
        faces.push_back({{0, 1 + i, 1 + j}});
        for (int row = 0; row + 1 < rings; ++row) {
            const int a = 1 + 5 * row, b = a + 5;
            faces.push_back({{a + j, a + i, b + i}});
            faces.push_back({{a + j, b + i, b + j}});
        }
        const int last = 1 + 5 * (rings - 1);
        faces.push_back({{bottom, last + j, last + i}});
    }
    PlaneGraph graph;
    if (!fullerene_unlabeled_from_faces(n, faces, graph))
        throw std::logic_error("invalid fullerene nanotube seed");
    return graph;
}

// Truncate a tetrahedron, then triangulate each of its four hexagons with
// a new centre. The twelve old vertices have degree 5 and the centres
// degree 6: this is the dual of the exceptional C28(Td).
inline PlaneGraph fullerene_unlabeled_c28() {
    const PlaneGraph tetrahedron = plane_triangulation_tetrahedron();
    std::vector<std::vector<int> > ends(4, std::vector<int>(4, -1));
    int id = 0;
    for (int v = 0; v < 4; ++v)
        for (int w = 0; w < 4; ++w)
            if (v != w) ends[v][w] = id++;
    std::vector<FullereneUnlabeledFace> faces;
    for (const auto& face : fullerene_unlabeled_faces(tetrahedron)) {
        const int a = face[0], b = face[1], c = face[2];
        const int boundary[] = {ends[a][b], ends[b][a], ends[b][c],
                                ends[c][b], ends[c][a], ends[a][c]};
        for (int i = 0; i < 6; ++i)
            faces.push_back({{id, boundary[i], boundary[(i + 1) % 6]}});
        ++id;
    }
    for (int v = 0; v < 4; ++v) {
        const auto& row = tetrahedron.rotation[v];
        faces.push_back({{ends[v][row[2]], ends[v][row[1]], ends[v][row[0]]}});
    }
    PlaneGraph graph;
    if (!fullerene_unlabeled_from_faces(16, faces, graph))
        throw std::logic_error("invalid fullerene C28 seed");
    return graph;
}

struct FullereneUnlabeledPatch {
    int boundary_count;
    int vertex_count;
    std::vector<FullereneUnlabeledFace> faces;
    // Face traversal with the first two vertices already known at each step.
    std::vector<FullereneUnlabeledFace> walk;
    std::vector<int> required_degree;
};

inline void fullerene_unlabeled_patch_walk(FullereneUnlabeledPatch& patch) {
    patch.required_degree.assign(patch.vertex_count, 0);
    const bool reduction = patch.vertex_count > patch.boundary_count;
    patch.required_degree[0] = patch.required_degree[patch.boundary_count - 1] = reduction ? 6 : 5;
    for (int i = patch.boundary_count; i < patch.vertex_count; ++i)
        patch.required_degree[i] = (i == patch.boundary_count || i == patch.vertex_count - 1) ? 5 : 6;
    std::map<std::pair<int, int>, int> at;
    for (int f = 0; f < static_cast<int>(patch.faces.size()); ++f)
        for (int j = 0; j < 3; ++j)
            at[std::make_pair(patch.faces[f][j], patch.faces[f][(j + 1) % 3])] = f;
    std::vector<int> queue(1, 0);
    std::vector<bool> seen(patch.faces.size(), false);
    seen[0] = true;
    patch.walk.push_back(patch.faces[0]);
    for (std::size_t i = 0; i < queue.size(); ++i) {
        const auto face = patch.faces[queue[i]];
        for (int j = 0; j < 3; ++j) {
            const int a = face[j], b = face[(j + 1) % 3];
            const auto other = at.find(std::make_pair(b, a));
            if (other == at.end() || seen[other->second]) continue;
            seen[other->second] = true;
            queue.push_back(other->second);
            const auto& next = patch.faces[other->second];
            for (int c : next)
                if (c != a && c != b) patch.walk.push_back({{b, a, c}});
        }
    }
    if (queue.size() != patch.faces.size()) throw std::logic_error("disconnected fullerene patch");
}

struct FullereneUnlabeledExpansion {
    FullereneUnlabeledPatch smaller, larger;
};

// A Figure 2 patch is a strip of 2k triangles, all 2k+2 vertices on its
// boundary. The turn sequence is (bottom,top)^(k-1) for L_(k-2), and
// (top,bottom)^(i+1)(bottom,top)^(j+1) for B_(i,j), k=i+j+3.
// Pair consecutive triangles, put a vertex in each quadrilateral, then
// flip the edges between pairs. The new central path has degrees 5,6,...,6,5;
// only the two old degree-5 tips gain a neighbour. Reflections of both
// patterns are matched too (in particular the two handed L expansions).
inline FullereneUnlabeledExpansion fullerene_unlabeled_expansion(int k, int bend = -1) {
    FullereneUnlabeledExpansion expansion;
    auto& small = expansion.smaller;
    auto& large = expansion.larger;
    small.boundary_count = large.boundary_count = 2 * k + 2;
    small.vertex_count = small.boundary_count;
    large.vertex_count = small.vertex_count + k;
    int top = 1, bottom = 2, vertex = 3;
    small.faces.push_back({{0, bottom, top}});
    for (int step = 0; step < 2 * k - 2; ++step) {
        small.faces.push_back({{top, bottom, vertex}});
        const bool add_top = bend < 0 ? (step % 2 == 1)
            : (step < 2 * (bend + 1) ? step % 2 == 0 : step % 2 == 1);
        if (add_top) top = vertex;
        else bottom = vertex;
        ++vertex;
    }
    small.faces.push_back({{top, bottom, vertex}});
    for (int pair = 0; pair < k; ++pair) {
        const auto& a = small.faces[2 * pair];
        const auto& b = small.faces[2 * pair + 1];
        for (int which = 0; which < 2; ++which) {
            const auto& face = which == 0 ? a : b;
            const auto& other = which == 0 ? b : a;
            for (int j = 0; j < 3; ++j) {
                const int u = face[j], v = face[(j + 1) % 3];
                bool shared = false;
                for (int t = 0; t < 3; ++t)
                    if (other[t] == v && other[(t + 1) % 3] == u) shared = true;
                if (!shared) large.faces.push_back({{u, v, small.vertex_count + pair}});
            }
        }
    }
    for (int pair = 0; pair + 1 < k; ++pair) {
        const int p = small.vertex_count + pair, q = p + 1;
        int first = -1, second = -1, u = -1, v = -1;
        for (int f = 0; f < static_cast<int>(large.faces.size()); ++f) {
            if (large.faces[f][2] != p) continue;
            for (int h = 0; h < static_cast<int>(large.faces.size()); ++h) {
                if (large.faces[h][2] == q && large.faces[f][0] == large.faces[h][1]
                    && large.faces[f][1] == large.faces[h][0]) {
                    first = f; second = h; u = large.faces[f][0]; v = large.faces[f][1];
                }
            }
        }
        if (first < 0) throw std::logic_error("invalid fullerene patch interface");
        large.faces[first] = {{p, u, q}};
        large.faces[second] = {{q, v, p}};
    }
    fullerene_unlabeled_patch_walk(small);
    fullerene_unlabeled_patch_walk(large);
    return expansion;
}

// An oriented first edge fixes every vertex of a triangular patch. All
// boundary and internal vertices must be distinct, as required in Figure 1.
inline bool fullerene_unlabeled_match(
    const PlaneGraph& graph, const FullereneUnlabeledPatch& patch,
    int u, int v, int direction, std::vector<int>& mapping) {
    const int n = static_cast<int>(graph.rotation.size());
    if (patch.vertex_count > n) return false;
    const auto degree_matches = [&](int vertex, int actual) {
        return patch.required_degree[vertex] == 0
            || patch.required_degree[vertex] == static_cast<int>(graph.rotation[actual].size());
    };
    if (!degree_matches(patch.walk[0][0], u) || !degree_matches(patch.walk[0][1], v)) return false;
    mapping.assign(patch.vertex_count, -1);
    std::vector<bool> used(n, false);
    mapping[patch.walk[0][0]] = u;
    mapping[patch.walk[0][1]] = v;
    used[u] = used[v] = true;
    for (const auto& step : patch.walk) {
        const int a = mapping[step[0]], b = mapping[step[1]];
        const auto& row = graph.rotation[b];
        const int degree = static_cast<int>(row.size());
        const int position = static_cast<int>(std::find(row.begin(), row.end(), a) - row.begin());
        if (position == degree) return false;
        const int c = row[(position + direction + degree) % degree];
        if (mapping[step[2]] >= 0) {
            if (mapping[step[2]] != c) return false;
        } else {
            if (used[c] || !degree_matches(step[2], c)) return false;
            used[c] = true;
            mapping[step[2]] = c;
        }
    }
    return true;
}

inline bool fullerene_unlabeled_replace(
    const PlaneGraph& graph, const FullereneUnlabeledPatch& from,
    const FullereneUnlabeledPatch& to, const std::vector<int>& mapping,
    int direction, PlaneGraph& result) {
    const int n = static_cast<int>(graph.rotation.size());
    std::set<FullereneUnlabeledFace> removed;
    for (const auto& face : from.faces) {
        FullereneUnlabeledFace key = {{mapping[face[0]], mapping[face[1]], mapping[face[2]]}};
        std::sort(key.begin(), key.end());
        removed.insert(key);
    }
    std::vector<int> labels(n, 0);
    for (int i = from.boundary_count; i < from.vertex_count; ++i) labels[mapping[i]] = -1;
    int order = 0;
    for (int i = 0; i < n; ++i) if (labels[i] != -1) labels[i] = order++;
    std::vector<int> replacement(to.vertex_count);
    for (int i = 0; i < to.boundary_count; ++i) replacement[i] = labels[mapping[i]];
    for (int i = to.boundary_count; i < to.vertex_count; ++i) replacement[i] = order++;
    std::vector<FullereneUnlabeledFace> faces;
    for (const auto& face : fullerene_unlabeled_faces(graph)) {
        auto key = face;
        std::sort(key.begin(), key.end());
        if (removed.count(key)) continue;
        faces.push_back({{labels[face[0]], labels[face[1]], labels[face[2]]}});
    }
    for (const auto& face : to.faces) {
        FullereneUnlabeledFace added = {{replacement[face[0]], replacement[face[1]], replacement[face[2]]}};
        if (direction < 0) std::swap(added[0], added[1]);
        faces.push_back(added);
    }
    return fullerene_unlabeled_from_faces(order, faces, result);
}

// Select the first admissible inverse patch on the canonical embedding.
// This tie-break is ours; buckygen's reduction invariants/orbit pruning are
// not reproduced. Empty means an irreducible seed, not a failed recognition.
inline std::vector<int> fullerene_unlabeled_parent_code(const PlaneGraph& canonical) {
    const int n = static_cast<int>(canonical.rotation.size());
    for (int k = 2; k <= std::min(n - 12, (n - 2) / 3); ++k) {
        for (int bend = -1; bend <= k - 3; ++bend) {
            const auto expansion = fullerene_unlabeled_expansion(k, bend);
            for (int u = 0; u < n; ++u) {
                for (int v : canonical.rotation[u]) {
                    for (int direction = -1; direction <= 1; direction += 2) {
                        std::vector<int> mapping;
                        if (!fullerene_unlabeled_match(canonical, expansion.larger, u, v, direction, mapping)) continue;
                        PlaneGraph parent;
                        if (fullerene_unlabeled_replace(canonical, expansion.larger, expansion.smaller,
                                                       mapping, direction, parent))
                            return plane_graph_canonicalize(parent).code;
                    }
                }
            }
        }
    }
    return std::vector<int>();
}

inline std::vector<PlaneGraphCanon> fullerene_unlabeled_children(
    const PlaneGraphCanon& parent, int target) {
    std::vector<PlaneGraphCanon> children;
    std::set<std::vector<int> > seen;
    const int n = static_cast<int>(parent.graph.rotation.size());
    for (int k = 2; k <= std::min(target - n, (n - 2) / 2); ++k) {
        for (int bend = -1; bend <= k - 3; ++bend) {
            const auto expansion = fullerene_unlabeled_expansion(k, bend);
            for (int u = 0; u < n; ++u) {
                for (int v : parent.graph.rotation[u]) {
                    for (int direction = -1; direction <= 1; direction += 2) {
                        std::vector<int> mapping;
                        if (!fullerene_unlabeled_match(parent.graph, expansion.smaller, u, v, direction, mapping)) continue;
                        PlaneGraph child;
                        if (!fullerene_unlabeled_replace(parent.graph, expansion.smaller, expansion.larger,
                                                        mapping, direction, child)) continue;
                        auto canonical = plane_graph_canonicalize(child);
                        if (!seen.insert(canonical.code).second) continue;
                        if (fullerene_unlabeled_parent_code(canonical.graph) == parent.code)
                            children.push_back(std::move(canonical));
                    }
                }
            }
        }
    }
    return children;
}

// Heap-backed depth-first search, including all exceptional roots up to the
// requested size. Only the requested size is emitted; intermediate orders
// are needed because L/B expansions add different numbers of vertices.
template <typename Callback>
inline void fullerene_unlabeled_expand(int target, Callback emit) {
    std::vector<PlaneGraphCanon> pending;
    pending.push_back(plane_graph_canonicalize(fullerene_unlabeled_nanotube(2)));
    if (target >= 16) pending.push_back(plane_graph_canonicalize(fullerene_unlabeled_c28()));
    for (int rings = 3; rings <= (target - 2) / 5; ++rings)
        pending.push_back(plane_graph_canonicalize(fullerene_unlabeled_nanotube(rings)));
    while (!pending.empty()) {
        auto current = std::move(pending.back());
        pending.pop_back();
        const int n = static_cast<int>(current.graph.rotation.size());
        if (n == target) { emit(current.graph); continue; }
        if (n + 2 > target) continue;
        auto children = fullerene_unlabeled_children(current, target);
        for (auto& child : children) pending.push_back(std::move(child));
    }
}

}  // namespace detail
}  // namespace graph_recognition

#endif
