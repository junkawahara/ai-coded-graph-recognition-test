#ifndef GRAPH_RECOGNITION_INTERVAL_H
#define GRAPH_RECOGNITION_INTERVAL_H

/**
 * @file interval.h
 * @brief Interval graph recognition
 *
 * Recognizes interval graphs via chordality check + consecutive-1s order search of maximal cliques,
 * and constructs an interval model.
 *
 * Algorithms:
 *   - BACKTRACKING: clique path search via backtracking
 *   - AT_FREE: chordality + AT-free check (polynomial time, default)
 *   - PQ_TREE: chordality + consecutive-ones test of the clique/vertex
 *     incidence matrix (Fulkerson & Gross 1965) using the Booth-Lueker
 *     PQ-tree
 *
 * All three build the model from a clique order and re-verify consecutiveness
 * while doing so, so a wrong order fails closed instead of producing a model
 * that does not describe the graph.
 */

#include "at_free.h"
#include "chordal.h"
#include "clique.h"
#include "graph.h"
#include "pq_tree.h"
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for interval graph recognition
 */
enum class IntervalAlgorithm {
    BACKTRACKING, /**< Clique path search via backtracking */
    AT_FREE,      /**< Chordality + AT-free check (default) */
    PQ_TREE       /**< Chordality + consecutive-ones test via PQ-tree */
};

/**
 * @brief Result of interval graph recognition
 */
struct IntervalResult {
    bool is_interval = false;   /**< true if the graph is an interval graph */
    /**
     * @brief intervals[v] = (L, R): interval for vertex v (1-indexed)
     *
     * Valid only when is_interval == true.
     */
    std::vector<std::pair<int, int>> intervals;
};

namespace detail {

/**
 * @brief Searches for a consecutive-1s order of maximal cliques via backtracking
 */
inline bool find_clique_path(
    int k,
    std::vector<int>& clique_order,
    std::vector<bool>& placed,
    std::vector<bool>& finished,
    std::vector<int>& unplaced_count,
    const MaximalCliques& mc,
    const std::vector<std::unordered_set<int>>& cset)
{
    if ((int)clique_order.size() == k) return true;

    int cur = clique_order.back();

    std::vector<int> active;
    for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
        int v = mc.cliques[cur][j];
        if (unplaced_count[v] > 0) active.push_back(v);
    }

    if (active.empty()) {
        for (int c = 0; c < k; ++c) {
            if (placed[c]) continue;
            bool ok = true;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j) {
                if (finished[mc.cliques[c][j]]) { ok = false; break; }
            }
            if (!ok) continue;

            clique_order.push_back(c);
            placed[c] = true;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j)
                unplaced_count[mc.cliques[c][j]]--;

            std::vector<int> newly_finished;
            for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
                int v = mc.cliques[cur][j];
                if (!finished[v]) {
                    finished[v] = true;
                    newly_finished.push_back(v);
                }
            }

            if (find_clique_path(k, clique_order, placed, finished,
                                 unplaced_count, mc, cset))
                return true;

            for (size_t j = 0; j < newly_finished.size(); ++j)
                finished[newly_finished[j]] = false;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j)
                unplaced_count[mc.cliques[c][j]]++;
            placed[c] = false;
            clique_order.pop_back();
        }
        return false;
    }

    for (int c = 0; c < k; ++c) {
        if (placed[c]) continue;

        bool ok = true;
        for (size_t j = 0; j < active.size(); ++j) {
            if (cset[c].count(active[j]) == 0) { ok = false; break; }
        }
        if (!ok) continue;

        for (size_t j = 0; j < mc.cliques[c].size(); ++j) {
            if (finished[mc.cliques[c][j]]) { ok = false; break; }
        }
        if (!ok) continue;

        clique_order.push_back(c);
        placed[c] = true;
        for (size_t j = 0; j < mc.cliques[c].size(); ++j)
            unplaced_count[mc.cliques[c][j]]--;

        std::vector<int> newly_finished;
        for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
            int v = mc.cliques[cur][j];
            if (cset[c].count(v) == 0 && !finished[v]) {
                finished[v] = true;
                newly_finished.push_back(v);
            }
        }

        if (find_clique_path(k, clique_order, placed, finished,
                             unplaced_count, mc, cset))
            return true;

        for (size_t j = 0; j < newly_finished.size(); ++j)
            finished[newly_finished[j]] = false;
        for (size_t j = 0; j < mc.cliques[c].size(); ++j)
            unplaced_count[mc.cliques[c][j]]++;
        placed[c] = false;
        clique_order.pop_back();
    }
    return false;
}

