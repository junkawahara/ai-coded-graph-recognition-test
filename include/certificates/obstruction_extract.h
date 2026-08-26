#ifndef GRAPH_RECOGNITION_OBSTRUCTION_EXTRACT_H
#define GRAPH_RECOGNITION_OBSTRUCTION_EXTRACT_H

/**
 * @file obstruction_extract.h
 * @brief Shared extraction routines for NO-side certificates
 *
 * Recognizers detect a violation and hold the offending vertices at that
 * moment; turning those into a verifiable structure (a hole, an induced P4, a
 * 2K2) is the same work in every class. These are the shared routines.
 *
 * Every function here runs only on the NO path of a recognizer, so its cost is
 * paid only by graphs outside the class.
 *
 * Key building block: a shortest path between two vertices of an induced
 * subgraph is an induced path. Restricting a BFS to the complement of a closed
 * neighborhood therefore yields a chordless cycle for free -- no separate
 * shortcutting pass is needed.
 */

#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include <algorithm>
#include <queue>
#include <vector>

namespace graph_recognition {
namespace detail_obstruction {

/**
 * @brief Shortest path from s to t through allowed vertices
 * @param g Input graph
 * @param s Source vertex
 * @param t Target vertex
 * @param allowed Size >= n+1 mask; vertex w may be used iff allowed[w] != 0
 * @return Vertices from s to t inclusive, or an empty vector if t is unreachable
 *
 * The returned path is a shortest path of the subgraph induced on the allowed
 * set, hence an induced path of that subgraph (and therefore of g restricted to
 * those vertices).
 */
inline std::vector<int> shortest_path_in_allowed(const Graph& g, int s, int t,
                                                 const std::vector<unsigned char>& allowed) {
    std::vector<int> path;
    if (s < 1 || s > g.n || t < 1 || t > g.n) return path;
    if (!allowed[s] || !allowed[t]) return path;
    if (s == t) {
        path.push_back(s);
        return path;
    }

    std::vector<int> parent(g.n + 1, 0);
    std::vector<unsigned char> seen(g.n + 1, 0);
    std::queue<int> q;
    seen[s] = 1;
    q.push(s);
    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        for (size_t i = 0; i < g.adj[cur].size(); ++i) {
            int w = g.adj[cur][i];
            if (!allowed[w] || seen[w]) continue;
            seen[w] = 1;
            parent[w] = cur;
            if (w == t) {
                for (int x = t; x != 0; x = parent[x]) path.push_back(x);
                std::reverse(path.begin(), path.end());
                return path;
            }
            q.push(w);
        }
    }
    return path;
}

/**
 * @brief Rebuilds a root-to-y path from a BFS parent array
 * @param parent parent[w] = predecessor of w, 0 at a root
 * @param y Endpoint
 * @return Vertices from the root down to y inclusive
 */
inline std::vector<int> path_from_bfs_parents(const std::vector<int>& parent, int y) {
    std::vector<int> path;
    for (int x = y; x != 0; x = parent[x]) path.push_back(x);
    std::reverse(path.begin(), path.end());
    return path;
}

/**
 * @brief Extracts a hole through a vertex and two of its non-adjacent neighbors
 * @param g Input graph
 * @param v Center vertex
 * @param x Neighbor of v
 * @param y Neighbor of v, non-adjacent to x
 * @return The hole in cyclic order starting at v, or empty if x and y are
 *         separated by the closed neighborhood of v
 *
 * The interior of the returned cycle avoids N[v] entirely, so v has no chord
 * into it, and the interior is a shortest path of the restricted graph, so it
 * has no chord of its own. The cycle has length >= 4 because xy is a non-edge.
 */
inline std::vector<int> hole_through_center(const Graph& g, int v, int x, int y) {
    std::vector<int> hole;
    if (v < 1 || v > g.n || x < 1 || x > g.n || y < 1 || y > g.n) return hole;
    if (x == y || !g.has_edge(v, x) || !g.has_edge(v, y) || g.has_edge(x, y)) return hole;

    std::vector<unsigned char> allowed(g.n + 1, 1);
    allowed[0] = 0;
    allowed[v] = 0;
    for (size_t i = 0; i < g.adj[v].size(); ++i) allowed[g.adj[v][i]] = 0;
    allowed[x] = 1;
    allowed[y] = 1;

    std::vector<int> path = shortest_path_in_allowed(g, x, y, allowed);
    if (path.size() < 3) return hole;

    hole.push_back(v);
    for (size_t i = 0; i < path.size(); ++i) hole.push_back(path[i]);
    return hole;
}

/**
 * @brief Finds a hole anywhere in the graph
 * @param g Input graph
 * @return A chordless cycle of length >= 4, or empty if g is chordal
 *
 * Complete: the result is non-empty exactly when g is not chordal. For each
 * vertex v it computes the components of G - N[v] once, then looks for a
 * component seen by two non-adjacent neighbors of v; every hole of g yields
 * such a triple at each of its vertices.
 *
 * Costs O(n(n+m) + n*d^2) with d the maximum degree, so it is used only as the
 * fallback of hole_from_failed_peo(), never on a path where a cheaper local
 * extraction already succeeded.
 */
inline std::vector<int> find_hole(const Graph& g) {
    std::vector<int> hole;
    std::vector<unsigned char> blocked(g.n + 1, 0);
    std::vector<int> comp(g.n + 1, -1);
    std::vector<int> stamp;

    for (int v = 1; v <= g.n; ++v) {
        if (g.adj[v].size() < 2) continue;

        std::fill(blocked.begin(), blocked.end(), 0);
        std::fill(comp.begin(), comp.end(), -1);
        blocked[v] = 1;
        for (size_t i = 0; i < g.adj[v].size(); ++i) blocked[g.adj[v][i]] = 1;

        int label = 0;
        std::queue<int> q;
        for (int u = 1; u <= g.n; ++u) {
            if (blocked[u] || comp[u] >= 0) continue;
            comp[u] = label;
            q.push(u);
            while (!q.empty()) {
                int cur = q.front();
                q.pop();
                for (size_t i = 0; i < g.adj[cur].size(); ++i) {
                    int w = g.adj[cur][i];
                    if (blocked[w] || comp[w] >= 0) continue;
                    comp[w] = label;
                    q.push(w);
                }
            }
            ++label;
        }
        if (label == 0) continue;

        // touch[c]: neighbors of v having a neighbor in component c of G - N[v].
        std::vector<std::vector<int>> touch(label);
        stamp.assign(label, 0);
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int x = g.adj[v][i];
            for (size_t j = 0; j < g.adj[x].size(); ++j) {
                int c = comp[g.adj[x][j]];
                if (c < 0 || stamp[c] == x) continue;
                stamp[c] = x;
                touch[c].push_back(x);
            }
        }

        for (int c = 0; c < label; ++c) {
            for (size_t i = 0; i < touch[c].size(); ++i) {
                for (size_t j = i + 1; j < touch[c].size(); ++j) {
                    int x = touch[c][i], y = touch[c][j];
                    if (g.has_edge(x, y)) continue;
                    hole = hole_through_center(g, v, x, y);
                    if (!hole.empty()) return hole;
                }
            }
        }
    }
    return hole;
}

