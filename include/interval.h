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
 *   - AT_FREE: chordality + AT-free check (polynomial time)
 */

#include "at_free.h"
#include "chordal.h"
#include "clique.h"
#include "graph.h"
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
    AT_FREE       /**< Chordality + AT-free check (default) */
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
 * @brief Interval graph recognition via backtracking
 */
inline IntervalResult check_interval_backtracking(const Graph& g) {
    IntervalResult res;
    res.is_interval = false;
    int n = g.n;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    MaximalCliques mc = enumerate_maximal_cliques(g, chordal);
    int k = (int)mc.cliques.size();
    if (k == 0) {
        res.is_interval = true;
        res.intervals.resize(n + 1);
        for (int v = 1; v <= n; ++v) {
            res.intervals[v] = std::make_pair(v, v);
        }
        return res;
    }

    std::vector<std::unordered_set<int>> cset(k);
    for (int i = 0; i < k; ++i) {
        cset[i].reserve(mc.cliques[i].size() * 2 + 1);
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            cset[i].insert(mc.cliques[i][j]);
        }
    }

    std::vector<int> unplaced_count(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        unplaced_count[v] = (int)mc.member[v].size();
    }

    std::vector<bool> placed(k, false);
    std::vector<bool> finished(n + 1, false);
    std::vector<int> clique_order;
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
    if (starts.empty()) {
        starts.push_back(0);
    }

    bool found = false;
    for (size_t si = 0; si < starts.size() && !found; ++si) {
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

        if (find_clique_path(k, clique_order, placed, finished,
                                     unplaced_count, mc, cset)) {
            found = true;
        }
    }

    if (!found) return res;

    std::vector<int> pos(k);
    for (int p = 0; p < k; ++p) pos[clique_order[p]] = p;

    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) continue;
        int lo = k, hi = -1;
        for (size_t j = 0; j < cl.size(); ++j) {
            int p = pos[cl[j]];
            if (p < lo) lo = p;
            if (p > hi) hi = p;
        }
        if (hi - lo + 1 != (int)cl.size()) return res;
    }

    res.intervals.resize(n + 1);
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) {
            res.intervals[v] = std::make_pair(k + v, k + v);
            continue;
        }
        int lo = k, hi = -1;
        for (size_t j = 0; j < cl.size(); ++j) {
            int p = pos[cl[j]];
            if (p < lo) lo = p;
            if (p > hi) hi = p;
        }
        res.intervals[v] = std::make_pair(lo + 1, hi + 1);
    }

    res.is_interval = true;
    return res;
}

/**
 * @brief Interval graph recognition via AT-free check
 */
inline IntervalResult check_interval_at_free(const Graph& g) {
    IntervalResult res;
    res.is_interval = false;
    int n = g.n;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    if (has_asteroidal_triple(g)) return res;

    // chordal + AT-free is a complete characterization of interval graphs
    res.is_interval = true;

    MaximalCliques mc = enumerate_maximal_cliques(g, chordal);
    int k = (int)mc.cliques.size();
    if (k == 0) {
        res.is_interval = true;
        res.intervals.resize(n + 1);
        for (int v = 1; v <= n; ++v) {
            res.intervals[v] = std::make_pair(v, v);
        }
        return res;
    }

    std::vector<std::unordered_set<int>> cset(k);
    for (int i = 0; i < k; ++i) {
        cset[i].reserve(mc.cliques[i].size() * 2 + 1);
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            cset[i].insert(mc.cliques[i][j]);
        }
    }

    std::vector<int> unplaced_count(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        unplaced_count[v] = (int)mc.member[v].size();
    }

    std::vector<bool> placed(k, false);
    std::vector<bool> finished(n + 1, false);
    std::vector<int> clique_order;
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
    if (starts.empty()) {
        starts.push_back(0);
    }

    bool found = false;
    for (size_t si = 0; si < starts.size() && !found; ++si) {
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

        if (find_clique_path(k, clique_order, placed, finished,
                                     unplaced_count, mc, cset)) {
            found = true;
        }
    }

    if (!found) return res;

    std::vector<int> pos(k);
    for (int p = 0; p < k; ++p) pos[clique_order[p]] = p;

    // Consecutiveness check: verify that the cliques containing each vertex form a contiguous interval
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) continue;
        int lo = k, hi = -1;
        for (size_t j = 0; j < cl.size(); ++j) {
            int p = pos[cl[j]];
            if (p < lo) lo = p;
            if (p > hi) hi = p;
        }
        if (hi - lo + 1 != (int)cl.size()) return res;
    }

    res.intervals.resize(n + 1);
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) {
            res.intervals[v] = std::make_pair(k + v, k + v);
            continue;
        }
        int lo = k, hi = -1;
        for (size_t j = 0; j < cl.size(); ++j) {
            int p = pos[cl[j]];
            if (p < lo) lo = p;
            if (p > hi) hi = p;
        }
        res.intervals[v] = std::make_pair(lo + 1, hi + 1);
    }

    res.is_interval = true;
    return res;
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
        default:
            break;
    }
    return IntervalResult();
}

} // namespace graph_recognition

#endif
