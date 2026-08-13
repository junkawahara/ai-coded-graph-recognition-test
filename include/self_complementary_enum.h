#ifndef GRAPH_RECOGNITION_SELF_COMPLEMENTARY_ENUM_H
#define GRAPH_RECOGNITION_SELF_COMPLEMENTARY_ENUM_H

/**
 * @file self_complementary_enum.h
 * @brief Self-complementary graph enumeration (complementing permutation based)
 *
 * Enumerates all labeled self-complementary graphs on vertex set {1, ..., n}
 * using the structure of complementing permutations.
 *
 * Self-complementary graphs exist only when n is congruent to 0 or 1 (mod 4).
 * Unlabeled count: OEIS A000171
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "chordal_enum.h" /* EnumeratedGraph */

namespace graph_recognition {

/**
 * @brief Algorithm selection for self-complementary graph enumeration
 */
enum class SelfComplementaryEnumAlgorithm {
    COMPLEMENTING_PERMUTATION /**< Complementing permutation based */
};

/**
 * @brief Result of self-complementary graph enumeration
 */
struct SelfComplementaryEnumerationResult {
    std::vector<EnumeratedGraph> graphs;
};

namespace detail {

/* ------------------------------------------------------------------ */
/*  Enumeration of valid cycle type partitions                                          */
/* ------------------------------------------------------------------ */

/**
 * @brief Recursively enumerates valid cycle type partitions.
 *
 * Only cycle lengths that are powers of 2 (>= 4).
 * When n = 1 (mod 4), called with the fixed point (length 1) already removed.
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
 * @brief Returns valid cycle type partitions for n.
 *
 * Each partition is a list of cycle lengths (fixed points included as length 1).
 */
inline std::vector<std::vector<int> > sc_get_valid_cycle_types(int n) {
    std::vector<std::vector<int> > result;
    if (n % 4 != 0 && n % 4 != 1) return result;

    std::vector<int> current;
    if (n % 4 == 1) {
        /* One fixed point */
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
/*  Generate all permutations from cycle type                                          */
/* ------------------------------------------------------------------ */

/**
 * @brief Recursively generates all permutations by assigning vertices to each cycle of the cycle type.
 *
 * @param cycle_type  Sorted list of cycle lengths
 * @param idx         Index of the cycle currently being processed
 * @param perm        Permutation under construction (1-indexed, perm[i] = sigma(i))
 * @param used        Used vertices
 * @param out         List of generated permutations
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
        /* Fixed point: in the canonical representation (cycles sorted by
           their minimum element) this cycle must contain the smallest
           unused vertex. All positions of the fixed point are covered by
           enumerating every ordering of the cycle-length multiset. */
        for (int v = 1; v <= n; ++v) {
            if (!used[v]) {
                used[v] = true;
                perm[v] = v;
                sc_gen_perms(cycle_type, idx + 1, perm, used, out);
                used[v] = false;
                perm[v] = 0;
                break;
            }
        }
        return;
    }

    /* Cycle of length len: head = smallest unused vertex (the cycle's
       minimum element in the canonical representation) */
    int start = 0;
    for (int v = 1; v <= n; ++v) {
        if (!used[v]) { start = v; break; }
    }
    if (start == 0) return;

    used[start] = true;

    /* Choose len-1 vertices from unused vertices and try all permutations */
    std::vector<int> pool;
    for (int v = 1; v <= n; ++v) {
        if (!used[v]) pool.push_back(v);
    }

    /* Choose len-1 elements from pool and generate all permutations */
    std::vector<int> chosen(len - 1);
    std::vector<bool> pool_used(pool.size(), false);

    /* Recursively choose and place len-1 elements, implemented as a DFS loop */
    /* DFS: determine chosen[pos] */
    struct PermDFS {
        static void run(int pos, int len, int start,
                        const std::vector<int>& pool,
                        std::vector<bool>& pool_used,
                        std::vector<int>& chosen,
                        const std::vector<int>& cycle_type, int idx,
                        std::vector<int>& perm, std::vector<bool>& used,
                        std::vector<std::vector<int> >* out) {
            if (pos == len - 1) {
                /* Build cycle: start -> chosen[0] -> ... -> chosen[len-2] -> start */
                perm[start] = chosen[0];
                for (int i = 0; i < len - 2; ++i) {
                    perm[chosen[i]] = chosen[i + 1];
                }
                perm[chosen[len - 2]] = start;

                sc_gen_perms(cycle_type, idx + 1, perm, used, out);

                /* Undo */
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
 * @brief Generate all permutations from a cycle type.
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
/*  Vertex pair orbit computation                                                    */
/* ------------------------------------------------------------------ */

/**
 * @brief Computes the orbit of vertex pair {u,v} under permutation sigma.
 *
 * Each orbit is a list of pairs generated by repeated application of sigma.
 * Orbit size is even due to the property of complementing permutations.
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
            /* a,b are already normalized at the end of the loop, so comparison with (u,v) is correct */

            orbits.push_back(orbit);
        }
    }
    return orbits;
}

/* ------------------------------------------------------------------ */
/*  Generate all graphs from orbits                                              */
/* ------------------------------------------------------------------ */

/**
 * @brief Pre-compute even/odd index halves of orbits and
 *        generate 2^r edge sets. Duplicates are eliminated using seen.
 */
inline void sc_generate_graphs(
    int n,
    const std::vector<std::vector<std::pair<int, int> > >& orbits,
    std::set<std::vector<std::pair<int, int> > >* seen,
    std::vector<EnumeratedGraph>* out) {

    int r = static_cast<int>(orbits.size());

    /* Split each orbit into two halves */
    std::vector<std::vector<std::pair<int, int> > > half0(r), half1(r);
    for (int i = 0; i < r; ++i) {
        for (size_t j = 0; j < orbits[i].size(); ++j) {
            if (j % 2 == 0)
                half0[i].push_back(orbits[i][j]);
            else
                half1[i].push_back(orbits[i][j]);
        }
    }

    /* Enumerate 2^r choices */
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
/*  Public API                                                           */
/* ------------------------------------------------------------------ */

/**
 * @brief using the structure of complementing permutations.
 *
 * Constructive enumeration using the structure of complementing permutations.
 * Returns non-empty results only when n = 0 or 1 (mod 4).
 * For n = 0 the single empty graph is returned (check_self_complementary
 * accepts the empty graph); for n < 0 the result is empty.
 *
 * @param n   Number of vertices
 * @param algo Algorithm selection (currently only COMPLEMENTING_PERMUTATION)
 * @return Enumeration result
 */
inline SelfComplementaryEnumerationResult
enumerate_self_complementary_graphs(
    int n,
    SelfComplementaryEnumAlgorithm algo =
        SelfComplementaryEnumAlgorithm::COMPLEMENTING_PERMUTATION) {
    (void)algo;

    SelfComplementaryEnumerationResult result;

    if (n < 0) return result;
    if (n == 0) {
        /* The empty graph is self-complementary
           (check_self_complementary(n=0) is YES) */
        EnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    if (n % 4 != 0 && n % 4 != 1) return result;

    if (n == 1) {
        EnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    /* Enumerate valid cycle types */
    std::vector<std::vector<int> > cycle_types =
        detail::sc_get_valid_cycle_types(n);

    std::set<std::vector<std::pair<int, int> > > seen;

    for (size_t t = 0; t < cycle_types.size(); ++t) {
        /* Canonical representation: cycles sorted by their minimum element,
           each cycle headed by its minimum. The sequence of cycle lengths in
           that order can be ANY ordering of the multiset (e.g. the fixed
           point of an n = 1 (mod 4) permutation need not contain vertex 1,
           and for mixed types like [4,8] vertex 1 may lie in the 8-cycle),
           so enumerate every distinct ordering. */
        std::vector<int> ordering = cycle_types[t];
        std::sort(ordering.begin(), ordering.end());
        do {
            /* Generate all permutations for this cycle-length ordering */
            std::vector<std::vector<int> > perms =
                detail::sc_generate_all_permutations(n, ordering);

            for (size_t p = 0; p < perms.size(); ++p) {
                /* Compute vertex pair orbits */
                std::vector<std::vector<std::pair<int, int> > > orbits =
                    detail::sc_compute_pair_orbits(n, perms[p]);

                /* Generate graphs from orbits */
                detail::sc_generate_graphs(n, orbits, &seen, &result.graphs);
            }
        } while (std::next_permutation(ordering.begin(), ordering.end()));
    }

    return result;
}

} /* namespace graph_recognition */

#endif /* GRAPH_RECOGNITION_SELF_COMPLEMENTARY_ENUM_H */