/**
 * @brief Extracts the hole certifying a failed perfect elimination ordering
 * @param g Input graph
 * @param v Vertex whose later-neighborhood failed the test
 * @param x First later-neighbor (the parent of v in the elimination forest)
 * @param y Later-neighbor of v that is not adjacent to x
 * @return A chordless cycle of length >= 4; never empty for a non-chordal graph
 *
 * Two tiers: the local extraction through v, which succeeds whenever x and y
 * stay connected outside N[v], and find_hole() as a complete fallback. The
 * Tarjan--Yannakakis test fails only on non-chordal graphs, so the fallback
 * always succeeds.
 *
 * The local tier is the one that runs: over all graphs on up to 7 vertices it
 * carried every one of the 4438431 failing runs of the three variants, matching
 * the classical argument that the failure point lies on a hole through v. The
 * fallback costs more than recognition does, so it stays as a safety net rather
 * than the expected path.
 */
inline std::vector<int> hole_from_failed_peo(const Graph& g, int v, int x, int y) {
    std::vector<int> hole = hole_through_center(g, v, x, y);
    if (!hole.empty()) return hole;
    return find_hole(g);
}

/**
 * @brief Closes a BFS path into a hole through the edge uv
 * @param u One endpoint of the edge the hole runs through
 * @param v The other endpoint
 * @param x Start of the path, a neighbour of u
 * @param y End of the path, a neighbour of v
 * @param par BFS parent array of the search that reached y from x
 * @return u, x, (path interior), y, v in cyclic order, or empty if the parent
 *         chain does not lead back to x
 *
 * The shape every hole detector in this library ends up with: an edge uv, a
 * neighbour of each end that misses the other end, and a search from one to
 * the other that avoided N[u] and N[v]. The interior therefore has no chord to
 * u or v, and being a shortest path it has none of its own.
 */
