#ifndef GRAPH_RECOGNITION_MINOR_H
#define GRAPH_RECOGNITION_MINOR_H

/**
 * @file minor.h
 * @brief Fixed forbidden minor detection utility
 *
 * Determines the existence of a fixed small graph minor via the recursion
 *   H is a minor of G  <=>  H is a subgraph of G, or H is a minor of G/e for some edge e.
 * (In a minor model, either every branch set is a singleton — giving a subgraph —
 * or some branch set contains an edge that can be contracted.)
 */

#include "util/graph.h"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace graph_recognition {
namespace detail_minor {

/** @brief Type of small graph for minor detection */
enum class MinorTarget {
    K4,
    K5,
    K23,
    K33,
};

/** @brief 0-indexed simple graph state for minor search */
struct MinorState {
    int n;
    int m;
    std::vector<std::vector<unsigned char>> adj;
    std::vector<int> deg;
    /**
     * @brief groups[i] = the 1-indexed input vertices contracted into vertex i
     *
     * Each group is connected in the input graph: contraction only ever merges
     * two groups joined by an edge. Two groups are adjacent here exactly when
     * the input has an edge between them, so a target subgraph found on the
     * contracted state lifts to a minor model of the input.
     */
    std::vector<std::vector<int>> groups;
};

/** @brief Constructs a MinorState from a Graph */
inline MinorState build_minor_state(const Graph& g) {
    MinorState st;
    st.n = g.n;
    st.adj.assign(st.n, std::vector<unsigned char>(st.n, 0));
    st.deg.assign(st.n, 0);
    st.groups.assign(st.n, std::vector<int>());
    for (int i = 0; i < st.n; ++i) st.groups[i].push_back(i + 1);
    st.m = 0;

    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u >= v) continue;
            int a = u - 1;
            int b = v - 1;
            if (st.adj[a][b]) continue;
            st.adj[a][b] = 1;
            st.adj[b][a] = 1;
            st.deg[a]++;
            st.deg[b]++;
            st.m++;
        }
    }

    return st;
}

/** @brief Returns the state after contracting edge (u,v) */
inline MinorState contract_edge(const MinorState& st, int u, int v) {
    if (u > v) std::swap(u, v);

    MinorState next;
    next.n = st.n - 1;
    next.adj.assign(next.n, std::vector<unsigned char>(next.n, 0));
    next.deg.assign(next.n, 0);
    next.groups.assign(next.n, std::vector<int>());
    next.m = 0;

    std::vector<int> map_old(st.n, -1);
    int id = 0;
    for (int x = 0; x < st.n; ++x) {
        if (x == v) continue;
        map_old[x] = id++;
    }
    map_old[v] = map_old[u];

    // u and v share a slot, so their groups accumulate into the same set.
    for (int x = 0; x < st.n; ++x) {
        std::vector<int>& dst = next.groups[map_old[x]];
        dst.insert(dst.end(), st.groups[x].begin(), st.groups[x].end());
    }

    for (int a = 0; a < st.n; ++a) {
        for (int b = a + 1; b < st.n; ++b) {
            if (!st.adj[a][b]) continue;
            int na = map_old[a];
            int nb = map_old[b];
            if (na == nb) continue;
            if (next.adj[na][nb]) continue;
            next.adj[na][nb] = 1;
            next.adj[nb][na] = 1;
            next.deg[na]++;
            next.deg[nb]++;
            next.m++;
        }
    }

    return next;
}

/** @brief State serialization (memoization key, canonical form)
 *
 * Sorts vertices by decreasing degree and uses the renumbered adjacency matrix as a key.
 * Isomorphic graphs are more likely to map to the same key, improving cache hit rate.
 */
