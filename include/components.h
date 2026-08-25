#ifndef GRAPH_RECOGNITION_COMPONENTS_H
#define GRAPH_RECOGNITION_COMPONENTS_H

/**
 * @file components.h
 * @brief Connected components and complement connected components
 *
 * Besides the whole-graph entry points, subset-restricted variants are
 * provided: decomposition algorithms repeatedly ask for the components of an
 * induced subgraph, and renumbering the subgraph for each such call would be
 * both wasteful and a source of index-translation bugs. The subset variants
 * therefore take and return original vertex numbers.
 *
 * The complement searches never materialize the complement graph: they keep a
 * list of not-yet-assigned vertices and, at every step, move all non-neighbors
 * of the current vertex at once. A vertex either leaves the list (at most once)
 * or stays because it is adjacent to the current vertex (chargeable to an
 * edge), so the total work is O(n + m) per call rather than O(n^2).
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Connected components of a graph
 */
struct ComponentsResult {
    int count = 0;                          /**< number of components */
    std::vector<int> comp;                  /**< comp[v] = component id in [1, count] (size n+1, comp[0] = 0) */
    std::vector<std::vector<int>> vertices; /**< vertices[i] = vertex list of component i+1 */
};

/**
 * @brief Connected components of the subgraph induced on a vertex subset
 * @param g Input graph
 * @param verts Vertex subset (original vertex numbers)
 * @return Component vertex lists, in original vertex numbers
 *
 * Out-of-range entries are ignored. Duplicates in `verts` would place a vertex
 * in a component twice, so they are dropped as well.
 */
inline std::vector<std::vector<int>> induced_components(const Graph& g,
                                                        const std::vector<int>& verts) {
    std::vector<std::vector<int>> comps;
    std::vector<unsigned char> in_subset(g.n + 1, 0);
    std::vector<unsigned char> seen(g.n + 1, 0);
    std::vector<int> members;
    members.reserve(verts.size());
    for (size_t i = 0; i < verts.size(); ++i) {
        int v = verts[i];
        if (v < 1 || v > g.n) continue;
        if (in_subset[v]) continue;
        in_subset[v] = 1;
        members.push_back(v);
    }

    std::vector<int> stack;
    for (size_t i = 0; i < members.size(); ++i) {
        int s = members[i];
        if (seen[s]) continue;
        std::vector<int> comp;
        seen[s] = 1;
        stack.push_back(s);
        while (!stack.empty()) {
            int v = stack.back();
            stack.pop_back();
            comp.push_back(v);
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (!in_subset[u] || seen[u]) continue;
                seen[u] = 1;
                stack.push_back(u);
            }
        }
        comps.push_back(comp);
    }
    return comps;
}

/**
 * @brief Connected components of the complement of the subgraph induced on a vertex subset
 * @param g Input graph
 * @param verts Vertex subset (original vertex numbers)
 * @return Co-component vertex lists, in original vertex numbers
 *
 * Equivalent to induced_components() on the complement of g[verts], but the
 * complement is never built.
 */
inline std::vector<std::vector<int>> induced_co_components(const Graph& g,
                                                           const std::vector<int>& verts) {
    std::vector<std::vector<int>> comps;
    std::vector<unsigned char> alive(g.n + 1, 0);
    std::vector<int> unassigned;
    unassigned.reserve(verts.size());
    for (size_t i = 0; i < verts.size(); ++i) {
        int v = verts[i];
        if (v < 1 || v > g.n) continue;
        if (alive[v]) continue;
        alive[v] = 1;
        unassigned.push_back(v);
    }

    std::vector<int> queue;
    std::vector<int> kept;
    while (!unassigned.empty()) {
        int s = unassigned.back();
        unassigned.pop_back();
        if (!alive[s]) continue;
        alive[s] = 0;

        std::vector<int> comp;
        comp.push_back(s);
        queue.clear();
        queue.push_back(s);
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int v = queue[qi];
            kept.clear();
            kept.reserve(unassigned.size());
            for (size_t i = 0; i < unassigned.size(); ++i) {
                int u = unassigned[i];
                if (!alive[u]) continue;
                if (g.has_edge(v, u)) {
                    kept.push_back(u);
                } else {
                    alive[u] = 0;
                    comp.push_back(u);
                    queue.push_back(u);
                }
            }
            unassigned.swap(kept);
        }
        comps.push_back(comp);
    }
    return comps;
}

namespace detail {

/** @brief Packs component vertex lists into a ComponentsResult */
inline ComponentsResult pack_components(int n, const std::vector<std::vector<int>>& comps) {
    ComponentsResult res;
    res.count = (int)comps.size();
    res.comp.assign(n + 1, 0);
    res.vertices = comps;
    for (size_t i = 0; i < comps.size(); ++i) {
        for (size_t j = 0; j < comps[i].size(); ++j) {
            res.comp[comps[i][j]] = (int)i + 1;
        }
    }
    return res;
}

} // namespace detail

/**
 * @brief Computes the connected components of a graph
 * @param g Input graph
 * @return ComponentsResult
 */
inline ComponentsResult connected_components(const Graph& g) {
    std::vector<int> all;
    all.reserve(g.n);
    for (int v = 1; v <= g.n; ++v) all.push_back(v);
    return detail::pack_components(g.n, induced_components(g, all));
}

/**
 * @brief Computes the connected components of the complement of a graph
 * @param g Input graph
 * @return ComponentsResult describing the components of complement(g)
 */
inline ComponentsResult co_components(const Graph& g) {
    std::vector<int> all;
    all.reserve(g.n);
    for (int v = 1; v <= g.n; ++v) all.push_back(v);
    return detail::pack_components(g.n, induced_co_components(g, all));
}

} // namespace graph_recognition

#endif