inline std::vector<int> hole_from_bfs_path(int u, int v, int x, int y,
                                           const std::vector<int>& par) {
    std::vector<int> path;
    for (int w = y; w != 0; w = par[w]) {
        path.push_back(w);
        if (w == x) break;
    }
    if (path.empty() || path.back() != x) return std::vector<int>();
    std::reverse(path.begin(), path.end());

    std::vector<int> hole;
    hole.reserve(path.size() + 2);
    hole.push_back(u);
    for (size_t i = 0; i < path.size(); ++i) hole.push_back(path[i]);
    hole.push_back(v);
    return hole;
}

/**
 * @brief Wraps an extracted cycle as an obstruction of the given kind
 * @param cycle Cycle in cyclic order, possibly empty
 * @param kind Kind to report
 * @param in_complement true if the cycle lives in the complement
 * @return The obstruction, or an empty one (kind NONE) if the cycle is empty
 *
 * Extraction routines return an empty vector when they find nothing; turning
 * that into a witness-carrying obstruction would produce a certificate that
 * fails verification, so the emptiness is preserved here instead.
 */
inline Obstruction cycle_obstruction(const std::vector<int>& cycle, ObstructionKind kind,
                                     bool in_complement = false) {
    Obstruction o;
    if (cycle.empty()) return o;
    o.kind = kind;
    o.vertices = cycle;
    o.in_complement = in_complement;
    return o;
}

/**
 * @brief Builds an odd cycle from a 2-coloring conflict
 * @param parent BFS parent array (0 at the root of each component)
 * @param a Endpoint of the conflicting edge
 * @param b Other endpoint, colored like a
 * @return A simple odd cycle through the edge ab, or empty if a and b lie in
 *         different BFS trees
 *
 * The cycle is not required to be chordless: any odd cycle certifies
 * non-bipartiteness. Its length is odd because equal colors force equal depth
 * parity, so the two root paths meet after an even total number of steps.
 */
inline std::vector<int> odd_cycle_from_conflict(const std::vector<int>& parent, int a, int b) {
    std::vector<int> cyc;
    if (a == b) return cyc;

    std::vector<int> pa, pb;
    for (int x = a; x != 0; x = parent[x]) pa.push_back(x);
    for (int x = b; x != 0; x = parent[x]) pb.push_back(x);
    if (pa.empty() || pb.empty()) return cyc;

    size_t k = 0;
    while (k < pa.size() && k < pb.size() && pa[pa.size() - 1 - k] == pb[pb.size() - 1 - k]) ++k;
    if (k == 0) return cyc;

    for (size_t i = 0; i + k <= pa.size(); ++i) cyc.push_back(pa[i]);
    for (size_t i = pb.size() - k; i-- > 0;) cyc.push_back(pb[i]);
    return cyc;
}

/**
 * @brief Finds an induced P4 inside a vertex subset
 * @param g Input graph
 * @param verts Vertices to search within
 * @return {a, b, c, d} inducing the path a-b-c-d, or empty if none exists
 *
 * Enumerates the middle edge bc and looks for a in N(b)\\N[c] and d in
 * N(c)\\N[b] that are non-adjacent; every induced P4 has that shape, so the
 * search is complete on the subset.
 */
