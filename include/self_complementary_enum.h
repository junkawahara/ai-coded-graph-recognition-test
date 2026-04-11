#ifndef GRAPH_RECOGNITION_SELF_COMPLEMENTARY_ENUM_H
#define GRAPH_RECOGNITION_SELF_COMPLEMENTARY_ENUM_H

/**
 * @file self_complementary_enum.h
 * @brief 自己補的グラフの列挙 (補置換ベース)
 *
 * 補置換 (complementing permutation) の構造を利用して
 * 頂点集合 {1, ..., n} 上のラベル付き自己補的グラフを全列挙する。
 *
 * 自己補的グラフは n ≡ 0 or 1 (mod 4) のときのみ存在する。
 * 非ラベル付き数列: OEIS A000171
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "chordal_enum.h" /* EnumeratedGraph */

namespace graph_recognition {

/**
 * @brief 自己補的グラフ列挙アルゴリズムの選択
 */
enum class SelfComplementaryEnumAlgorithm {
    COMPLEMENTING_PERMUTATION /**< 補置換ベース */
};

/**
 * @brief 自己補的グラフ列挙の結果
 */
struct SelfComplementaryEnumerationResult {
    std::vector<EnumeratedGraph> graphs;
};

namespace detail {

/* ------------------------------------------------------------------ */
/*  有効なサイクル型分割の列挙                                          */
/* ------------------------------------------------------------------ */

/**
 * @brief 有効なサイクル型分割を再帰的に列挙する。
 *
 * サイクル長は 2 の冪 (>= 4) のみ。
 * n ≡ 1 (mod 4) の場合、固定点 (長さ 1) が既に除かれた状態で呼ばれる。
 */
inline void sc_enum_partitions(int remaining, int min_len,
                               std::vector<int>& current,
                               std::vector<std::vector<int> >* out) {
    if (remaining == 0) {
        out->push_back(current);
        return;
    }
    for (int len = (min_len < 4 ? 4 : min_len); len <= remaining; len *= 2) {
        current.push_back(len);
        sc_enum_partitions(remaining - len, len, current, out);
        current.pop_back();
    }
}

/**
 * @brief n に対する有効なサイクル型分割を返す。
 *
 * 各分割は cycle length のリスト (固定点は長さ 1 として含む)。
 */
inline std::vector<std::vector<int> > sc_get_valid_cycle_types(int n) {
    std::vector<std::vector<int> > result;
    if (n % 4 != 0 && n % 4 != 1) return result;

    std::vector<int> current;
    if (n % 4 == 1) {
        /* 固定点 1 つ */
        current.push_back(1);
        std::vector<std::vector<int> > partitions;
        sc_enum_partitions(n - 1, 4, current, &partitions);
        result.swap(partitions);
    } else {
        sc_enum_partitions(n, 4, current, &result);
    }
    return result;
}

/* ------------------------------------------------------------------ */
/*  サイクル型から全置換を生成                                          */
/* ------------------------------------------------------------------ */

/**
 * @brief サイクル型の各サイクルに頂点を割り当てて全置換を再帰的に生成する。
 *
 * @param cycle_type  サイクル長のリスト (ソート済み)
 * @param idx         現在処理中のサイクルのインデックス
 * @param perm        構築中の置換 (1-indexed, perm[i] = σ(i))
 * @param used        使用済み頂点
 * @param out         生成された置換のリスト
 */
inline void sc_gen_perms(const std::vector<int>& cycle_type, int idx,
                         std::vector<int>& perm, std::vector<bool>& used,
                         std::vector<std::vector<int> >* out) {
    int n = static_cast<int>(perm.size()) - 1;
    if (idx == static_cast<int>(cycle_type.size())) {
        out->push_back(perm);
        return;
    }

    int len = cycle_type[idx];

    if (len == 1) {
        /* 固定点: 最小の未使用頂点を割り当て */
        for (int v = 1; v <= n; ++v) {
            if (!used[v]) {
                used[v] = true;
                perm[v] = v;
                sc_gen_perms(cycle_type, idx + 1, perm, used, out);
                used[v] = false;
                perm[v] = 0;
                break; /* 固定点は 1 つだけ。最小を選んで対称性を排除 */
            }
        }
        return;
    }

    /* 長さ len のサイクル: 最小の未使用頂点を先頭に固定 */
    int start = 0;
    for (int v = 1; v <= n; ++v) {
        if (!used[v]) { start = v; break; }
    }
    if (start == 0) return;

    /* 同長の直前サイクルの先頭より大きいことを保証（自動的に満たされる） */
    used[start] = true;

    /* 残りの len-1 頂点を未使用頂点から選んで全順列を試す */
    std::vector<int> pool;
    for (int v = 1; v <= n; ++v) {
        if (!used[v]) pool.push_back(v);
    }

    /* pool から len-1 個を選んで全順列を生成 */
    std::vector<int> chosen(len - 1);
    std::vector<bool> pool_used(pool.size(), false);

    /* 再帰的に len-1 個を選んで配置する内部関数をループで実装 */
    /* DFS: chosen[pos] を決定する */
    struct PermDFS {
        static void run(int pos, int len, int start,
                        const std::vector<int>& pool,
                        std::vector<bool>& pool_used,
                        std::vector<int>& chosen,
                        const std::vector<int>& cycle_type, int idx,
                        std::vector<int>& perm, std::vector<bool>& used,
                        std::vector<std::vector<int> >* out) {
            if (pos == len - 1) {
                /* サイクルを構築: start -> chosen[0] -> ... -> chosen[len-2] -> start */
                perm[start] = chosen[0];
                for (int i = 0; i < len - 2; ++i) {
                    perm[chosen[i]] = chosen[i + 1];
                }
                perm[chosen[len - 2]] = start;

                sc_gen_perms(cycle_type, idx + 1, perm, used, out);

                /* 元に戻す */
                perm[start] = 0;
                for (int i = 0; i < len - 1; ++i) {
                    perm[chosen[i]] = 0;
                }
                return;
            }
            for (size_t i = 0; i < pool.size(); ++i) {
                if (!pool_used[i]) {
                    pool_used[i] = true;
                    used[pool[i]] = true;
                    chosen[pos] = pool[i];
                    run(pos + 1, len, start, pool, pool_used, chosen,
                        cycle_type, idx, perm, used, out);
                    pool_used[i] = false;
                    used[pool[i]] = false;
                }
            }
        }
    };

    PermDFS::run(0, len, start, pool, pool_used, chosen,
                 cycle_type, idx, perm, used, out);

    used[start] = false;
}

/**
 * @brief サイクル型から全置換を生成する。
 */
inline std::vector<std::vector<int> > sc_generate_all_permutations(
    int n, const std::vector<int>& cycle_type) {
    std::vector<std::vector<int> > result;
    std::vector<int> perm(n + 1, 0);
    std::vector<bool> used(n + 1, false);
    sc_gen_perms(cycle_type, 0, perm, used, &result);
    return result;
}

/* ------------------------------------------------------------------ */
/*  頂点対の軌道計算                                                    */
/* ------------------------------------------------------------------ */

/**
 * @brief 置換 σ の下での頂点対 {u,v} の軌道を計算する。
 *
 * 各軌道は σ の反復適用による対のリスト。
 * 補置換の性質により軌道サイズは偶数。
 */
inline std::vector<std::vector<std::pair<int, int> > >
sc_compute_pair_orbits(int n, const std::vector<int>& perm) {
    std::vector<std::vector<char> > visited(
        n + 1, std::vector<char>(n + 1, 0));
    std::vector<std::vector<std::pair<int, int> > > orbits;

    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (visited[u][v]) continue;

            std::vector<std::pair<int, int> > orbit;
            int a = u, b = v;
            do {
                if (a > b) { int tmp = a; a = b; b = tmp; }
                visited[a][b] = 1;
                orbit.push_back(std::make_pair(a, b));
                int na = perm[a], nb = perm[b];
                a = na;
                b = nb;
                if (a > b) { int tmp = a; a = b; b = tmp; }
            } while (a != u || b != v);
            /* a,b はループ末尾で正規化済みなので (u,v) との比較は正しい */

            orbits.push_back(orbit);
        }
    }
    return orbits;
}

