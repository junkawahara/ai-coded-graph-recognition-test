#ifndef GRAPH_RECOGNITION_COGRAPH_H
#define GRAPH_RECOGNITION_COGRAPH_H

/**
 * @file cograph.h
 * @brief Cograph recognition
 *
 * Recognizes cographs by recursive decomposition into connected components / complement connected components.
 *
 * COTREE: original cotree decomposition algorithm. Scans all unvisited vertices for complement component search.
 * PARTITION_REFINEMENT: fast complement component search using doubly-linked lists.
 *   At each BFS step, temporarily removes adjacent vertices and moves all remaining vertices at once.
 */

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for cograph recognition
 */
enum class CographAlgorithm {
    COTREE,              /**< cotree decomposition */
    PARTITION_REFINEMENT /**< fast cotree decomposition via partition refinement (default) */
};

/**
 * @brief Result of cograph recognition
 */
struct CographResult {
    bool is_cograph = false; /**< true if the graph is a cograph */
};

namespace detail {

/** @brief Recursive cograph checker (original algorithm) */
class CographChecker {
public:
    explicit CographChecker(const Graph& graph)
        : g(graph),
          in_subset(graph.n + 1, 0),
          seen(graph.n + 1, 0),
          subset_token(0),
          seen_token(0) {}

    bool run() {
        std::vector<int> verts;
        verts.reserve(g.n);
        for (int v = 1; v <= g.n; ++v) verts.push_back(v);
        return solve(verts);
    }

private:
    const Graph& g;
    std::vector<long long> in_subset;
    std::vector<long long> seen;
    long long subset_token;
    long long seen_token;

    /* Iterative worklist instead of recursion: the decomposition tree can be
       a path of depth O(n) (e.g. threshold graphs), which overflows the call
       stack for large n. Order of subproblems does not matter. */
    bool solve(const std::vector<int>& all_verts) {
        std::vector<std::vector<int>> pending;
        pending.push_back(all_verts);
        while (!pending.empty()) {
            std::vector<int> verts = std::move(pending.back());
            pending.pop_back();
            if ((int)verts.size() <= 1) continue;

            std::vector<std::vector<int>> comps;
            graph_components(verts, comps);
            if ((int)comps.size() > 1) {
                for (size_t i = 0; i < comps.size(); ++i) {
                    pending.push_back(std::move(comps[i]));
                }
                continue;
            }

            std::vector<std::vector<int>> cocomps;
            complement_components(verts, cocomps);
            if ((int)cocomps.size() > 1) {
                for (size_t i = 0; i < cocomps.size(); ++i) {
                    pending.push_back(std::move(cocomps[i]));
                }
                continue;
            }

            return false;
        }
        return true;
    }

    void graph_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) {
        comps.clear();
        subset_token++;
        seen_token++;
        for (size_t i = 0; i < verts.size(); ++i) {
            in_subset[verts[i]] = subset_token;
        }

        std::queue<int> q;
        for (size_t i = 0; i < verts.size(); ++i) {
            int s = verts[i];
            if (seen[s] == seen_token) continue;
            std::vector<int> comp;
            seen[s] = seen_token;
            q.push(s);
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                comp.push_back(v);
                for (size_t j = 0; j < g.adj[v].size(); ++j) {
                    int u = g.adj[v][j];
                    if (in_subset[u] != subset_token) continue;
                    if (seen[u] == seen_token) continue;
                    seen[u] = seen_token;
                    q.push(u);
                }
            }
            comps.push_back(comp);
        }
    }

    void complement_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) {
        comps.clear();

        std::vector<int> unvisited = verts;
        std::vector<unsigned char> alive(g.n + 1, 0);
        for (size_t i = 0; i < verts.size(); ++i) alive[verts[i]] = 1;

        std::queue<int> q;
        while (!unvisited.empty()) {
            int s = unvisited.back();
            unvisited.pop_back();
            if (!alive[s]) continue;
            alive[s] = 0;

            std::vector<int> comp;
            comp.push_back(s);
            q.push(s);

            while (!q.empty()) {
                int v = q.front();
                q.pop();

                std::vector<int> next_unvisited;
                next_unvisited.reserve(unvisited.size());
                for (size_t i = 0; i < unvisited.size(); ++i) {
                    int u = unvisited[i];
                    if (!alive[u]) continue;
                    if (!g.has_edge(v, u)) {
                        alive[u] = 0;
                        comp.push_back(u);
                        q.push(u);
                    } else {
                        next_unvisited.push_back(u);
                    }
                }
                unvisited.swap(next_unvisited);
            }

            comps.push_back(comp);
        }
    }
};

/** @brief Cograph recognition (original algorithm) */
inline CographResult check_cograph_cotree(const Graph& g) {
    CographResult res;
    CographChecker checker(g);
    res.is_cograph = checker.run();
    return res;
}

/** @brief Recursive cograph checker (fast version: partition refinement) */
class CographCheckerFast {
public:
    explicit CographCheckerFast(const Graph& graph)
        : g(graph),
          in_subset(graph.n + 1, 0),
          seen(graph.n + 1, 0),
          ll_nxt(graph.n + 1, 0),
          ll_prv(graph.n + 1, 0),
          in_remaining(graph.n + 1, 0),
          subset_token(0),
          seen_token(0) {}

    bool run() {
        std::vector<int> verts;
        verts.reserve(g.n);
        for (int v = 1; v <= g.n; ++v) verts.push_back(v);
        return solve(verts);
    }

private:
    const Graph& g;
    std::vector<long long> in_subset;
    std::vector<long long> seen;
    std::vector<int> ll_nxt;
    std::vector<int> ll_prv;
    std::vector<unsigned char> in_remaining;
    long long subset_token;
    long long seen_token;

