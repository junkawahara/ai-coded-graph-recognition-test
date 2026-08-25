#ifndef GRAPH_RECOGNITION_DISTANCE_HEREDITARY_H
#define GRAPH_RECOGNITION_DISTANCE_HEREDITARY_H

/**
 * @file distance_hereditary.h
 * @brief Distance-hereditary graph recognition
 *
 * Algorithms:
 *   - HASHMAP_TWINS: Twin detection using hash map
 *   - SORTED_TWINS: Twin detection by sorted neighbor list comparison
 *   - HASH_TWINS: Incremental twin detection via XOR hash + exact verification (candidate buckets are rescanned per removal; worst case O(n^3) on dense graphs) (default)
 *   - SPLIT_DECOMPOSITION: every bag of Cunningham's split decomposition is
 *     degenerate (Bandelt & Mulder 1986). Slow, but shares nothing with the
 *     twin-elimination variants, which makes it a genuinely independent check.
 *
 * build_pruning_sequence() additionally reports the pruning sequence: which
 * vertex was removed at each step, whether as a pendant or as a twin, and of
 * which other vertex. It is a separate entry point rather than a field of
 * DistanceHereditaryResult because recording the witnesses costs more than
 * the fast recognizers pay to answer the question.
 */

#include "forbidden_subgraph.h"
#include "graph.h"
#include "obstruction_extract.h"
#include "split_decomposition.h"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for distance-hereditary graph recognition
 */
enum class DistanceHereditaryAlgorithm {
    HASHMAP_TWINS, /**< Twin detection using hash map */
    SORTED_TWINS,  /**< Twin detection by sorted neighbor list comparison */
    HASH_TWINS,    /**< Incremental twin detection via XOR hash + exact verification (candidate buckets are rescanned per removal; worst case O(n^3) on dense graphs) (default) */
    SPLIT_DECOMPOSITION /**< every bag of the split decomposition is degenerate */
};

/**
 * @brief Result of distance-hereditary graph recognition
 */
struct DistanceHereditaryResult {
    bool is_distance_hereditary = false; /**< true if the graph is distance-hereditary */
    Obstruction obstruction; /**< NO certificate: NON_SHORTEST_INDUCED_PATH. Left empty
                                  by every variant -- the twin eliminations end with a
                                  residual graph, not with a path -- so callers use
                                  build_distance_hereditary_obstruction(). Valid only
                                  when is_distance_hereditary == false */
};

/**
 * @brief One step of a pruning sequence
 */
struct PruningStep {
    int vertex = 0;  /**< the vertex removed */
    int witness = 0; /**< the neighbour it hung from, or its twin; 0 when isolated */
    int kind = 0;    /**< 0 isolated, 1 pendant, 2 true twin, 3 false twin */
};

/**
 * @brief Result of pruning sequence construction
 */
struct PruningSequenceResult {
    bool is_distance_hereditary = false; /**< true if the graph is distance-hereditary */
    /**
     * @brief The pruning steps, in the order they were applied
     *
     * Has n-1 entries for a graph on n >= 1 vertices: pruning stops with one
     * vertex left. Valid only when is_distance_hereditary == true.
     */
    std::vector<PruningStep> steps;
};

