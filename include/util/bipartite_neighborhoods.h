#ifndef GRAPH_RECOGNITION_BIPARTITE_NEIGHBORHOODS_H
#define GRAPH_RECOGNITION_BIPARTITE_NEIGHBORHOODS_H

#include <cstddef>
#include <vector>

namespace graph_recognition {
namespace detail {

/** @brief The two color classes of one component, as disjoint vertex masks. */
struct BipartiteComponentSides {
    unsigned long long first;
    unsigned long long second;
};

/**
 * @brief BFS 2-coloring of a bipartite graph on vertices 0, ..., k-1 (k <= 64).
 *
 * Bipartiteness is a search invariant, so no conflict check is needed.
 * Each component can swap its two sides independently, including isolates.
 */
inline std::vector<BipartiteComponentSides> bipartite_component_sides(
    int k, const std::vector<unsigned long long>& adj) {
    std::vector<BipartiteComponentSides> sides;
    unsigned long long visited = 0;
    std::vector<int> queue;
    for (int s = 0; s < k; ++s) {
        if ((visited >> s) & 1ULL) continue;
        BipartiteComponentSides comp = {1ULL << s, 0};
        visited |= 1ULL << s;
        queue.clear();
        queue.push_back(s);
        for (std::size_t qi = 0; qi < queue.size(); ++qi) {
            const int u = queue[qi];
            const bool u_in_first = ((comp.first >> u) & 1ULL) != 0;
            const unsigned long long fresh = adj[u] & ~visited;
            for (int v = 0; v < k; ++v) {
                if (!((fresh >> v) & 1ULL)) continue;
                visited |= 1ULL << v;
                if (u_in_first) comp.second |= 1ULL << v;
                else comp.first |= 1ULL << v;
                queue.push_back(v);
            }
        }
        sides.push_back(comp);
    }
    return sides;
}

template <typename Callback>
inline void bipartite_neighborhoods_dfs(
    const std::vector<BipartiteComponentSides>& sides, std::size_t component,
    unsigned long long neighborhood, const Callback& emit) {
    if (component == sides.size()) {
        emit(neighborhood);
        return;
    }

    // The empty choice is shared by both sides and must be visited only once.
    bipartite_neighborhoods_dfs(sides, component + 1, neighborhood, emit);
    const unsigned long long masks[] = {
        sides[component].first, sides[component].second};
    for (int side = 0; side < 2; ++side) {
        for (unsigned long long subset = masks[side]; subset != 0;
             subset = (subset - 1) & masks[side]) {
            bipartite_neighborhoods_dfs(sides, component + 1,
                                       neighborhood | subset, emit);
        }
    }
}

/**
 * @brief Emits exactly the neighborhoods that preserve bipartiteness.
 *
 * In each connected component a new vertex may meet either color class,
 * but never both: meeting both creates an odd cycle, while meeting one
 * permits that component to be colored opposite the new vertex.
 * The Cartesian product of these choices emits each valid mask once,
 * including the empty mask. No invalid candidate or list of masks is built.
 */
template <typename Callback>
inline void enumerate_bipartite_neighborhoods(
    const std::vector<BipartiteComponentSides>& sides, const Callback& emit) {
    bipartite_neighborhoods_dfs(sides, 0, 0, emit);
}

}  // namespace detail
}  // namespace graph_recognition

#endif