inline std::vector<int> find_induced_p4_in(const Graph& g, const std::vector<int>& verts) {
    std::vector<int> res;
    std::vector<unsigned char> inside(g.n + 1, 0);
    for (size_t i = 0; i < verts.size(); ++i) {
        int v = verts[i];
        if (v >= 1 && v <= g.n) inside[v] = 1;
    }

    for (size_t bi = 0; bi < verts.size(); ++bi) {
        int b = verts[bi];
        if (b < 1 || b > g.n || !inside[b]) continue;
        for (size_t ci = 0; ci < g.adj[b].size(); ++ci) {
            int c = g.adj[b][ci];
            if (!inside[c]) continue;
            for (size_t ai = 0; ai < g.adj[b].size(); ++ai) {
                int a = g.adj[b][ai];
                if (!inside[a] || a == c || g.has_edge(a, c)) continue;
                for (size_t di = 0; di < g.adj[c].size(); ++di) {
                    int d = g.adj[c][di];
                    if (!inside[d] || d == b || d == a) continue;
                    if (g.has_edge(d, b) || g.has_edge(a, d)) continue;
                    res.push_back(a);
                    res.push_back(b);
                    res.push_back(c);
                    res.push_back(d);
                    return res;
                }
            }
        }
    }
    return res;
}

/**
 * @brief Finds an induced diamond (K4 minus an edge)
 * @param g Input graph
 * @return {u, v, w1, w2} where u, v are the degree-3 vertices and w1, w2 the
 *         non-adjacent pair, or empty if g is diamond-free
 *
 * Scans each edge uv for two non-adjacent common neighbours, which is exactly
 * the definition of a diamond on {u, v, w1, w2}.
 */
inline std::vector<int> find_diamond(const Graph& g) {
    std::vector<int> res;
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u >= v) continue;

            std::vector<int> common;
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int w = g.adj[u][j];
                if (w != v && g.has_edge(v, w)) common.push_back(w);
            }
            for (size_t a = 0; a < common.size(); ++a) {
                for (size_t b = a + 1; b < common.size(); ++b) {
                    if (g.has_edge(common[a], common[b])) continue;
                    res.push_back(u);
                    res.push_back(v);
                    res.push_back(common[a]);
                    res.push_back(common[b]);
                    return res;
                }
            }
        }
    }
    return res;
}

/**
 * @brief Finds an induced P4 anywhere in the graph
 * @param g Input graph
 * @return {a, b, c, d} inducing a-b-c-d, or empty if g is P4-free
 */
inline std::vector<int> find_induced_p4(const Graph& g) {
    std::vector<int> all;
    all.reserve(g.n);
    for (int v = 1; v <= g.n; ++v) all.push_back(v);
    return find_induced_p4_in(g, all);
}

/**
 * @brief Finds an induced claw
 * @param g Input graph
 * @return {centre, leaf, leaf, leaf}, or empty if g is claw-free
 */
inline std::vector<int> find_claw(const Graph& g) {
    std::vector<int> res;
    for (int c = 1; c <= g.n; ++c) {
        const std::vector<int>& nbrs = g.adj[c];
        if (nbrs.size() < 3) continue;
        for (size_t i = 0; i < nbrs.size(); ++i) {
            for (size_t j = i + 1; j < nbrs.size(); ++j) {
                if (g.has_edge(nbrs[i], nbrs[j])) continue;
                for (size_t k = j + 1; k < nbrs.size(); ++k) {
                    if (g.has_edge(nbrs[i], nbrs[k])) continue;
                    if (g.has_edge(nbrs[j], nbrs[k])) continue;
                    res.push_back(c);
                    res.push_back(nbrs[i]);
                    res.push_back(nbrs[j]);
                    res.push_back(nbrs[k]);
                    return res;
                }
            }
        }
    }
    return res;
}

/**
 * @brief Finds an induced bull
 * @param g Input graph
 * @return {a, b, c, x, y} with triangle abc, pendant x on a and pendant y on b,
 *         or empty if g is bull-free
 */