namespace detail {

/**
 * @brief Distance-hereditary graph recognition using hash map
 */
inline DistanceHereditaryResult check_distance_hereditary_hashmap(const Graph& g) {
    DistanceHereditaryResult res;
    res.is_distance_hereditary = true;
    int n = g.n;
    if (n <= 1) return res;

    std::vector<int> degree(n + 1, 0);
    for (int u = 1; u <= n; ++u) {
        degree[u] = (int)g.adj[u].size();
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
                    } else if (g.has_edge(v, u)) {
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
        for (size_t j = 0; j < g.adj[pick].size(); ++j) {
            int u = g.adj[pick][j];
            if (alive[u]) degree[u]--;
        }
    }

    return res;
}

/**
 * @brief Distance-hereditary graph recognition by sorted neighbor list comparison
 */
inline DistanceHereditaryResult check_distance_hereditary_sorted(const Graph& g) {
    DistanceHereditaryResult res;
    res.is_distance_hereditary = true;
    int n = g.n;
    if (n <= 1) return res;

    std::vector<int> degree(n + 1, 0);
    for (int u = 1; u <= n; ++u) {
        degree[u] = (int)g.adj[u].size();
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
                    if (u != v && g.has_edge(v, u)) {
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
        for (size_t j = 0; j < g.adj[pick].size(); ++j) {
            int u = g.adj[pick][j];
            if (alive[u]) degree[u]--;
        }
    }

    return res;
}

/**
 * @brief Incremental twin detection via XOR hash + exact verification (candidate buckets are rescanned per removal; worst case O(n^3) on dense graphs)
 *
 * Assigns a random 64-bit weight to each vertex.
 * open_hash[v] = XOR(weight[u] : u in N(v))
 * closed_hash[v] = open_hash[v] XOR weight[v]
 * Vertex pairs with the same hash are processed as twin candidates.
 * When a vertex is removed, neighbor hashes are updated via XOR in O(1).
 */
inline DistanceHereditaryResult check_distance_hereditary_hash(const Graph& g) {
    DistanceHereditaryResult res;
    res.is_distance_hereditary = true;
    int n = g.n;
    if (n <= 1) return res;

    // Adjacency list (dynamic)
    std::vector<std::vector<int>> adj(n + 1);
    std::vector<int> degree(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        adj[v] = g.adj[v];
        degree[v] = (int)adj[v].size();
    }

    // Random weights (generated by LCG, input-dependent seed)
    std::vector<unsigned long long> weight(n + 1);
    unsigned long long rng_state = 0x123456789ABCDEFULL ^ ((unsigned long long)n * 2654435761ULL);
    for (int v = 1; v <= n; ++v) {
        rng_state = rng_state * 6364136223846793005ULL + 1442695040888963407ULL;
        weight[v] = rng_state;
    }

    // Hash computation
    std::vector<unsigned long long> open_hash(n + 1, 0);
    std::vector<unsigned long long> closed_hash(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        for (size_t j = 0; j < adj[v].size(); ++j) {
            open_hash[v] ^= weight[adj[v][j]];
        }
        closed_hash[v] = open_hash[v] ^ weight[v];
    }

    // Hash -> vertex list map
    std::unordered_map<unsigned long long, std::vector<int>> open_map;
    std::unordered_map<unsigned long long, std::vector<int>> closed_map;
    open_map.reserve(n * 2 + 1);
    closed_map.reserve(n * 2 + 1);
    for (int v = 1; v <= n; ++v) {
        open_map[open_hash[v]].push_back(v);
        closed_map[closed_hash[v]].push_back(v);
    }

    std::vector<unsigned char> alive(n + 1, 1);
    // pendant queue (deg <= 1)
    std::vector<int> pendant_queue;
    for (int v = 1; v <= n; ++v) {
        if (degree[v] <= 1) pendant_queue.push_back(v);
    }

    int remaining = n;

    while (remaining > 1) {
        int pick = 0;

        // Prioritize pendant (deg <= 1) vertices
        while (!pendant_queue.empty() && pick == 0) {
            int v = pendant_queue.back();
            pendant_queue.pop_back();
            if (!alive[v] || degree[v] > 1) continue;
            pick = v;
        }

        // Twin detection: find alive pairs with same hash in open_map, exact neighbor comparison
        if (pick == 0) {
            for (std::unordered_map<unsigned long long, std::vector<int>>::iterator
                     it = open_map.begin(); it != open_map.end() && pick == 0; ++it) {
                std::vector<int>& vec = it->second;
                std::vector<int> av;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (alive[vec[i]]) av.push_back(vec[i]);
                }
                std::sort(av.begin(), av.end());
                av.erase(std::unique(av.begin(), av.end()), av.end());
                if (av.size() < 2) continue;
                // Build and compare alive neighbor lists for each candidate
                std::vector<std::vector<int>> nb(av.size());
                for (size_t i = 0; i < av.size(); ++i) {
                    for (size_t j2 = 0; j2 < adj[av[i]].size(); ++j2)
                        if (alive[adj[av[i]][j2]]) nb[i].push_back(adj[av[i]][j2]);
                    std::sort(nb[i].begin(), nb[i].end());
                }
                for (size_t i = 0; i < av.size() && pick == 0; ++i) {
                    for (size_t k = i + 1; k < av.size() && pick == 0; ++k) {
                        if (nb[i] == nb[k]) pick = av[k];
                    }
                }
            }
        }

        // Closed twin: exact comparison of N[u] = N[v]
        if (pick == 0) {
            for (std::unordered_map<unsigned long long, std::vector<int>>::iterator
                     it = closed_map.begin(); it != closed_map.end() && pick == 0; ++it) {
                std::vector<int>& vec = it->second;
                std::vector<int> av;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (alive[vec[i]]) av.push_back(vec[i]);
                }
                std::sort(av.begin(), av.end());
                av.erase(std::unique(av.begin(), av.end()), av.end());
                if (av.size() < 2) continue;
                std::vector<std::vector<int>> nb(av.size());
                for (size_t i = 0; i < av.size(); ++i) {
                    for (size_t j2 = 0; j2 < adj[av[i]].size(); ++j2)
                        if (alive[adj[av[i]][j2]]) nb[i].push_back(adj[av[i]][j2]);
                    nb[i].push_back(av[i]);
                    std::sort(nb[i].begin(), nb[i].end());
                }
                for (size_t i = 0; i < av.size() && pick == 0; ++i) {
                    for (size_t k = i + 1; k < av.size() && pick == 0; ++k) {
                        if (nb[i] == nb[k]) pick = av[k];
                    }
                }
            }
        }

        if (pick == 0) {
            res.is_distance_hereditary = false;
            return res;
        }

        // Remove pick
        alive[pick] = 0;
        remaining--;

        // Update neighbor hashes
        for (size_t j = 0; j < adj[pick].size(); ++j) {
            int u = adj[pick][j];
            if (!alive[u]) continue;

            unsigned long long old_oh = open_hash[u];
            unsigned long long old_ch = closed_hash[u];

            // Hash update
            open_hash[u] ^= weight[pick];
            closed_hash[u] = open_hash[u] ^ weight[u];
            degree[u]--;

            // Remove u from old hash bucket
            {
                std::vector<int>& vec = open_map[old_oh];
                for (size_t vi = 0; vi < vec.size(); ++vi) {
                    if (vec[vi] == u) { vec[vi] = vec.back(); vec.pop_back(); break; }
                }
            }
            {
                std::vector<int>& vec = closed_map[old_ch];
                for (size_t vi = 0; vi < vec.size(); ++vi) {
                    if (vec[vi] == u) { vec[vi] = vec.back(); vec.pop_back(); break; }
                }
            }

            // Add with new hash
            open_map[open_hash[u]].push_back(u);
            closed_map[closed_hash[u]].push_back(u);

            if (degree[u] <= 1) pendant_queue.push_back(u);
        }
    }

