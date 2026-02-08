#ifndef GRAPH_RECOGNITION_INTERVAL_V2_H
#define GRAPH_RECOGNITION_INTERVAL_V2_H

#include "chordal.h"
#include "clique.h"
#include "graph.h"
#include "interval.h"
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

namespace detail_v2 {

// Check whether a graph has an asteroidal triple (AT).
// An AT is three vertices a, b, c such that between any two there exists
// a path avoiding the closed neighborhood of the third.
//
// Algorithm: for each vertex v, compute connected components of G - N[v].
// Two vertices are "connected avoiding N[v]" iff they are in the same
// component.  Then check all triples.
//
// Complexity: O(n * (n + m)) for component computation + O(n^3) for triple check.
// = O(n^3 + nm).
inline bool has_asteroidal_triple(const Graph& g) {
    int n = g.n;
    if (n < 3) return false;

    // comp[v][u] = component label of u in G - N[v].
    // -1 if u is in N[v] or u == v.
    std::vector<std::vector<int>> comp(n + 1, std::vector<int>(n + 1, -1));

    for (int v = 1; v <= n; ++v) {
        // Mark N[v] (closed neighborhood).
        std::vector<unsigned char> blocked(n + 1, 0);
        blocked[v] = 1;
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            blocked[g.adj[v][i]] = 1;
        }

        // BFS to find components of G - N[v].
        int label = 0;
        std::queue<int> q;
        for (int u = 1; u <= n; ++u) {
            if (blocked[u]) continue;
            if (comp[v][u] >= 0) continue;
            comp[v][u] = label;
            q.push(u);
            while (!q.empty()) {
                int cur = q.front();
                q.pop();
                for (size_t i = 0; i < g.adj[cur].size(); ++i) {
                    int w = g.adj[cur][i];
                    if (blocked[w]) continue;
                    if (comp[v][w] >= 0) continue;
                    comp[v][w] = label;
                    q.push(w);
                }
            }
            label++;
        }
    }

    // Check all triples (a, b, c).
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                // (a, b, c) is an AT if:
                //   a and b connected avoiding N[c]
                //   a and c connected avoiding N[b]
                //   b and c connected avoiding N[a]
                if (comp[c][a] >= 0 && comp[c][b] >= 0 && comp[c][a] == comp[c][b] &&
                    comp[b][a] >= 0 && comp[b][c] >= 0 && comp[b][a] == comp[b][c] &&
                    comp[a][b] >= 0 && comp[a][c] >= 0 && comp[a][b] == comp[a][c]) {
                    return true;
                }
            }
        }
    }

    return false;
}

} // namespace detail_v2

// Check whether a graph is an interval graph (improved version).
// Algorithm:
//   1. Check chordality via MCS + PEO verification.
//   2. Check AT-free (asteroidal triple free) in polynomial time.
//   3. If chordal and AT-free, build interval model using clique path search.
//      (The search is guaranteed to succeed and be fast for actual interval graphs.)
inline IntervalResult check_interval_v2(const Graph& g) {
    IntervalResult res;
    res.is_interval = false;
    int n = g.n;

    // Step 1: Check chordality.
    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    // Step 2: Check AT-free (polynomial).
    if (detail_v2::has_asteroidal_triple(g)) return res;

    // Step 3: Build interval model.
    // Since the graph is chordal and AT-free, it's an interval graph.
    // Enumerate maximal cliques and find a valid clique path.
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

    // Build per-clique vertex sets for O(1) membership.
    std::vector<std::unordered_set<int>> cset(k);
    for (int i = 0; i < k; ++i) {
        cset[i].reserve(mc.cliques[i].size() * 2 + 1);
        for (size_t j = 0; j < mc.cliques[i].size(); ++j) {
            cset[i].insert(mc.cliques[i][j]);
        }
    }

    // Find a valid consecutive-1s ordering using backtracking.
    // Since we know the graph IS interval, this will succeed quickly.
    std::vector<int> unplaced_count(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        unplaced_count[v] = (int)mc.member[v].size();
    }

    std::vector<bool> placed(k, false);
    std::vector<bool> finished(n + 1, false);
    std::vector<int> clique_order;
    clique_order.reserve(k);

    // Collect candidate starting cliques.
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

        if (detail::find_clique_path(k, n, clique_order, placed, finished,
                                     unplaced_count, mc, cset)) {
            found = true;
        }
    }

    if (!found) return res; // Should not happen for interval graphs.

    // Build interval model.
    std::vector<int> pos(k);
    for (int p = 0; p < k; ++p) pos[clique_order[p]] = p;

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

} // namespace graph_recognition

#endif
