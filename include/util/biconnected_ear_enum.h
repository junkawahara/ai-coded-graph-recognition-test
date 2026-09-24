#ifndef GRAPH_RECOGNITION_BICONNECTED_EAR_ENUM_H
#define GRAPH_RECOGNITION_BICONNECTED_EAR_ENUM_H

/**
 * @file biconnected_ear_enum.h
 * @brief Stolee-style canonical ear augmentation of 2-connected graphs
 *
 * Starts at C_3, ..., C_n and adds paths between distinct existing vertices.
 * Internal vertices are new; a zero-order ear is a missing edge. Every state
 * is 2-connected. In the canonical labeling of a noncycle, delete a shortest
 * ear whose removal preserves 2-connectivity, breaking ties by its endpoints.
 * Equal-length ears with the same endpoints are interchangeable by an
 * automorphism. This defines one parent isomorphism class for every noncycle.
 *
 * This implementation compares canonical parent forms and deduplicates child
 * forms per parent, instead of computing pair orbits with nauty. It reuses
 * the exact, worst-case factorial bitmask canonicalizer. No global visited
 * set is needed: ear deletion decreases the edge count and reaches a cycle;
 * every inverse augmentation is tried. Only states of exactly n vertices
 * are emitted, but zero-order augmentations continue at that size.
 *
 * Reference: D. Stolee, "Isomorph-free generation of 2-connected graphs with
 * applications" (2011), sections 2.1--2.5, https://arxiv.org/abs/1104.5261
 * This is an implementation of the framework, not the EarSearch software
 * or its nauty/orbit optimizations. No polynomial delay bound is claimed.
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "recognizers/biconnected.h"
#include "util/canonical_augmentation.h"

namespace graph_recognition {
namespace detail {

inline int biconnected_enum_edge_limit(int n, int max_edges) {
    if (max_edges < -1)
        throw std::invalid_argument("biconnected enumeration: max_edges must be -1 or nonnegative");
    const int complete = n * (n - 1) / 2;
    return max_edges == -1 ? complete : std::min(max_edges, complete);
}

inline std::vector<unsigned long long> biconnected_ear_graph_from_form(
    const std::vector<unsigned long long>& form) {
    const int k = static_cast<int>(form.size()) + 1;
    std::vector<unsigned long long> adj(k, 0);
    for (int v = 1; v < k; ++v) {
        adj[v] = form[v - 1];
        for (int u = 0; u < v; ++u)
            if ((adj[v] >> u) & 1ULL) adj[u] |= 1ULL << v;
    }
    return adj;
}

struct BiconnectedEar {
    int u, v, order;
    unsigned long long internal;
};

// The input is a 2-connected noncycle. Walk maximal degree-two paths from
// branch vertices; orient each ear once by its endpoint labels.
inline std::vector<BiconnectedEar> biconnected_ear_paths(
    const std::vector<unsigned long long>& adj) {
    const int k = static_cast<int>(adj.size());
    std::vector<int> degree(k, 0);
    for (int u = 0; u < k; ++u)
        for (auto bits = adj[u]; bits; bits &= bits - 1) ++degree[u];
    std::vector<BiconnectedEar> ears;
    for (int u = 0; u < k; ++u) {
        if (degree[u] < 3) continue;
        for (int first = 0; first < k; ++first) {
            if (!((adj[u] >> first) & 1ULL)) continue;
            int previous = u, v = first, order = 0;
            unsigned long long internal = 0;
            while (degree[v] == 2) {
                internal |= 1ULL << v;
                ++order;
                int next = 0;
                while (next == previous || !((adj[v] >> next) & 1ULL)) ++next;
                previous = v;
                v = next;
            }
            if (u < v) ears.push_back({u, v, order, internal});
        }
    }
    std::sort(ears.begin(), ears.end(), [](const BiconnectedEar& a, const BiconnectedEar& b) {
        if (a.order != b.order) return a.order < b.order;
        if (a.u != b.u) return a.u < b.u;
        if (a.v != b.v) return a.v < b.v;
        return a.internal < b.internal;
    });
    return ears;
}

inline std::vector<unsigned long long> biconnected_ear_delete(
    const std::vector<unsigned long long>& adj, const BiconnectedEar& ear) {
    const int k = static_cast<int>(adj.size());
    std::vector<int> label(k, -1);
    int size = 0;
    for (int u = 0; u < k; ++u)
        if (!((ear.internal >> u) & 1ULL)) label[u] = size++;
    std::vector<unsigned long long> parent(size, 0);
    for (int u = 0; u < k; ++u) {
        if (label[u] < 0) continue;
        for (int v = u + 1; v < k; ++v) {
            if (label[v] < 0 || !((adj[u] >> v) & 1ULL)) continue;
            if (ear.order == 0 && u == ear.u && v == ear.v) continue;
            parent[label[u]] |= 1ULL << label[v];
            parent[label[v]] |= 1ULL << label[u];
        }
    }
    return parent;
}

// Work on the canonical child graph, so the chosen deletion is independent
// of the input labeling. Recognition is needed for deletions, not emissions:
// some ears of a 2-connected graph leave a cut vertex when removed.
inline std::vector<unsigned long long> biconnected_ear_parent(
    const std::vector<unsigned long long>& canonical) {
    const auto adj = biconnected_ear_graph_from_form(canonical);
    for (const auto& ear : biconnected_ear_paths(adj)) {
        auto parent = biconnected_ear_delete(adj, ear);
        const int size = static_cast<int>(parent.size());
        if (size < 3) continue;
        if (check_biconnected(Graph(size, bitmask_graph_edges(size, parent))).is_biconnected)
            return canonicalize_bitmask_graph(size, parent).form;
    }
    throw std::logic_error("biconnected_ear_parent: noncycle has no removable ear");
}

template <typename Emit>
inline void biconnected_ear_enum_dfs(
    const std::vector<unsigned long long>& canonical, int edges,
    int n, int max_edges, Emit& emit) {
    const int k = static_cast<int>(canonical.size()) + 1;
    // At least one more ear is needed when vertices remain. Each such ear
    // adds its internal-vertex count plus one edges (Stolee, section 2.5).
    if (edges > max_edges || (k < n && edges + n - k + 1 > max_edges)) return;
    const auto adj = biconnected_ear_graph_from_form(canonical);
    if (k == n) emit(n, bitmask_graph_edges(k, adj));
    std::set<std::vector<unsigned long long>> child_forms;
    const int max_order = std::min(n - k, max_edges - edges - 1);
    for (int order = 0; order <= max_order; ++order) {
        for (int u = 0; u < k; ++u) {
            for (int v = u + 1; v < k; ++v) {
                if (order == 0 && ((adj[u] >> v) & 1ULL)) continue;
                auto child = adj;
                child.resize(k + order, 0);
                int previous = u;
                for (int i = 0; i < order; ++i) {
                    child[previous] |= 1ULL << (k + i);
                    child[k + i] |= 1ULL << previous;
                    previous = k + i;
                }
                child[previous] |= 1ULL << v;
                child[v] |= 1ULL << previous;
                auto form = canonicalize_bitmask_graph(k + order, child).form;
                if (!child_forms.insert(form).second) continue;
                if (biconnected_ear_parent(form) == canonical)
                    biconnected_ear_enum_dfs(form, edges + order + 1, n, max_edges, emit);
            }
        }
    }
}

// Caller validates 0 <= n <= 63 and normalizes max_edges. Only the current
// DFS path and per-parent sibling forms are retained, in addition to whatever
// the callback stores. Each augmentation increases m - n by one, so the
// recursion depth is at most n(n-1)/2 - n + 1.
template <typename Emit>
inline void enumerate_biconnected_ears(int n, int max_edges, Emit emit) {
    if (n < 3 || max_edges < n) return;
    for (int k = 3; k <= n; ++k) {
        if (k < n && n + 1 > max_edges) continue;
        std::vector<unsigned long long> cycle(k, 0);
        for (int u = 0; u < k; ++u) {
            const int v = (u + 1) % k;
            cycle[u] |= 1ULL << v;
            cycle[v] |= 1ULL << u;
        }
        biconnected_ear_enum_dfs(canonicalize_bitmask_graph(k, cycle).form,
                                k, n, max_edges, emit);
    }
}

}  // namespace detail
}  // namespace graph_recognition

#endif