inline std::vector<int> find_bull(const Graph& g) {
    std::vector<int> res;
    if (g.n < 5) return res;

    for (int a = 1; a <= g.n; ++a) {
        for (size_t bi = 0; bi < g.adj[a].size(); ++bi) {
            int b = g.adj[a][bi];
            if (b <= a) continue;
            for (size_t ci = 0; ci < g.adj[a].size(); ++ci) {
                int c = g.adj[a][ci];
                if (c == b || !g.has_edge(b, c)) continue;
                // Triangle abc; c plays the degree-2 role, so the pendants hang
                // off a and b and must miss everything else.
                for (size_t xi = 0; xi < g.adj[a].size(); ++xi) {
                    int x = g.adj[a][xi];
                    if (x == b || x == c) continue;
                    if (g.has_edge(x, b) || g.has_edge(x, c)) continue;
                    for (size_t yi = 0; yi < g.adj[b].size(); ++yi) {
                        int y = g.adj[b][yi];
                        if (y == a || y == c || y == x) continue;
                        if (g.has_edge(y, a) || g.has_edge(y, c) || g.has_edge(y, x)) continue;
                        res.push_back(a);
                        res.push_back(b);
                        res.push_back(c);
                        res.push_back(x);
                        res.push_back(y);
                        return res;
                    }
                }
            }
        }
    }
    return res;
}

/**
 * @brief Finds an induced gem
 * @param g Input graph
 * @return {a, b, c, d, apex} where a-b-c-d is an induced path and apex sees all
 *         four, or empty if g is gem-free
 *
 * A gem is an induced P4 inside the neighbourhood of a vertex, so the search
 * reuses find_induced_p4_in() on each neighbourhood.
 */
inline std::vector<int> find_gem(const Graph& g) {
    std::vector<int> res;
    for (int v = 1; v <= g.n; ++v) {
        if (g.adj[v].size() < 4) continue;
        std::vector<int> p4 = find_induced_p4_in(g, g.adj[v]);
        if (p4.empty()) continue;
        res = p4;
        res.push_back(v);
        return res;
    }
    return res;
}

/**
 * @brief Finds an induced P5
 * @param g Input graph
 * @return {a, b, c, d, e} inducing the path a-b-c-d-e, or empty if g is P5-free
 */
inline std::vector<int> find_p5(const Graph& g) {
    std::vector<int> res;
    for (int b = 1; b <= g.n; ++b) {
        for (size_t ci = 0; ci < g.adj[b].size(); ++ci) {
            int c = g.adj[b][ci];
            for (size_t ai = 0; ai < g.adj[b].size(); ++ai) {
                int a = g.adj[b][ai];
                if (a == c || g.has_edge(a, c)) continue;
                for (size_t di = 0; di < g.adj[c].size(); ++di) {
                    int d = g.adj[c][di];
                    if (d == b || d == a) continue;
                    if (g.has_edge(d, b) || g.has_edge(d, a)) continue;
                    for (size_t ei = 0; ei < g.adj[d].size(); ++ei) {
                        int e = g.adj[d][ei];
                        if (e == c || e == b || e == a) continue;
                        if (g.has_edge(e, c) || g.has_edge(e, b) || g.has_edge(e, a)) continue;
                        res.push_back(a);
                        res.push_back(b);
                        res.push_back(c);
                        res.push_back(d);
                        res.push_back(e);
                        return res;
                    }
                }
            }
        }
    }
    return res;
}

/**
 * @brief Finds a Gamma-forcing chain from an arc back to its own reverse
 * @param g Input graph, or its complement for the co-comparability side
 * @return A FORCING_CYCLE obstruction, or an empty one if g is a comparability
 *         graph
 *
 * Two arcs of a transitive orientation force each other when they share their
 * tail and their heads are non-adjacent, or share their head and their tails
 * are non-adjacent: orienting a->b with a->c and bc a non-edge would otherwise
 * give c->a->b and force the missing edge cb. The relation is symmetric, so a
 * breadth-first search from one arc explores its whole implication class, and
 * by Golumbic's theorem a graph fails to be a comparability graph exactly when
 * some class contains an arc together with its reverse.
 *
 * The chain is built from the Gamma relation alone, never from a solver's
 * propagation trail: a trail mixes in transitivity steps and arcs fixed by
 * earlier classes, which no independent verifier could replay.
 *
 * A failed search rules out every arc it reached, since implication classes
 * partition the arcs, so the whole scan stays linear in the number of arcs
 * times the maximum degree.
 */