/* ------------------------------------------------------------------ */
/*  軌道から全グラフを生成                                              */
/* ------------------------------------------------------------------ */

/**
 * @brief 軌道の偶数/奇数インデックス半分を事前計算し、
 *        2^r 通りの辺集合を生成する。重複は seen で排除。
 */
inline void sc_generate_graphs(
    int n,
    const std::vector<std::vector<std::pair<int, int> > >& orbits,
    std::set<std::vector<std::pair<int, int> > >* seen,
    std::vector<EnumeratedGraph>* out) {

    int r = static_cast<int>(orbits.size());

    /* 各軌道を 2 つの半分に分割 */
    std::vector<std::vector<std::pair<int, int> > > half0(r), half1(r);
    for (int i = 0; i < r; ++i) {
        for (size_t j = 0; j < orbits[i].size(); ++j) {
            if (j % 2 == 0)
                half0[i].push_back(orbits[i][j]);
            else
                half1[i].push_back(orbits[i][j]);
        }
    }

    /* 2^r 通りの選択を列挙 */
    unsigned long long limit = 1ULL << r;
    for (unsigned long long mask = 0; mask < limit; ++mask) {
        std::vector<std::pair<int, int> > edges;
        for (int i = 0; i < r; ++i) {
            const std::vector<std::pair<int, int> >& h =
                ((mask >> i) & 1) ? half1[i] : half0[i];
            for (size_t j = 0; j < h.size(); ++j) {
                edges.push_back(h[j]);
            }
        }
        std::sort(edges.begin(), edges.end());

        if (seen->count(edges)) continue;
        seen->insert(edges);

        EnumeratedGraph g;
        g.n = n;
        g.edges = edges;
        out->push_back(g);
    }
}

} /* namespace detail */