    return res;
}

/**
 * @brief Distance-hereditary recognition via the split decomposition
 *
 * Bandelt & Mulder (1986): a graph is distance-hereditary exactly when its
 * split decomposition has no prime bag.
 */
inline DistanceHereditaryResult check_distance_hereditary_split(const Graph& g) {
    DistanceHereditaryResult res;
    res.is_distance_hereditary = is_totally_decomposable(g);
    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is distance-hereditary
 * @param g Input graph
 * @param algo Algorithm to use (default: HASH_TWINS)
 * @return DistanceHereditaryResult
 */
inline DistanceHereditaryResult check_distance_hereditary(const Graph& g,
    DistanceHereditaryAlgorithm algo = DistanceHereditaryAlgorithm::HASH_TWINS) {
    switch (algo) {
        case DistanceHereditaryAlgorithm::HASHMAP_TWINS:
            return detail::check_distance_hereditary_hashmap(g);
        case DistanceHereditaryAlgorithm::SORTED_TWINS:
            return detail::check_distance_hereditary_sorted(g);
        case DistanceHereditaryAlgorithm::HASH_TWINS:
            return detail::check_distance_hereditary_hash(g);
        case DistanceHereditaryAlgorithm::SPLIT_DECOMPOSITION:
            return detail::check_distance_hereditary_split(g);
        default:
            break;
    }
    return DistanceHereditaryResult();
}

namespace detail {

/**
 * @brief Replays a pruning sequence against the graph
 *
 * Each step must be legal in the graph left at that point: an isolated vertex
 * has no live neighbour, a pendant exactly one, a true twin has the same
 * closed neighbourhood as its witness, and a false twin the same open one.
 */
inline bool pruning_sequence_is_valid(const Graph& g, const std::vector<PruningStep>& steps) {
    int n = g.n;
    if (n == 0) return steps.empty();
    if ((int)steps.size() != n - 1) return false;

    std::vector<unsigned char> alive(n + 1, 1);
    for (size_t i = 0; i < steps.size(); ++i) {
        int v = steps[i].vertex, w = steps[i].witness;
        if (v < 1 || v > n || !alive[v]) return false;

        std::vector<int> nbrs;
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            if (alive[g.adj[v][j]]) nbrs.push_back(g.adj[v][j]);
        }

        if (steps[i].kind == 0) {
            if (!nbrs.empty() || w != 0) return false;
        } else if (steps[i].kind == 1) {
            if (nbrs.size() != 1 || nbrs[0] != w) return false;
        } else if (steps[i].kind == 2 || steps[i].kind == 3) {
            if (w < 1 || w > n || !alive[w] || w == v) return false;
            bool adjacent = g.has_edge(v, w);
            if (adjacent != (steps[i].kind == 2)) return false;
            for (int u = 1; u <= n; ++u) {
                if (!alive[u] || u == v || u == w) continue;
                if (g.has_edge(v, u) != g.has_edge(w, u)) return false;
            }
        } else {
            return false;
        }
        alive[v] = 0;
    }

    int left = 0;
    for (int v = 1; v <= n; ++v) {
        if (alive[v]) ++left;
    }
    return left == 1;
}

} // namespace detail

