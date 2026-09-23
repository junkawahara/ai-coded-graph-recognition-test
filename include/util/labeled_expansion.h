#ifndef GRAPH_RECOGNITION_LABELED_EXPANSION_H
#define GRAPH_RECOGNITION_LABELED_EXPANSION_H

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace detail {

/**
 * @brief Appends every distinct labeling of one simple graph on {1, ..., n}
 *
 * The input edges must be sorted with u < v. OutputGraph has fields n and
 * edges. Adjacent label swaps generate all vertex permutations, so exploring
 * their action on edge sets reaches all n!/|Aut(G)| labelings. A visited set
 * removes automorphism duplicates, including twins and equal components.
 * Distinct non-isomorphic representatives have disjoint orbits.
 *
 * The appended portion of out doubles as the traversal queue. For an orbit
 * of L graphs and n >= 1, there are L(n-1) swap attempts, each sorting m edges
 * and looking up an edge vector in a set of at most L entries. The set uses
 * O(L(m+1)) extra space and is released after this representative.
 */
template <typename OutputGraph>
inline void expand_graph_labelings(
    int n, const std::vector<std::pair<int, int>>& edges,
    std::vector<OutputGraph>* out) {
    std::set<std::vector<std::pair<int, int>>> seen;
    seen.insert(edges);
    const std::size_t first = out->size();
    OutputGraph initial;
    initial.n = n;
    initial.edges = edges;
    out->push_back(std::move(initial));

    for (std::size_t i = first; i < out->size(); ++i) {
        for (int label = 1; label < n; ++label) {
            // Copy before appending: push_back can invalidate references to out.
            OutputGraph next = (*out)[i];
            for (std::size_t e = 0; e < next.edges.size(); ++e) {
                int& u = next.edges[e].first;
                int& v = next.edges[e].second;
                if (u == label) ++u;
                else if (u == label + 1) --u;
                if (v == label) ++v;
                else if (v == label + 1) --v;
                if (u > v) std::swap(u, v);
            }
            std::sort(next.edges.begin(), next.edges.end());
            if (seen.insert(next.edges).second) out->push_back(std::move(next));
        }
    }
}

}  // namespace detail
}  // namespace graph_recognition

#endif
