#ifndef GRAPH_RECOGNITION_DISTANCE_HEREDITARY_H
#define GRAPH_RECOGNITION_DISTANCE_HEREDITARY_H

/**
 * @file distance_hereditary.h
 * @brief 距離遺伝グラフ (distance-hereditary graph) 認識
 *
 * アルゴリズム:
 *   - HASHMAP_TWINS: ハッシュマップによるツイン検出
 *   - SORTED_TWINS: ソート済み近傍リスト比較によるツイン検出 (デフォルト)
 */

#include "graph.h"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace graph_recognition {

/**
 * @brief 距離遺伝グラフ認識アルゴリズムの選択
 */
enum class DistanceHereditaryAlgorithm {
    HASHMAP_TWINS, /**< ハッシュマップによるツイン検出 */
    SORTED_TWINS   /**< ソート済み近傍リスト比較によるツイン検出 (デフォルト) */
};

/**
 * @brief 距離遺伝グラフ認識の結果
 */
struct DistanceHereditaryResult {
    bool is_distance_hereditary; /**< 距離遺伝グラフであれば true */
};

namespace detail {

/**
 * @brief ハッシュマップによる距離遺伝グラフ認識
 */
inline DistanceHereditaryResult check_distance_hereditary_hashmap(const Graph& g) {
    DistanceHereditaryResult res;
    res.is_distance_hereditary = true;
    int n = g.n;
    if (n <= 1) return res;

    std::vector<std::vector<unsigned char>> adj(
        n + 1, std::vector<unsigned char>(n + 1, 0));
    std::vector<int> degree(n + 1, 0);
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (!g.has_edge(u, v)) continue;
            adj[u][v] = 1;
            adj[v][u] = 1;
            degree[u]++;
            degree[v]++;
        }
    }

    std::vector<unsigned char> alive(n + 1, 1);
    int remaining = n;

    while (remaining > 1) {
        int pick = 0;

        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] <= 1) {
                pick = v;
                break;
            }
        }

        if (pick == 0) {
            std::vector<int> verts;
            verts.reserve(remaining);
            for (int v = 1; v <= n; ++v) {
                if (alive[v]) verts.push_back(v);
            }

            std::unordered_map<std::string, int> open_seen;
            std::unordered_map<std::string, int> closed_seen;
            open_seen.reserve(verts.size() * 2 + 1);
            closed_seen.reserve(verts.size() * 2 + 1);

            for (size_t i = 0; i < verts.size() && pick == 0; ++i) {
                int v = verts[i];
                std::string open_sig;
                std::string closed_sig;
                open_sig.reserve(verts.size());
                closed_sig.reserve(verts.size());

                for (size_t j = 0; j < verts.size(); ++j) {
                    int u = verts[j];
                    if (u == v) {
                        open_sig.push_back('0');
                        closed_sig.push_back('1');
                    } else if (adj[v][u]) {
                        open_sig.push_back('1');
                        closed_sig.push_back('1');
                    } else {
                        open_sig.push_back('0');
                        closed_sig.push_back('0');
                    }
                }

                if (open_seen.find(open_sig) != open_seen.end()) {
                    pick = v;
                    break;
                }
                open_seen.insert(std::make_pair(open_sig, v));

                if (closed_seen.find(closed_sig) != closed_seen.end()) {
                    pick = v;
                    break;
                }
                closed_seen.insert(std::make_pair(closed_sig, v));
            }
        }

        if (pick == 0) {
            res.is_distance_hereditary = false;
            return res;
        }

        alive[pick] = 0;
        remaining--;
        for (int u = 1; u <= n; ++u) {
            if (!alive[u]) continue;
            if (adj[pick][u]) degree[u]--;
        }
    }

    return res;
}

/**
 * @brief ソート済み近傍リスト比較による距離遺伝グラフ認識
 */
inline DistanceHereditaryResult check_distance_hereditary_sorted(const Graph& g) {
    DistanceHereditaryResult res;
    res.is_distance_hereditary = true;
    int n = g.n;
    if (n <= 1) return res;

    std::vector<std::vector<unsigned char>> adj(
        n + 1, std::vector<unsigned char>(n + 1, 0));
    std::vector<int> degree(n + 1, 0);
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (!g.has_edge(u, v)) continue;
            adj[u][v] = 1;
            adj[v][u] = 1;
            degree[u]++;
            degree[v]++;
        }
    }

    std::vector<unsigned char> alive(n + 1, 1);
    int remaining = n;

    while (remaining > 1) {
        int pick = 0;

        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] <= 1) {
                pick = v;
                break;
            }
        }

        if (pick == 0) {
            std::vector<int> verts;
            verts.reserve(remaining);
            for (int v = 1; v <= n; ++v) {
                if (alive[v]) verts.push_back(v);
            }

            std::vector<std::vector<int>> open_nb(n + 1);
            for (size_t i = 0; i < verts.size(); ++i) {
                int v = verts[i];
                open_nb[v].reserve(degree[v]);
                for (size_t j = 0; j < verts.size(); ++j) {
                    int u = verts[j];
                    if (u != v && adj[v][u]) {
                        open_nb[v].push_back(u);
                    }
                }
            }

            std::vector<int> by_open(verts);
            std::sort(by_open.begin(), by_open.end(),
                      [&](int a, int b) { return open_nb[a] < open_nb[b]; });
            for (size_t i = 1; i < by_open.size(); ++i) {
                if (open_nb[by_open[i]] == open_nb[by_open[i - 1]]) {
                    pick = by_open[i];
                    break;
                }
            }

            if (pick == 0) {
                std::vector<std::vector<int>> closed_nb(n + 1);
                for (size_t i = 0; i < verts.size(); ++i) {
                    int v = verts[i];
                    closed_nb[v] = open_nb[v];
                    std::vector<int>::iterator it =
                        std::lower_bound(closed_nb[v].begin(), closed_nb[v].end(), v);
                    closed_nb[v].insert(it, v);
                }

                std::vector<int> by_closed(verts);
                std::sort(by_closed.begin(), by_closed.end(),
                          [&](int a, int b) { return closed_nb[a] < closed_nb[b]; });
                for (size_t i = 1; i < by_closed.size(); ++i) {
                    if (closed_nb[by_closed[i]] == closed_nb[by_closed[i - 1]]) {
                        pick = by_closed[i];
                        break;
                    }
                }
            }
        }

        if (pick == 0) {
            res.is_distance_hereditary = false;
            return res;
        }

        alive[pick] = 0;
        remaining--;
        for (int u = 1; u <= n; ++u) {
            if (!alive[u]) continue;
            if (adj[pick][u]) degree[u]--;
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief グラフが距離遺伝グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: SORTED_TWINS)
 * @return DistanceHereditaryResult
 */
inline DistanceHereditaryResult check_distance_hereditary(const Graph& g,
    DistanceHereditaryAlgorithm algo = DistanceHereditaryAlgorithm::SORTED_TWINS) {
    switch (algo) {
        case DistanceHereditaryAlgorithm::HASHMAP_TWINS:
            return detail::check_distance_hereditary_hashmap(g);
        case DistanceHereditaryAlgorithm::SORTED_TWINS:
            return detail::check_distance_hereditary_sorted(g);
    }
    return DistanceHereditaryResult();
}

} // namespace graph_recognition

#endif