inline Obstruction find_forcing_cycle(const Graph& g) {
    Obstruction o;
    int n = g.n;
    if (n < 2) return o;

    const size_t stride = static_cast<size_t>(n) + 1;
    const size_t arc_count = stride * stride;
    std::vector<int> parent(arc_count, -1);
    std::vector<unsigned char> settled(arc_count, 0);
    std::vector<unsigned char> seen(arc_count, 0);
    std::vector<size_t> touched;

    for (int a = 1; a <= n; ++a) {
        for (size_t ai = 0; ai < g.adj[a].size(); ++ai) {
            int b = g.adj[a][ai];
            size_t seed = static_cast<size_t>(a) * stride + b;
            if (settled[seed]) continue;

            size_t goal = static_cast<size_t>(b) * stride + a;
            touched.clear();
            std::vector<size_t> queue;
            seen[seed] = 1;
            parent[seed] = -1;
            touched.push_back(seed);
            queue.push_back(seed);

            size_t hit = 0;
            bool found = false;
            for (size_t qi = 0; qi < queue.size() && !found; ++qi) {
                int x = static_cast<int>(queue[qi] / stride);
                int y = static_cast<int>(queue[qi] % stride);

                for (int side = 0; side < 2 && !found; ++side) {
                    const std::vector<int>& nbrs = (side == 0) ? g.adj[x] : g.adj[y];
                    for (size_t i = 0; i < nbrs.size(); ++i) {
                        int z = nbrs[i];
                        size_t next;
                        if (side == 0) {
                            // Same tail x, heads y and z non-adjacent.
                            if (z == y || g.has_edge(y, z)) continue;
                            next = static_cast<size_t>(x) * stride + z;
                        } else {
                            // Same head y, tails x and z non-adjacent.
                            if (z == x || g.has_edge(x, z)) continue;
                            next = static_cast<size_t>(z) * stride + y;
                        }
                        if (seen[next]) continue;
                        seen[next] = 1;
                        parent[next] = static_cast<int>(queue[qi]);
                        touched.push_back(next);
                        if (next == goal) {
                            hit = next;
                            found = true;
                            break;
                        }
                        queue.push_back(next);
                    }
                }
            }

            if (found) {
                std::vector<size_t> chain;
                for (int cur = static_cast<int>(hit); cur >= 0; cur = parent[cur]) {
                    chain.push_back(static_cast<size_t>(cur));
                }
                std::reverse(chain.begin(), chain.end());
                o.kind = ObstructionKind::FORCING_CYCLE;
                for (size_t i = 0; i < chain.size(); ++i) {
                    o.vertices.push_back(static_cast<int>(chain[i] / stride));
                    o.vertices.push_back(static_cast<int>(chain[i] % stride));
                }
                return o;
            }

            // The class this arc belongs to holds no reversal; skip all of it.
            for (size_t i = 0; i < touched.size(); ++i) {
                seen[touched[i]] = 0;
                settled[touched[i]] = 1;
            }
        }
    }
    return o;
}

/**
 * @brief Classifies the obstruction hidden in a non-nested neighborhood pair
 * @param g Input graph
 * @param u First vertex
 * @param v Second vertex
 * @param x A vertex in N(u) \\ N[v]
 * @param y A vertex in N(v) \\ N[u]
 * @return A TWO_K2, C4 or P4 obstruction on {u, v, x, y}
 *
 * The four vertices always induce one of those three patterns, selected by the
 * two remaining pairs uv and xy. This is the shared core of the threshold,
 * chain and split witnesses: all three classes forbid exactly the patterns that
 * a non-nested pair can produce.
 */
