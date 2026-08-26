#ifndef GRAPH_RECOGNITION_PROPER_INTERVAL_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_PROPER_INTERVAL_UNLABELED_ENUM_H

/**
 * @file proper_interval_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic proper interval graphs
 *
 * Enumerates one representative per isomorphism class using the bracket-string
 * representation of Saitoh, Yamanaka, Kiyomi and Uehara. A connected proper
 * interval graph on n vertices corresponds to a string of n '[' and n ']' in
 * which every proper non-empty prefix holds strictly more '[' than ']', that is
 * '[' + (Dyck word of semilength n-1) + ']'. The representation is unique up to
 * reversal (Deng, Hell, Huang), so keeping only the strings that do not exceed
 * their reverse-flip leaves exactly one string per isomorphism class.
 * Disconnected graphs are composed from connected components over integer
 * partitions, as forest_unlabeled_enum.h does for trees.
 *
 * Number of non-isomorphic proper interval graphs: OEIS A005217
 *   1, 2, 4, 9, 21, 55, 151, 447, ...
 * Connected only: OEIS A007123
 *   1, 1, 2, 4, 10, 26, 76, 232, ...
 *
 * References:
 *   Saitoh, Yamanaka, Kiyomi, Uehara, "Random Generation and Enumeration of
 *   Proper Interval Graphs," WALCOM 2009 / IEICE Trans. E93-D(7), 2010
 *
 *   Deng, Hell, Huang, "Linear-Time Representation Algorithms for Proper
 *   Circular-Arc Graphs and Proper Interval Graphs," SIAM J. Comput. 25(2), 1996
 */

#include <algorithm>
#include <cstddef>
#include <deque>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic proper interval enumeration
 */
enum class ProperIntervalUnlabeledEnumAlgorithm {
    STRING_REPRESENTATION /**< Saitoh et al. bracket-string representation */
};

/**
 * @brief An enumerated proper interval graph
 */
struct ProperIntervalUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic proper interval enumeration
 */
struct ProperIntervalUnlabeledEnumerationResult {
    std::vector<ProperIntervalUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/**
 * @brief Tests whether a bracket string is the canonical one of its mirror pair
 *
 * The mirror of a string is its reverse with '[' and ']' exchanged; it
 * represents the same graph read from the other end. The canonical
 * representative is the one that does not exceed its mirror.
 *
 * The comparison must accept equality: a string equal to its own mirror (P3's
 * "[[]]" for instance) is the only representative of its class, and rejecting
 * it would drop that graph from the enumeration.
 */
inline bool proper_interval_unlabeled_string_is_canonical(const std::string& s) {
    std::size_t len = s.size();
    for (std::size_t i = 0; i < len; ++i) {
        char a = s[i];
        // Mirror character at this position: flip the bracket at the far end.
        char b = (s[len - 1 - i] == '[') ? ']' : '[';
        if (a != b) return a < b;
    }
    return true;  // equal to its own mirror
}

/**
 * @brief Builds the proper interval graph a bracket string represents
 *
 * '[' opens a new vertex, numbered in opening order and adjacent to every
 * vertex still open. ']' closes the vertex that has been open the longest
 * (FIFO). Closing the most recent one instead (LIFO) builds nested intervals,
 * which is a different graph and not a proper interval representation.
 */
inline ProperIntervalUnlabeledEnumeratedGraph proper_interval_unlabeled_build_graph(
    const std::string& s) {

    ProperIntervalUnlabeledEnumeratedGraph g;
    g.n = 0;

    std::deque<int> open;
    int vertex_count = 0;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '[') {
            int v = ++vertex_count;
            for (std::deque<int>::const_iterator it = open.begin(); it != open.end(); ++it) {
                g.edges.push_back(std::make_pair(*it, v));  // *it < v: opened earlier
            }
            open.push_back(v);
        } else {
            open.pop_front();
        }
    }

    g.n = vertex_count;
    std::sort(g.edges.begin(), g.edges.end());
    return g;
}

/**
 * @brief Recursively generates Dyck words and emits the canonical graphs
 *
 * Generates the middle part of the string; the caller wraps it in the leading
 * '[' and trailing ']' that make every proper prefix strictly positive.
 *
 * @param pairs Number of bracket pairs in the middle part
 * @param open_used Number of '[' placed so far
 * @param close_used Number of ']' placed so far
 * @param buf Middle part under construction
 * @param out Storage for results
 */
inline void proper_interval_unlabeled_dyck_dfs(
    int pairs, int open_used, int close_used,
    std::string& buf,
    std::vector<ProperIntervalUnlabeledEnumeratedGraph>& out) {

    if (close_used == pairs) {
        std::string s = "[" + buf + "]";
        if (proper_interval_unlabeled_string_is_canonical(s)) {
            out.push_back(proper_interval_unlabeled_build_graph(s));
        }
        return;
    }

    if (open_used < pairs) {
        buf.push_back('[');
        proper_interval_unlabeled_dyck_dfs(pairs, open_used + 1, close_used, buf, out);
        buf.erase(buf.size() - 1);
    }
    if (close_used < open_used) {
        buf.push_back(']');
        proper_interval_unlabeled_dyck_dfs(pairs, open_used, close_used + 1, buf, out);
        buf.erase(buf.size() - 1);
    }
}

