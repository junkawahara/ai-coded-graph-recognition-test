#ifndef GRAPH_RECOGNITION_STRONGLY_REGULAR_ENUM_H
#define GRAPH_RECOGNITION_STRONGLY_REGULAR_ENUM_H

/**
 * @file strongly_regular_enum.h
 * @brief Strongly regular graph enumeration (backtracking with parameter constraints)
 *
 * Enumerates all labeled strongly regular graphs on vertex set {1, ..., n}.
 *
 * Strongly regular graph srg(n, k, lambda, mu):
 *   - All vertices have degree k
 *   - Common neighbor count for adjacent vertex pairs is lambda
 *   - Common neighbor count for non-adjacent vertex pairs is mu
 *   - Neither complete nor empty graph (0 < k < n-1)
 *
 * Algorithm:
 *   1. Filter valid parameters (k, lambda, mu) by mathematical conditions
 *      - Basic equation: k(k-lambda-1) = mu(n-k-1)
 *      - Handshaking lemma: n*k must be even
 *      - Eigenvalue integrality: discriminant and multiplicity conditions
 *   2. For each parameter, construct via kregular_enum-style backtracking
 *      - Track common neighbor count (cn) matrix and prune by upper/lower bounds
 *
 * References:
 *   McKay, Spence, "Classification of regular two-graphs on 36 and 38 vertices,"
 *   Australas. J. Combin. 24, 2001
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for strongly regular graph enumeration
 */
enum class StronglyRegularEnumAlgorithm {
    BACKTRACK /**< Backtracking with parameter constraints */
};

/**
 * @brief Result of strongly regular graph enumeration
 */
struct StronglyRegularEnumerationResult {
    std::vector<EnumeratedGraph> graphs;
};

namespace detail {

/** @brief Strongly regular graph parameters (k, lambda, mu) */
struct SRGParams {
    int k, lambda, mu;
};

/**
 * @brief Enumerate all valid srg parameters for given n
 *
 * Filter conditions:
 *   1. 0 < k < n-1
 *   2. 0 <= lambda <= k-1, 0 <= mu <= k
 *   3. n*k is even
 *   4. k*(k-lambda-1) = mu*(n-k-1)
 *   5. Eigenvalue integrality check
 */
inline std::vector<SRGParams> srg_enumerate_params(int n) {
    std::vector<SRGParams> params;
    if (n < 4) return params;

    for (int k = 1; k <= n - 2; ++k) {
        if ((long long)n * k % 2 != 0) continue;

        int denom = n - k - 1;  // > 0 since k < n-1

        for (int lam = 0; lam <= k - 1; ++lam) {
            long long num = (long long)k * (k - lam - 1);
            if (num < 0) continue;
            if (num % denom != 0) continue;
            int mu = (int)(num / denom);
            if (mu < 0 || mu > k) continue;

            // Eigenvalue integrality check
            // Δ = (λ-μ)² + 4(k-μ)
            long long delta = (long long)(lam - mu) * (lam - mu)
                            + 4LL * (k - mu);
            if (delta < 0) continue;

            long long val = 2LL * k + (long long)(n - 1) * (lam - mu);

            if (val == 0) {
                // Conference graph: f = g = (n-1)/2
                if ((n - 1) % 2 != 0) continue;
                // Both positive -> n >= 3 -> OK (n >= 4 already guaranteed)
            } else {
                // Is Delta a perfect square?
                long long d = 0;
                {
                    // Cap the search at 2e9 so that mid*mid stays well within
                    // long long range (4e18 < 9.2e18 = LLONG_MAX).
                    long long lo = 0, hi = delta;
                    if (hi > (long long)2e9) hi = (long long)2e9;
                    while (lo <= hi) {
                        long long mid = lo + (hi - lo) / 2;
                        long long sq = mid * mid;
                        if (sq == delta) { d = mid; break; }
                        if (sq < delta) lo = mid + 1;
                        else hi = mid - 1;
                    }
                }
                if (d * d != delta) continue;
                if (d == 0) continue;

                // Check if val is divisible by d
                if (val % d != 0) continue;
                long long quot = val / d;

                // f = (n-1 + quot) / 2, g = n-1 - f
                long long f_num = (n - 1) + quot;
                if (f_num % 2 != 0) continue;
                long long f = f_num / 2;
                long long g = (n - 1) - f;
                if (f <= 0 || g <= 0) continue;
            }

            SRGParams p;
            p.k = k;
            p.lambda = lam;
            p.mu = mu;
            params.push_back(p);
        }
    }
    return params;
}

/** @brief Internal state for backtracking */
struct SRGEnumState {
    int total_n;
    int target_k;
    int target_lambda;
    int target_mu;
    int alive_count;
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;
    std::vector<std::vector<int> > cn;  /**< cn[u][v] = number of common neighbors */

    SRGEnumState(int n, int k, int lam, int mu)
        : total_n(n), target_k(k), target_lambda(lam), target_mu(mu),
          alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0),
          cn(n + 1, std::vector<int>(n + 1, 0)) {}
};

inline void srg_enum_dfs(SRGEnumState& state,
                         std::vector<EnumeratedGraph>* out);

inline void srg_enum_choose(SRGEnumState& state,
                            const std::vector<int>& available,
                            std::size_t start,
                            int chosen_count,
                            int min_size, int max_size,
                            std::vector<int>& chosen,
                            std::vector<EnumeratedGraph>* out);

