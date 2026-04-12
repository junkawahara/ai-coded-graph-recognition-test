#ifndef GRAPH_RECOGNITION_STRONGLY_REGULAR_ENUM_H
#define GRAPH_RECOGNITION_STRONGLY_REGULAR_ENUM_H

/**
 * @file strongly_regular_enum.h
 * @brief 強正則グラフの列挙 (パラメータ制約付きバックトラッキング)
 *
 * 頂点集合 {1, ..., n} 上のラベル付き強正則グラフを全列挙する。
 *
 * 強正則グラフ srg(n, k, λ, μ):
 *   - 全頂点の次数が k
 *   - 隣接頂点ペアの共通近傍数が λ
 *   - 非隣接頂点ペアの共通近傍数が μ
 *   - 完全グラフでも空グラフでもない (0 < k < n-1)
 *
 * アルゴリズム:
 *   1. 有効なパラメータ (k, λ, μ) を数学的条件でフィルタリング
 *      - 基本等式: k(k-λ-1) = μ(n-k-1)
 *      - 握手補題: n*k が偶数
 *      - 固有値整数性: 判別式と重複度の条件
 *   2. 各パラメータに対し、kregular_enum 型のバックトラッキングで構築
 *      - 共通近傍数 (cn) 行列を追跡し、上限・下限で枝刈り
 *
 * 参考文献:
 *   McKay, Spence, "Classification of regular two-graphs on 36 and 38 vertices,"
 *   Australas. J. Combin. 24, 2001
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief 強正則列挙アルゴリズムの選択
 */
enum class StronglyRegularEnumAlgorithm {
    BACKTRACK /**< パラメータ制約付きバックトラッキング */
};

/**
 * @brief 強正則列挙の結果
 */
struct StronglyRegularEnumerationResult {
    std::vector<EnumeratedGraph> graphs;
};

namespace detail {

/** @brief 強正則グラフのパラメータ (k, λ, μ) */
struct SRGParams {
    int k, lambda, mu;
};

/**
 * @brief 与えられた n に対し有効な srg パラメータを全列挙する
 *
 * フィルタ条件:
 *   1. 0 < k < n-1
 *   2. 0 <= lambda <= k-1, 0 <= mu <= k
 *   3. n*k が偶数
 *   4. k*(k-lambda-1) = mu*(n-k-1)
 *   5. 固有値整数性チェック
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

            // 固有値整数性チェック
            // Δ = (λ-μ)² + 4(k-μ)
            long long delta = (long long)(lam - mu) * (lam - mu)
                            + 4LL * (k - mu);
            if (delta < 0) continue;

            long long val = 2LL * k + (long long)(n - 1) * (lam - mu);

            if (val == 0) {
                // Conference graph: f = g = (n-1)/2
                if ((n - 1) % 2 != 0) continue;
                // 両方正 → n >= 3 → OK (n >= 4 は既に保証)
            } else {
                // Δ が完全平方数か
                long long d = 0;
                {
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

                // val が d で割り切れるか
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

/** @brief バックトラッキングの内部状態 */
struct SRGEnumState {
    int total_n;
    int target_k;
    int target_lambda;
    int target_mu;
    int alive_count;
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;
    std::vector<std::vector<int> > cn;  /**< cn[u][v] = 共通近傍数 */

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
        // 全頂点追加済み: 厳密検証
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

    // 候補: {v < x : deg[v] < k}
    std::vector<int> available;
    for (int v = 1; v < x; ++v) {
        if (state.deg[v] < k) {
            available.push_back(v);
        }
    }

    // x の近傍数の範囲
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
        // コミット: x を追加
        state.deg[x] = chosen_count;
        state.alive_count = x;

        // Phase 2: cn[u][x] を計算 (u < x)
        for (int u = 1; u < x; ++u) {
            int count = 0;
            for (int i = 0; i < chosen_count; ++i) {
                if (state.adj[u][chosen[i]]) ++count;
            }
            state.cn[u][x] = count;
            state.cn[x][u] = count;
        }

        // 制約チェック
        bool feasible = true;

        // 次数到達可能性
        for (int v = 1; v <= x && feasible; ++v) {
            if (state.deg[v] + remaining_after_x < k) feasible = false;
        }

        // 共通近傍数の上限・下限チェック (全ペア)
        for (int u = 1; u <= x && feasible; ++u) {
            for (int v = u + 1; v <= x && feasible; ++v) {
                int target = state.adj[u][v] ? lam : mu;
                if (state.cn[u][v] > target) {
                    feasible = false;
                    break;
                }
                // 下限: 今後追加可能な共通近傍数
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

    // 残り候補数で枝刈り
    int can_still = (int)available.size() - (int)start;
    if (chosen_count + can_still < min_size) return;

    for (std::size_t i = start; i < available.size(); ++i) {
        int v = available[i];
        if (state.deg[v] >= k) continue;

        // 辺 x-v を追加
        state.adj[x][v] = 1;
        state.adj[v][x] = 1;
        state.deg[v]++;

        // Phase 1: chosen 内の各 w に対し cn[w][v]++ (x が新共通近傍)
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
 * @brief 頂点集合 {1, ..., n} 上のラベル付き強正則グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は BACKTRACK のみ)
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