/**
 * @brief Interval model for a graph whose clique enumeration is empty
 *
 * Only reachable for the edgeless graph, where every vertex gets its own
 * point interval.
 */
inline IntervalResult trivial_interval_result(const Graph& g) {
    IntervalResult res;
    res.is_interval = true;
    res.intervals.resize(g.n + 1);
    for (int v = 1; v <= g.n; ++v) res.intervals[v] = std::make_pair(v, v);
    return res;
}

/**
 * @brief Builds an interval model from an order of the maximal cliques
 *
 * A vertex spans the positions of the cliques containing it. If those
 * positions are not consecutive the order is not a clique path, and an
 * unset result is returned; this doubles as the verification of whatever
 * search produced the order.
 */
inline IntervalResult interval_model_from_clique_order(
    const Graph& g, const MaximalCliques& mc, const std::vector<int>& clique_order) {
    IntervalResult res;
    int n = g.n;
    int k = (int)mc.cliques.size();
    if ((int)clique_order.size() != k) return res;

    std::vector<int> pos(k, -1);
    for (int p = 0; p < k; ++p) {
        int c = clique_order[p];
        if (c < 0 || c >= k || pos[c] != -1) return res;
        pos[c] = p;
    }

    std::vector<std::pair<int, int>> intervals(n + 1);
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) {
            intervals[v] = std::make_pair(k + v, k + v);
            continue;
        }
        int lo = k, hi = -1;
        for (size_t j = 0; j < cl.size(); ++j) {
            int p = pos[cl[j]];
            if (p < lo) lo = p;
            if (p > hi) hi = p;
        }
        if (hi - lo + 1 != (int)cl.size()) return res;
        intervals[v] = std::make_pair(lo + 1, hi + 1);
    }

    res.intervals.swap(intervals);
    res.is_interval = true;
    return res;
}

/**
 * @brief Searches for a clique path by backtracking
 * @return true and the clique order, or false if no clique path exists
 *
 * Cliques holding a vertex that lies in no other clique must be endpoints of
 * the path, so they are tried as starting points first.
 */
inline bool search_clique_order(const Graph& g, const MaximalCliques& mc,
                                std::vector<int>& clique_order) {
    int n = g.n;
    int k = (int)mc.cliques.size();

    std::vector<std::unordered_set<int>> cset(k);
    for (int i = 0; i < k; ++i) {
        cset[i].reserve(mc.cliques[i].size() * 2 + 1);
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            cset[i].insert(mc.cliques[i][j]);
        }
    }

    std::vector<int> unplaced_count(n + 1, 0);
    std::vector<bool> placed(k, false);
    std::vector<bool> finished(n + 1, false);
    clique_order.clear();
    clique_order.reserve(k);

    std::vector<int> starts;
    for (int i = 0; i < k; ++i) {
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            if ((int)mc.member[mc.cliques[i][j]].size() == 1) {
                starts.push_back(i);
                break;
            }
        }
    }
    if (starts.empty()) starts.push_back(0);

    for (size_t si = 0; si < starts.size(); ++si) {
        int s = starts[si];
        clique_order.clear();
        std::fill(placed.begin(), placed.end(), false);
        std::fill(finished.begin(), finished.end(), false);
        for (int v = 1; v <= n; ++v) unplaced_count[v] = (int)mc.member[v].size();

        clique_order.push_back(s);
        placed[s] = true;
        for (size_t j = 0; j < mc.cliques[s].size(); ++j) {
            unplaced_count[mc.cliques[s][j]]--;
        }

        if (find_clique_path(k, clique_order, placed, finished, unplaced_count, mc, cset)) {
            return true;
        }
    }
    clique_order.clear();
    return false;
}