inline std::string serialize(const MinorState& st) {
    // Sort vertices by decreasing degree to build canonical ordering
    std::vector<int> perm(st.n);
    for (int i = 0; i < st.n; ++i) perm[i] = i;
    std::sort(perm.begin(), perm.end(), [&](int a, int b) {
        if (st.deg[a] != st.deg[b]) return st.deg[a] > st.deg[b];
        return a < b;
    });

    std::string key;
    key.reserve(4 + (size_t)st.n * (size_t)(st.n - 1) / 2);
    key.push_back((char)(st.n & 0xFF));
    key.push_back((char)((st.n >> 8) & 0xFF));
    key.push_back((char)((st.n >> 16) & 0xFF));
    key.push_back((char)((st.n >> 24) & 0xFF));
    for (int i = 0; i < st.n; ++i) {
        for (int j = i + 1; j < st.n; ++j) {
            key.push_back(st.adj[perm[i]][perm[j]] ? '\1' : '\0');
        }
    }
    return key;
}

inline bool clique_dfs(
    const MinorState& st,
    int k,
    int start,
    std::vector<int>* chosen) {
    if ((int)chosen->size() == k) return true;

    int need = k - (int)chosen->size();
    for (int v = start; v <= st.n - need; ++v) {
        if (st.deg[v] < k - 1) continue;

        bool ok = true;
        for (size_t i = 0; i < chosen->size(); ++i) {
            if (!st.adj[v][(*chosen)[i]]) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        chosen->push_back(v);
        if (clique_dfs(st, k, v + 1, chosen)) return true;
        chosen->pop_back();
    }

    return false;
}

/** @brief Finds a K_k subgraph and reports its vertices */
inline bool find_clique_k(const MinorState& st, int k, std::vector<int>* out) {
    if (st.n < k) return false;
    out->clear();
    out->reserve(k);
    if (clique_dfs(st, k, 0, out)) return true;
    out->clear();
    return false;
}

/** @brief Determines whether K_k is contained as a (non-induced) subgraph */
inline bool has_clique_k(const MinorState& st, int k) {
    std::vector<int> chosen;
    return find_clique_k(st, k, &chosen);
}

inline bool bipartite_complete_dfs(
    const MinorState& st,
    int a_size,
    int b_size,
    int start,
    std::vector<int>* a_set,
    std::vector<unsigned char>* in_a,
    std::vector<int>* b_set) {
    if ((int)a_set->size() == a_size) {
        b_set->clear();
        for (int v = 0; v < st.n; ++v) {
            if ((*in_a)[v]) continue;
            bool ok = true;
            for (size_t i = 0; i < a_set->size(); ++i) {
                if (!st.adj[v][(*a_set)[i]]) {
                    ok = false;
                    break;
                }
            }
            if (ok) b_set->push_back(v);
            if ((int)b_set->size() >= b_size) return true;
        }
        b_set->clear();
        return false;
    }

    int need = a_size - (int)a_set->size();
    for (int v = start; v <= st.n - need; ++v) {
        if (st.deg[v] < b_size) continue;
        (*a_set).push_back(v);
        (*in_a)[v] = 1;
        if (bipartite_complete_dfs(st, a_size, b_size, v + 1, a_set, in_a, b_set)) {
            return true;
        }
        (*in_a)[v] = 0;
        (*a_set).pop_back();
    }

    return false;
}

/** @brief Finds a K_{a,b} subgraph and reports both sides */
inline bool find_complete_bipartite(const MinorState& st, int a_size, int b_size,
                                    std::vector<int>* out_a, std::vector<int>* out_b) {
    out_a->clear();
    out_b->clear();
    if (st.n < a_size + b_size) return false;

    out_a->reserve(a_size);
    std::vector<unsigned char> in_a(st.n, 0);
    if (bipartite_complete_dfs(st, a_size, b_size, 0, out_a, &in_a, out_b)) return true;
    out_a->clear();
    out_b->clear();
    return false;
}

/** @brief Determines whether K_{a,b} is contained as a (non-induced) subgraph */
inline bool has_complete_bipartite(const MinorState& st, int a_size, int b_size) {
    std::vector<int> a_set, b_set;
    return find_complete_bipartite(st, a_size, b_size, &a_set, &b_set);
}

/** @brief Fixed small graph minor checker */
class MinorChecker {
public:
    explicit MinorChecker(MinorTarget target) : target_(target) {}

    bool has_minor(const MinorState& st) {
        return dfs(st);
    }

    /**
     * @brief Searches for a minor and reports the branch sets
     * @param st Search state, whose groups must still be the singletons
     *           build_minor_state() sets up
     * @param out Receives one vertex set per target vertex, in the target's
     *            canonical order (for K_{a,b}, the a-side first)
     * @return true if a model was found
     *
     * Kept separate from has_minor() because the shared memo stores only a
     * verdict. A cached "true" belongs to an isomorphic state whose groups
     * differ, so it cannot supply a model here; cached "false" is a property of
     * the structure alone and is still reused.
     */
    bool find_model(const MinorState& st, std::vector<std::vector<int>>* out) {
        return model_dfs(st, out);
    }

private:
    MinorTarget target_;
    std::unordered_map<std::string, unsigned char> memo_;
    std::unordered_map<std::string, unsigned char> dead_;

    bool target_subgraph_vertices(const MinorState& st, std::vector<int>* nodes) const {
        if (target_ == MinorTarget::K4) return find_clique_k(st, 4, nodes);
        if (target_ == MinorTarget::K5) return find_clique_k(st, 5, nodes);
        int a_size = (target_ == MinorTarget::K23) ? 2 : 3;
        std::vector<int> a_set, b_set;
        if (!find_complete_bipartite(st, a_size, 3, &a_set, &b_set)) return false;
        nodes->clear();
        nodes->insert(nodes->end(), a_set.begin(), a_set.end());
        nodes->insert(nodes->end(), b_set.begin(), b_set.end());
        return true;
    }

    bool model_dfs(const MinorState& st, std::vector<std::vector<int>>* out) {
        if (st.n < min_vertices()) return false;
        if (st.m < min_edges()) return false;

        std::vector<int> nodes;
        if (target_subgraph_vertices(st, &nodes)) {
            out->clear();
            for (size_t i = 0; i < nodes.size(); ++i) out->push_back(st.groups[nodes[i]]);
            return true;
        }
        if (st.n == min_vertices()) return false;

        std::string key = serialize(st);
        if (dead_.find(key) != dead_.end()) return false;

        for (int u = 0; u < st.n; ++u) {
            if (st.deg[u] == 0) continue;
            for (int v = u + 1; v < st.n; ++v) {
                if (!st.adj[u][v]) continue;
                if (model_dfs(contract_edge(st, u, v), out)) return true;
            }
        }
        dead_[key] = 1;
        return false;
    }

    int min_vertices() const {
        if (target_ == MinorTarget::K4) return 4;
        if (target_ == MinorTarget::K5) return 5;
        if (target_ == MinorTarget::K23) return 5;
        return 6; // K33
    }

    int min_edges() const {
        if (target_ == MinorTarget::K4) return 6;
        if (target_ == MinorTarget::K5) return 10;
        if (target_ == MinorTarget::K23) return 6;
        return 9; // K33
    }

    bool contains_target_subgraph(const MinorState& st) const {
        if (target_ == MinorTarget::K4) return has_clique_k(st, 4);
        if (target_ == MinorTarget::K5) return has_clique_k(st, 5);
        if (target_ == MinorTarget::K23) return has_complete_bipartite(st, 2, 3);
        return has_complete_bipartite(st, 3, 3);
    }

    bool dfs(const MinorState& st) {
        if (st.n < min_vertices()) return false;
        if (st.m < min_edges()) return false;

        if (contains_target_subgraph(st)) return true;
        /* No subgraph and no room to contract further */
        if (st.n == min_vertices()) return false;

        std::string key = serialize(st);
        std::unordered_map<std::string, unsigned char>::const_iterator it =
            memo_.find(key);
        if (it != memo_.end()) return it->second != 0;

        /* Branch contraction over every edge; a single chosen edge is not
           sufficient because that edge may need to be preserved as an H-edge
           of the minor model (e.g. K4 with one edge subdivided). */
        bool found = false;
        for (int u = 0; u < st.n && !found; ++u) {
            if (st.deg[u] == 0) continue;
            for (int v = u + 1; v < st.n && !found; ++v) {
                if (!st.adj[u][v]) continue;
                if (dfs(contract_edge(st, u, v))) found = true;
            }
        }

        memo_[key] = found ? 1 : 0;
        return found;
    }
};

} // namespace detail_minor
} // namespace graph_recognition

#endif