inline Obstruction pattern_from_non_nested(const Graph& g, int u, int v, int x, int y) {
    Obstruction o;
    bool uv = g.has_edge(u, v);
    bool xy = g.has_edge(x, y);
    if (uv && xy) {
        // x-u-v-y-x is a chordless 4-cycle.
        o.kind = ObstructionKind::C4;
        o.vertices.push_back(x);
        o.vertices.push_back(u);
        o.vertices.push_back(v);
        o.vertices.push_back(y);
    } else if (uv && !xy) {
        // x-u-v-y is an induced path.
        o.kind = ObstructionKind::P4;
        o.vertices.push_back(x);
        o.vertices.push_back(u);
        o.vertices.push_back(v);
        o.vertices.push_back(y);
    } else if (!uv && xy) {
        // u-x-y-v is an induced path.
        o.kind = ObstructionKind::P4;
        o.vertices.push_back(u);
        o.vertices.push_back(x);
        o.vertices.push_back(y);
        o.vertices.push_back(v);
    } else {
        // Only the edges ux and vy survive.
        o.kind = ObstructionKind::TWO_K2;
        o.vertices.push_back(u);
        o.vertices.push_back(x);
        o.vertices.push_back(v);
        o.vertices.push_back(y);
    }
    return o;
}

/**
 * @brief Maps a hole to a split-graph obstruction
 * @param hole Chordless cycle, in g when complement_side is false, in the
 *             complement of g otherwise
 * @param complement_side true if the hole was found in the complement of g
 * @return A 2K2, C4 or C5 obstruction whose pattern is induced in g itself
 *
 * Split graphs are exactly the {2K2, C4, C5}-free graphs, so a raw hole of
 * length >= 6 is not a valid witness on its own and has to be projected onto
 * one of the three. Four consecutive vertices of a long hole induce a 2K2 in
 * the graph the hole lives in; on the complement side that same quadruple
 * induces a C4 in g. The result always describes a pattern of g, so
 * in_complement stays false.
 */
inline Obstruction split_obstruction_from_hole(const std::vector<int>& hole,
                                               bool complement_side) {
    Obstruction o;
    size_t k = hole.size();
    if (k < 4) return o;

    if (!complement_side) {
        if (k == 4) {
            o.kind = ObstructionKind::C4;
            o.vertices = hole;
        } else if (k == 5) {
            o.kind = ObstructionKind::C5;
            o.vertices = hole;
        } else {
            // h1h2 and h4h5 are edges; the four cross pairs are at distance >= 2.
            o.kind = ObstructionKind::TWO_K2;
            o.vertices.push_back(hole[0]);
            o.vertices.push_back(hole[1]);
            o.vertices.push_back(hole[3]);
            o.vertices.push_back(hole[4]);
        }
        return o;
    }

    if (k == 4) {
        // Complement of C4 is 2K2: the diagonals of the hole are the edges of g.
        o.kind = ObstructionKind::TWO_K2;
        o.vertices.push_back(hole[0]);
        o.vertices.push_back(hole[2]);
        o.vertices.push_back(hole[1]);
        o.vertices.push_back(hole[3]);
    } else if (k == 5) {
        // Complement of C5 is C5, traversed by skipping one vertex.
        o.kind = ObstructionKind::C5;
        o.vertices.push_back(hole[0]);
        o.vertices.push_back(hole[2]);
        o.vertices.push_back(hole[4]);
        o.vertices.push_back(hole[1]);
        o.vertices.push_back(hole[3]);
    } else {
        // {h1,h2,h4,h5} induces 2K2 in the complement, hence C4 in g.
        o.kind = ObstructionKind::C4;
        o.vertices.push_back(hole[0]);
        o.vertices.push_back(hole[3]);
        o.vertices.push_back(hole[1]);
        o.vertices.push_back(hole[4]);
    }
    return o;
}

/**
 * @brief Maps a hole to a trivially perfect obstruction
 * @param hole Chordless cycle of g
 * @return A C4 obstruction for a 4-hole, otherwise a P4 on four consecutive
 *         hole vertices
 *
 * Trivially perfect graphs are the {C4, P4}-free graphs; in a chordless cycle
 * of length >= 5 any four consecutive vertices are pairwise at distance >= 2
 * except along the path, so they induce a P4.
 */
inline Obstruction tp_obstruction_from_hole(const std::vector<int>& hole) {
    Obstruction o;
    if (hole.size() < 4) return o;
    if (hole.size() == 4) {
        o.kind = ObstructionKind::C4;
        o.vertices = hole;
        return o;
    }
    o.kind = ObstructionKind::P4;
    for (size_t i = 0; i < 4; ++i) o.vertices.push_back(hole[i]);
    return o;
}

} // namespace detail_obstruction
} // namespace graph_recognition

#endif