/**
 * @brief Builds a pruning sequence of a distance-hereditary graph
 * @param g Input graph
 * @return PruningSequenceResult
 *
 * Distance-hereditary graphs are exactly the graphs that can be reduced to a
 * single vertex by repeatedly deleting a pendant vertex or one of a pair of
 * twins (Bandelt & Mulder 1986). This builds such a sequence and names the
 * witness of every step. Runs in O(n^3): each step compares the live
 * neighbourhoods of all remaining pairs.
 *
 * The sequence is replayed against the graph before being returned.
 */
inline PruningSequenceResult build_pruning_sequence(const Graph& g) {
    PruningSequenceResult res;
    int n = g.n;
    if (n == 0) {
        res.is_distance_hereditary = true;
        return res;
    }

    std::vector<unsigned char> alive(n + 1, 1);
    std::vector<PruningStep> steps;
    steps.reserve(n > 0 ? n - 1 : 0);

    for (int remaining = n; remaining > 1; --remaining) {
        std::vector<int> verts;
        verts.reserve(remaining);
        for (int v = 1; v <= n; ++v) {
            if (alive[v]) verts.push_back(v);
        }

        PruningStep step;
        bool found = false;

        // Pendant (or isolated) vertices first: they are the cheapest to spot.
        for (size_t i = 0; i < verts.size() && !found; ++i) {
            int v = verts[i];
            int live_nbrs = 0, last = 0;
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                if (!alive[g.adj[v][j]]) continue;
                ++live_nbrs;
                last = g.adj[v][j];
                if (live_nbrs > 1) break;
            }
            if (live_nbrs == 0) {
                step.vertex = v;
                step.witness = 0;
                step.kind = 0;
                found = true;
            } else if (live_nbrs == 1) {
                step.vertex = v;
                step.witness = last;
                step.kind = 1;
                found = true;
            }
        }

        for (size_t i = 0; i < verts.size() && !found; ++i) {
            for (size_t j = i + 1; j < verts.size() && !found; ++j) {
                int u = verts[i], v = verts[j];
                bool same = true;
                for (size_t t = 0; t < verts.size() && same; ++t) {
                    int w = verts[t];
                    if (w == u || w == v) continue;
                    if (g.has_edge(u, w) != g.has_edge(v, w)) same = false;
                }
                if (!same) continue;
                // Remove the later vertex and keep the earlier as the witness.
                step.vertex = v;
                step.witness = u;
                step.kind = g.has_edge(u, v) ? 2 : 3;
                found = true;
            }
        }

        if (!found) return res;
        steps.push_back(step);
        alive[step.vertex] = 0;
    }

    if (!detail::pruning_sequence_is_valid(g, steps)) return res;
    res.steps.swap(steps);
    res.is_distance_hereditary = true;
    return res;
}