    /* Iterative worklist instead of recursion (same rationale as
       CographChecker::solve above). */
    bool solve(const std::vector<int>& all_verts) {
        std::vector<std::vector<int>> pending;
        pending.push_back(all_verts);
        while (!pending.empty()) {
            std::vector<int> verts = std::move(pending.back());
            pending.pop_back();
            if ((int)verts.size() <= 1) continue;

            std::vector<std::vector<int>> comps;
            graph_components(verts, comps);
            if ((int)comps.size() > 1) {
                for (size_t i = 0; i < comps.size(); ++i) {
                    pending.push_back(std::move(comps[i]));
                }
                continue;
            }

            std::vector<std::vector<int>> cocomps;
            complement_components(verts, cocomps);
            if ((int)cocomps.size() > 1) {
                for (size_t i = 0; i < cocomps.size(); ++i) {
                    pending.push_back(std::move(cocomps[i]));
                }
                continue;
            }

            return false;
        }
        return true;
    }

    void graph_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) {
        comps.clear();
        subset_token++;
        seen_token++;
        for (size_t i = 0; i < verts.size(); ++i) {
            in_subset[verts[i]] = subset_token;
        }

        std::queue<int> q;
        for (size_t i = 0; i < verts.size(); ++i) {
            int s = verts[i];
            if (seen[s] == seen_token) continue;
            std::vector<int> comp;
            seen[s] = seen_token;
            q.push(s);
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                comp.push_back(v);
                for (size_t j = 0; j < g.adj[v].size(); ++j) {
                    int u = g.adj[v][j];
                    if (in_subset[u] != subset_token) continue;
                    if (seen[u] == seen_token) continue;
                    seen[u] = seen_token;
                    q.push(u);
                }
            }
            comps.push_back(comp);
        }
    }

    /** @brief Removes a vertex from the doubly-linked list */
    void ll_remove(int v) {
        ll_nxt[ll_prv[v]] = ll_nxt[v];
        ll_prv[ll_nxt[v]] = ll_prv[v];
        in_remaining[v] = 0;
    }

    /** @brief Inserts a vertex right after the sentinel in the doubly-linked list */
    void ll_insert_front(int v) {
        ll_nxt[v] = ll_nxt[0];
        ll_prv[v] = 0;
        ll_prv[ll_nxt[0]] = v;
        ll_nxt[0] = v;
        in_remaining[v] = 1;
    }

    /**
     * @brief Finds connected components of the complement graph efficiently
     *
     * Manages the remaining set using a doubly-linked list.
     * At each BFS step:
     *   1. Temporarily remove adjacent vertices of the dequeued v from remaining
     *   2. Move all vertices remaining (= complement graph neighbors) to the component
     *   3. Restore the temporarily removed vertices to remaining
     */
    void complement_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) {
        comps.clear();
        int k = (int)verts.size();
        if (k == 0) return;

        // Build doubly-linked list (sentinel = 0)
        int sentinel = 0;
        ll_nxt[sentinel] = verts[0];
        ll_prv[verts[0]] = sentinel;
        for (int i = 0; i < k - 1; ++i) {
            ll_nxt[verts[i]] = verts[i + 1];
            ll_prv[verts[i + 1]] = verts[i];
        }
        ll_nxt[verts[k - 1]] = sentinel;
        ll_prv[sentinel] = verts[k - 1];
        for (int i = 0; i < k; ++i) in_remaining[verts[i]] = 1;

        std::queue<int> q;
        std::vector<int> temp_removed;

        while (ll_nxt[sentinel] != sentinel) {
            int s = ll_nxt[sentinel];
            ll_remove(s);

            std::vector<int> comp;
            comp.push_back(s);
            q.push(s);

            while (!q.empty()) {
                int v = q.front();
                q.pop();

                // Step 1: Temporarily remove v's neighbors in G from remaining
                temp_removed.clear();
                for (size_t j = 0; j < g.adj[v].size(); ++j) {
                    int u = g.adj[v][j];
                    if (in_remaining[u]) {
                        ll_remove(u);
                        temp_removed.push_back(u);
                    }
                }

                // Step 2: Move all vertices remaining to component
                // (These are v's neighbors in the complement graph)
                while (ll_nxt[sentinel] != sentinel) {
                    int u = ll_nxt[sentinel];
                    ll_remove(u);
                    comp.push_back(u);
                    q.push(u);
                }

                // Step 3: Restore temporarily removed neighbors to remaining
                for (size_t j = 0; j < temp_removed.size(); ++j) {
                    ll_insert_front(temp_removed[j]);
                }
            }

            comps.push_back(comp);
        }
    }
};

/** @brief Cograph recognition (fast version: partition refinement) */
inline CographResult check_cograph_partition(const Graph& g) {
    CographResult res;
    CographCheckerFast checker(g);
    res.is_cograph = checker.run();
    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is a cograph
 * @param g Input graph
 * @param algo Algorithm selector (COTREE or PARTITION_REFINEMENT)
 * @return CographResult
 *
 * A graph is a cograph if every induced subgraph on 2 or more vertices is disconnected
 * or has a disconnected complement. Determined by recursively decomposing into
 * connected components / complement connected components.
 */
inline CographResult check_cograph(const Graph& g,
    CographAlgorithm algo = CographAlgorithm::PARTITION_REFINEMENT) {
    switch (algo) {
        case CographAlgorithm::COTREE:
            return detail::check_cograph_cotree(g);
        case CographAlgorithm::PARTITION_REFINEMENT:
            return detail::check_cograph_partition(g);
        default:
            break;
    }
    return CographResult();
}

} // namespace graph_recognition

#endif