/**
 * @brief Enumerates connected non-isomorphic proper interval graphs on n vertices
 *
 * The strict-prefix condition on the string is exactly connectivity: a balanced
 * proper prefix would split the vertices into two parts with no interval
 * spanning the boundary, that is into separate components. Hence wrapping every
 * Dyck word of semilength n-1 covers all connected graphs, and by the
 * uniqueness of the representation up to reversal the canonicity filter keeps
 * each isomorphism class exactly once.
 *
 * The count is A007123(n) = (Catalan(n-1) + C(n-1, floor((n-1)/2))) / 2.
 */
inline std::vector<ProperIntervalUnlabeledEnumeratedGraph>
proper_interval_unlabeled_connected(int n) {
    std::vector<ProperIntervalUnlabeledEnumeratedGraph> out;
    if (n < 1) return out;

    std::string buf;
    proper_interval_unlabeled_dyck_dfs(n - 1, 0, 0, buf, out);
    return out;
}

/**
 * @brief Recursive function that selects a component for each part and builds a graph
 *
 * Selects a connected graph for each part size in parts[idx..], applying
 * non-decreasing index constraints for equal-size parts.
 *
 * @param parts Partition (in non-increasing order)
 * @param idx Index of the currently processed part
 * @param prev_index Index of the component chosen for the previous part of equal size
 * @param vertex_offset Current vertex offset
 * @param current_edges Edge list under construction
 * @param connected_by_size List of connected graphs by size
 * @param results Storage for results
 * @param n Total number of vertices
 */
inline void proper_interval_unlabeled_combine(
    const std::vector<int>& parts, std::size_t idx,
    int prev_index, int vertex_offset,
    std::vector<std::pair<int, int> >& current_edges,
    const std::map<int, std::vector<ProperIntervalUnlabeledEnumeratedGraph> >& connected_by_size,
    std::vector<ProperIntervalUnlabeledEnumeratedGraph>& results,
    int n) {

    if (idx == parts.size()) {
        ProperIntervalUnlabeledEnumeratedGraph g;
        g.n = n;
        g.edges = current_edges;
        std::sort(g.edges.begin(), g.edges.end());
        results.push_back(g);
        return;
    }

    int sz = parts[idx];
    const std::vector<ProperIntervalUnlabeledEnumeratedGraph>& comps =
        connected_by_size.find(sz)->second;

    // If a previous part of equal size exists, restrict index >= prev_index
    int start = 0;
    if (idx > 0 && parts[idx] == parts[idx - 1]) {
        start = prev_index;
    }

    for (int i = start; i < (int)comps.size(); ++i) {
        std::size_t old_size = current_edges.size();
        const std::vector<std::pair<int, int> >& comp_edges = comps[i].edges;
        for (std::size_t e = 0; e < comp_edges.size(); ++e) {
            int u = comp_edges[e].first + vertex_offset;
            int v = comp_edges[e].second + vertex_offset;
            current_edges.push_back(std::make_pair(u, v));
        }

        proper_interval_unlabeled_combine(parts, idx + 1, i, vertex_offset + sz,
                                          current_edges, connected_by_size, results, n);

        current_edges.resize(old_size);
    }
}

/**
 * @brief Enumerates integer partitions and builds a graph for each partition
 *
 * Enumerates all partitions of n as a sum of positive integers in non-increasing order.
 */
inline void proper_interval_unlabeled_partition_dfs(
    int remaining, int max_part,
    std::vector<int>& parts,
    const std::map<int, std::vector<ProperIntervalUnlabeledEnumeratedGraph> >& connected_by_size,
    std::vector<ProperIntervalUnlabeledEnumeratedGraph>& results,
    int n) {

    if (remaining == 0) {
        std::vector<std::pair<int, int> > current_edges;
        proper_interval_unlabeled_combine(parts, 0, 0, 0, current_edges,
                                          connected_by_size, results, n);
        return;
    }

    int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        proper_interval_unlabeled_partition_dfs(remaining - s, s, parts,
                                                connected_by_size, results, n);
        parts.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic proper interval graphs on n vertices
 * @param n Number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: STRING_REPRESENTATION)
 * @return ProperIntervalUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A005217(n) graphs
 * (1, 2, 4, 9, 21, 55, 151, 447, ... for n = 1, 2, ...), or A007123(n) graphs
 * (1, 1, 2, 4, 10, 26, 76, 232, ...) when @p connected_only is set.
 * n = 0 yields the single empty graph in both modes; negative n yields nothing.
 *
 * @note The result materializes every graph; the counts grow exponentially
 *       (A005217 passes two million around n = 15).
 */
inline ProperIntervalUnlabeledEnumerationResult enumerate_proper_interval_unlabeled_graphs(
    int n, bool connected_only = false,
    ProperIntervalUnlabeledEnumAlgorithm algo =
        ProperIntervalUnlabeledEnumAlgorithm::STRING_REPRESENTATION) {
    (void)algo;
    ProperIntervalUnlabeledEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        // check_proper_interval accepts n = 0; emit the empty graph.
        ProperIntervalUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    if (connected_only) {
        result.graphs = detail::proper_interval_unlabeled_connected(n);
        return result;
    }

    // Pre-compute connected graphs for each size
    std::map<int, std::vector<ProperIntervalUnlabeledEnumeratedGraph> > connected_by_size;
    for (int k = 1; k <= n; ++k) {
        connected_by_size[k] = detail::proper_interval_unlabeled_connected(k);
    }

    // Enumerate integer partitions and build graphs
    std::vector<int> parts;
    detail::proper_interval_unlabeled_partition_dfs(n, n, parts, connected_by_size,
                                                    result.graphs, n);

    return result;
}

}  // namespace graph_recognition

#endif