inline void srg_enum_dfs(SRGEnumState& state,
                         std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        // All vertices added: strict verification
        int n = state.total_n;
        int k = state.target_k;
        int lam = state.target_lambda;
        int mu = state.target_mu;

        for (int v = 1; v <= n; ++v) {
            if (state.deg[v] != k) return;
        }
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                int target = state.adj[u][v] ? lam : mu;
                if (state.cn[u][v] != target) return;
            }
        }

        EnumeratedGraph graph;
        graph.n = n;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                if (state.adj[u][v])
                    graph.edges.push_back(std::make_pair(u, v));
        out->push_back(graph);
        return;
    }

    int x = state.alive_count + 1;
    int k = state.target_k;
    int remaining = state.total_n - x;

    // Candidates: {v < x : deg[v] < k}
    std::vector<int> available;
    for (int v = 1; v < x; ++v) {
        if (state.deg[v] < k) {
            available.push_back(v);
        }
    }

    // Range for number of x's neighbors
    int min_neighbors = k - remaining;
    if (min_neighbors < 0) min_neighbors = 0;
    int max_neighbors = k;
    if (max_neighbors > (int)available.size()) max_neighbors = (int)available.size();

    if (max_neighbors < min_neighbors) return;

    std::vector<int> chosen;
    chosen.reserve(max_neighbors);
    srg_enum_choose(state, available, 0, 0,
                    min_neighbors, max_neighbors, chosen, out);
}

inline void srg_enum_choose(SRGEnumState& state,
                            const std::vector<int>& available,
                            std::size_t start,
                            int chosen_count,
                            int min_size, int max_size,
                            std::vector<int>& chosen,
                            std::vector<EnumeratedGraph>* out) {
    int x = state.alive_count + 1;
    int n = state.total_n;
    int k = state.target_k;
    int lam = state.target_lambda;
    int mu = state.target_mu;
    int remaining_after_x = n - x;

    if (chosen_count >= min_size) {
        // Commit: add vertex x
        state.deg[x] = chosen_count;
        state.alive_count = x;

        // Phase 2: compute cn[u][x] (u < x)
        for (int u = 1; u < x; ++u) {
            int count = 0;
            for (int i = 0; i < chosen_count; ++i) {
                if (state.adj[u][chosen[i]]) ++count;
            }
            state.cn[u][x] = count;
            state.cn[x][u] = count;
        }

        // Constraint check
        bool feasible = true;

        // Degree reachability
        for (int v = 1; v <= x && feasible; ++v) {
            if (state.deg[v] + remaining_after_x < k) feasible = false;
        }

        // Upper/lower bound check of common neighbor count (all pairs)
        for (int u = 1; u <= x && feasible; ++u) {
            for (int v = u + 1; v <= x && feasible; ++v) {
                int target = state.adj[u][v] ? lam : mu;
                if (state.cn[u][v] > target) {
                    feasible = false;
                    break;
                }
                // Lower bound: common neighbors that can still be added
                int rem_u = k - state.deg[u];
                int rem_v = k - state.deg[v];
                int future = rem_u < rem_v ? rem_u : rem_v;
                if (future > remaining_after_x) future = remaining_after_x;
                if (state.cn[u][v] + future < target) {
                    feasible = false;
                    break;
                }
            }
        }

        if (feasible) {
            srg_enum_dfs(state, out);
        }

        // Undo Phase 2
        for (int u = 1; u < x; ++u) {
            state.cn[u][x] = 0;
            state.cn[x][u] = 0;
        }
        state.alive_count = x - 1;
        state.deg[x] = 0;
    }

    if (chosen_count == max_size) return;

    // Prune by remaining candidate count
    int can_still = (int)available.size() - (int)start;
    if (chosen_count + can_still < min_size) return;

    for (std::size_t i = start; i < available.size(); ++i) {
        int v = available[i];
        if (state.deg[v] >= k) continue;

        // Add edge x-v
        state.adj[x][v] = 1;
        state.adj[v][x] = 1;
        state.deg[v]++;

        // Phase 1: for each w in chosen, cn[w][v]++ (x is a new common neighbor)
        bool prune = false;
        int updated = 0;
        for (int j = 0; j < chosen_count; ++j) {
            int w = chosen[j];
            state.cn[w][v]++;
            state.cn[v][w]++;
            ++updated;
            int target = state.adj[w][v] ? lam : mu;
            if (state.cn[w][v] > target) {
                prune = true;
                break;
            }
        }

        if (!prune) {
            chosen.push_back(v);
            srg_enum_choose(state, available, i + 1, chosen_count + 1,
                            min_size, max_size, chosen, out);
            chosen.pop_back();
        }

        // Undo Phase 1
        for (int j = 0; j < updated; ++j) {
            int w = chosen[j];
            state.cn[w][v]--;
            state.cn[v][w]--;
        }

        state.adj[x][v] = 0;
        state.adj[v][x] = 0;
        state.deg[v]--;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled strongly regular graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only BACKTRACK)
 * @return StronglyRegularEnumerationResult
 */
inline StronglyRegularEnumerationResult
enumerate_strongly_regular_graphs(int n,
    StronglyRegularEnumAlgorithm algo =
        StronglyRegularEnumAlgorithm::BACKTRACK) {
    (void)algo;
    StronglyRegularEnumerationResult result;
    if (n < 4) return result;

    std::vector<detail::SRGParams> params = detail::srg_enumerate_params(n);

    for (std::size_t pi = 0; pi < params.size(); ++pi) {
        const detail::SRGParams& p = params[pi];
        detail::SRGEnumState state(n, p.k, p.lambda, p.mu);
        detail::srg_enum_dfs(state, &result.graphs);
    }

    return result;
}

}  // namespace graph_recognition

#endif
