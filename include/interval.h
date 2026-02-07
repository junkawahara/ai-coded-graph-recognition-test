#ifndef GRAPH_RECOGNITION_INTERVAL_H
#define GRAPH_RECOGNITION_INTERVAL_H

#include "chordal.h"
#include "clique.h"
#include "graph.h"
#include <algorithm>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

// Result of interval graph recognition.
struct IntervalResult {
    bool is_interval;
    // intervals[v] = (L, R) for vertex v (1-indexed).
    // Only valid when is_interval == true.
    std::vector<std::pair<int, int>> intervals;
};

namespace detail {

// Backtracking search for a consecutive-1s ordering of maximal cliques.
// Returns true if a valid ordering is found (stored in clique_order).
inline bool find_clique_path(
    int k, int n,
    std::vector<int>& clique_order,
    std::vector<bool>& placed,
    std::vector<bool>& finished,
    std::vector<int>& unplaced_count,
    const MaximalCliques& mc,
    const std::vector<std::unordered_set<int>>& cset)
{
    if ((int)clique_order.size() == k) return true;

    int cur = clique_order.back();

    // Active vertices: in current clique and in some unplaced clique.
    std::vector<int> active;
    for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
        int v = mc.cliques[cur][j];
        if (unplaced_count[v] > 0) active.push_back(v);
    }

    if (active.empty()) {
        // Current chain ended. Start a new chain for another component.
        for (int c = 0; c < k; ++c) {
            if (placed[c]) continue;
            // Check no finished vertex in this clique.
            bool ok = true;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j) {
                if (finished[mc.cliques[c][j]]) { ok = false; break; }
            }
            if (!ok) continue;

            clique_order.push_back(c);
            placed[c] = true;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j)
                unplaced_count[mc.cliques[c][j]]--;

            // Mark all vertices of previous last clique as finished.
            std::vector<int> newly_finished;
            for (size_t j = 0; j < mc.cliques[cur].size(); ++j) {
                int v = mc.cliques[cur][j];
                if (!finished[v]) {
                    finished[v] = true;
                    newly_finished.push_back(v);
                }
            }

            if (find_clique_path(k, n, clique_order, placed, finished,
                                 unplaced_count, mc, cset))
                return true;

            // Undo.
            for (size_t j = 0; j < newly_finished.size(); ++j)
                finished[newly_finished[j]] = false;
            for (size_t j = 0; j < mc.cliques[c].size(); ++j)
                unplaced_count[mc.cliques[c][j]]++;
            placed[c] = false;
            clique_order.pop_back();
        }
        return false;
    }

    // Find valid next cliques: must contain all active and no finished vertices.
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

        // Try this candidate.
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

        if (find_clique_path(k, n, clique_order, placed, finished,
                             unplaced_count, mc, cset))
            return true;

        // Undo.
        for (size_t j = 0; j < newly_finished.size(); ++j)
            finished[newly_finished[j]] = false;
        for (size_t j = 0; j < mc.cliques[c].size(); ++j)
            unplaced_count[mc.cliques[c][j]]++;
        placed[c] = false;
        clique_order.pop_back();
    }
    return false;
}

} // namespace detail

// Check whether a graph is an interval graph.
// Algorithm:
//   1. Check chordality via MCS + PEO verification.
//   2. Enumerate maximal cliques.
//   3. Search for a consecutive-1s ordering of cliques: try each endpoint
//      candidate as starting clique, then use backtracking to extend.
//   4. Verify the ordering and build the interval model.
inline IntervalResult check_interval(const Graph& g) {
    IntervalResult res;
    res.is_interval = false;
    int n = g.n;

    // Step 1: Check chordality.
    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    // Step 2: Enumerate maximal cliques.
    MaximalCliques mc = enumerate_maximal_cliques(g, chordal);
    int k = (int)mc.cliques.size();
    if (k == 0) return res;

    // Build per-clique vertex sets for O(1) membership.
    std::vector<std::unordered_set<int>> cset(k);
    for (int i = 0; i < k; ++i) {
        cset[i].reserve(mc.cliques[i].size() * 2 + 1);
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            cset[i].insert(mc.cliques[i][j]);
        }
    }

    // Step 3: Find a valid consecutive-1s ordering.
    std::vector<int> unplaced_count(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        unplaced_count[v] = (int)mc.member[v].size();
    }

    std::vector<bool> placed(k, false);
    std::vector<bool> finished(n + 1, false);
    std::vector<int> clique_order;
    clique_order.reserve(k);

    // Collect candidate starting cliques: those with a private vertex.
    std::vector<int> starts;
    for (int i = 0; i < k; ++i) {
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            if ((int)mc.member[mc.cliques[i][j]].size() == 1) {
                starts.push_back(i);
                break;
            }
        }
    }
    // If no clique has a private vertex (k == 1), use the only clique.
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

        if (detail::find_clique_path(k, n, clique_order, placed, finished,
                                     unplaced_count, mc, cset)) {
            found = true;
        }
    }

    if (!found) return res;

    // Step 3b: Verify the consecutive-1s property (safety check).
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

    // Step 4: Build interval model.
    res.intervals.resize(n + 1);
    for (int v = 1; v <= n; ++v) {
        const std::vector<int>& cl = mc.member[v];
        if (cl.empty()) {
            res.intervals[v] = std::make_pair(1, 1);
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

} // namespace graph_recognition

#endif