/**
 * @brief Interval graph recognition via backtracking
 */
inline IntervalResult check_interval_backtracking(const Graph& g) {
    IntervalResult res;
    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    MaximalCliques mc = enumerate_maximal_cliques(g, chordal);
    if (mc.cliques.empty()) return trivial_interval_result(g);

    std::vector<int> clique_order;
    if (!search_clique_order(g, mc, clique_order)) return res;
    return interval_model_from_clique_order(g, mc, clique_order);
}

/**
 * @brief Interval graph recognition via AT-free check
 */
inline IntervalResult check_interval_at_free(const Graph& g) {
    IntervalResult res;
    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    if (has_asteroidal_triple(g)) return res;

    // chordal + AT-free is a complete characterization of interval graphs,
    // but is_interval is only set once a clique path (certificate) is found,
    // so the theoretically unreachable failure paths below fail closed
    // instead of returning YES with empty intervals.
    MaximalCliques mc = enumerate_maximal_cliques(g, chordal);
    if (mc.cliques.empty()) return trivial_interval_result(g);

    std::vector<int> clique_order;
    if (!search_clique_order(g, mc, clique_order)) return res;
    return interval_model_from_clique_order(g, mc, clique_order);
}

/**
 * @brief Interval graph recognition via the consecutive ones property
 *
 * Fulkerson & Gross (1965): a chordal graph is an interval graph exactly when
 * its maximal cliques admit a linear order in which the cliques containing
 * any one vertex are consecutive. That is the consecutive ones property of
 * the vertex/clique incidence matrix, which the PQ-tree decides directly.
 *
 * The model this produces is generally a different (equally valid) one from
 * the backtracking search, since the PQ-tree reports an arbitrary order out
 * of the class it represents.
 */
inline IntervalResult check_interval_pq_tree(const Graph& g) {
    IntervalResult res;
    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    MaximalCliques mc = enumerate_maximal_cliques(g, chordal);
    int k = (int)mc.cliques.size();
    if (k == 0) return trivial_interval_result(g);

    std::vector<std::vector<int>> rows;
    rows.reserve(g.n);
    for (int v = 1; v <= g.n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) continue;
        std::vector<int> row;
        row.reserve(cl.size());
        // consecutive_ones() numbers columns from 1.
        for (size_t j = 0; j < cl.size(); ++j) row.push_back(cl[j] + 1);
        rows.push_back(row);
    }

    ConsecutiveOnesResult c1p = consecutive_ones(k, rows);
    if (!c1p.success) return res;

    std::vector<int> clique_order(k);
    for (int i = 1; i <= k; ++i) clique_order[i - 1] = c1p.column_order[i] - 1;
    return interval_model_from_clique_order(g, mc, clique_order);
}

} // namespace detail

/**
 * @brief Determines whether the graph is an interval graph
 * @param g Input graph
 * @param algo Algorithm to use (default: AT_FREE)
 * @return IntervalResult
 */
inline IntervalResult check_interval(const Graph& g,
    IntervalAlgorithm algo = IntervalAlgorithm::AT_FREE) {
    switch (algo) {
        case IntervalAlgorithm::BACKTRACKING:
            return detail::check_interval_backtracking(g);
        case IntervalAlgorithm::AT_FREE:
            return detail::check_interval_at_free(g);
        case IntervalAlgorithm::PQ_TREE:
            return detail::check_interval_pq_tree(g);
        default:
            break;
    }
    return IntervalResult();
}

} // namespace graph_recognition

#endif
