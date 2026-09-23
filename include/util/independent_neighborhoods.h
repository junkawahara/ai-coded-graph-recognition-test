#ifndef GRAPH_RECOGNITION_INDEPENDENT_NEIGHBORHOODS_H
#define GRAPH_RECOGNITION_INDEPENDENT_NEIGHBORHOODS_H

#include <vector>

namespace graph_recognition {
namespace detail {

template <typename Callback>
inline void independent_neighborhoods_dfs(
    int last, const std::vector<unsigned long long>& adj,
    unsigned long long available, unsigned long long neighborhood,
    const Callback& emit) {
    if (available == 0) {
        emit(neighborhood);
        return;
    }

    while ((available & (1ULL << last)) == 0) --last;
    const unsigned long long bit = 1ULL << last;
    available &= ~bit;
    // Read the row before callbacks can extend/reallocate the adjacency vector.
    const unsigned long long compatible = available & ~adj[last];
    independent_neighborhoods_dfs(last - 1, adj, available, neighborhood, emit);
    independent_neighborhoods_dfs(last - 1, adj, compatible, neighborhood | bit, emit);
}

/**
 * @brief Emits every independent neighborhood once, including the empty set.
 *
 * The simple undirected parent has vertices 0, ..., k-1, with 0 <= k <= 64
 * and at least k adjacency rows. Including a vertex removes its neighbors
 * from the remaining candidates; excluding it removes only that vertex.
 * Thus every leaf is an independent set, and no invalid subset is built or
 * tested. Excluding the highest available bit first emits masks in ascending
 * order, matching an all-subsets scan filtered by independence.
 *
 * Uses O(k) stack space and O(k) work per emitted mask (excluding callbacks),
 * without storing the masks. A callback may extend adj while exploring a
 * child, provided it restores the parent before returning; no row references
 * or iterators are retained across callbacks.
 */
template <typename Callback>
inline void enumerate_independent_neighborhoods(
    int k, const std::vector<unsigned long long>& adj, const Callback& emit) {
    const unsigned long long available = k == 64 ? ~0ULL : (1ULL << k) - 1;
    independent_neighborhoods_dfs(k - 1, adj, available, 0, emit);
}

}  // namespace detail
}  // namespace graph_recognition

#endif