namespace detail_dh_obstruction {

/**
 * @brief State of the induced-path search
 *
 * blocked[w] counts the path vertices adjacent to w, so w extends the induced
 * path exactly when blocked[w] == 1: adjacent to the current endpoint and to
 * nothing else already on it.
 */
struct PathSearch {
    const Graph& g;
    std::vector<int> blocked;
    std::vector<unsigned char> in_path;
    std::vector<int> path;
    std::vector<int> witness;
    int target;
    int limit;
    bool found;

    PathSearch(const Graph& g_, int t, int d)
        : g(g_), blocked(g_.n + 1, 0), in_path(g_.n + 1, 0),
          target(t), limit(d), found(false) {}
};

/** @brief Searches for an induced path to the target longer than the distance */
inline void search(PathSearch& s, int cur, int depth) {
    if (s.found) return;
    for (size_t i = 0; i < s.g.adj[cur].size(); ++i) {
        if (s.found) return;
        int w = s.g.adj[cur][i];
        if (s.in_path[w]) continue;

        if (w == s.target) {
            if (s.blocked[w] == 1 && depth + 1 > s.limit) {
                s.found = true;
                s.witness = s.path;
                s.witness.push_back(s.target);
                return;
            }
            continue;
        }
        if (s.blocked[w] != 1) continue;

        s.in_path[w] = 1;
        s.path.push_back(w);
        for (size_t j = 0; j < s.g.adj[w].size(); ++j) s.blocked[s.g.adj[w][j]]++;

        search(s, w, depth + 1);

        for (size_t j = 0; j < s.g.adj[w].size(); ++j) s.blocked[s.g.adj[w][j]]--;
        s.path.pop_back();
        s.in_path[w] = 0;
    }
}

} // namespace detail_dh_obstruction

/**
 * @brief Builds a NO certificate for a non-distance-hereditary graph
 * @param g Input graph
 * @return A NON_SHORTEST_INDUCED_PATH, or an empty obstruction if g is
 *         distance-hereditary
 *
 * Howorka's characterization: a graph is distance-hereditary exactly when every
 * induced path is a shortest path. So the witness is one induced u-v path
 * longer than d(u,v), and a verifier only has to recompute that distance.
 *
 * The path search backtracks and is exponential in the worst case, which is why
 * this is separate from check_distance_hereditary(): the twin-elimination
 * variants recognize in polynomial time and keep that bound.
 */
inline Obstruction build_distance_hereditary_obstruction(const Graph& g) {
    Obstruction o;
    int n = g.n;
    if (n < 4) return o;

    std::vector<std::vector<int> > dist(n + 1, std::vector<int>(n + 1, -1));
    for (int s = 1; s <= n; ++s) {
        dist[s][s] = 0;
        std::vector<int> queue(1, s);
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (dist[s][w] != -1) continue;
                dist[s][w] = dist[s][u] + 1;
                queue.push_back(w);
            }
        }
    }

    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (dist[u][v] == -1) continue;
            detail_dh_obstruction::PathSearch s(g, v, dist[u][v]);
            s.in_path[u] = 1;
            s.path.push_back(u);
            for (size_t i = 0; i < g.adj[u].size(); ++i) s.blocked[g.adj[u][i]]++;
            detail_dh_obstruction::search(s, u, 0);
            if (!s.found) continue;

            o.kind = ObstructionKind::NON_SHORTEST_INDUCED_PATH;
            o.vertices = s.witness;
            return o;
        }
    }
    return o;
}

} // namespace graph_recognition

#endif