/* ------------------------------------------------------------------ */
/*  公開 API                                                           */
/* ------------------------------------------------------------------ */

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き自己補的グラフを全列挙する。
 *
 * 補置換 (complementing permutation) の構造を利用した構成的列挙。
 * n ≡ 0 or 1 (mod 4) のときのみ非空の結果を返す。
 *
 * @param n   頂点数
 * @param algo アルゴリズム選択 (現在は COMPLEMENTING_PERMUTATION のみ)
 * @return 列挙結果
 */
inline SelfComplementaryEnumerationResult
enumerate_self_complementary_graphs(
    int n,
    SelfComplementaryEnumAlgorithm algo =
        SelfComplementaryEnumAlgorithm::COMPLEMENTING_PERMUTATION) {
    (void)algo;

    SelfComplementaryEnumerationResult result;

    if (n <= 0) return result;
    if (n % 4 != 0 && n % 4 != 1) return result;

    if (n == 1) {
        EnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    /* 有効なサイクル型を列挙 */
    std::vector<std::vector<int> > cycle_types =
        detail::sc_get_valid_cycle_types(n);

    std::set<std::vector<std::pair<int, int> > > seen;

    for (size_t t = 0; t < cycle_types.size(); ++t) {
        /* このサイクル型の全置換を生成 */
        std::vector<std::vector<int> > perms =
            detail::sc_generate_all_permutations(n, cycle_types[t]);

        for (size_t p = 0; p < perms.size(); ++p) {
            /* 頂点対の軌道を計算 */
            std::vector<std::vector<std::pair<int, int> > > orbits =
                detail::sc_compute_pair_orbits(n, perms[p]);

            /* 軌道からグラフを生成 */
            detail::sc_generate_graphs(n, orbits, &seen, &result.graphs);
        }
    }

    return result;
}

} /* namespace graph_recognition */

#endif /* GRAPH_RECOGNITION_SELF_COMPLEMENTARY_ENUM_H */
