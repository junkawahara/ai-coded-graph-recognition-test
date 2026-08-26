#ifndef GRAPH_RECOGNITION_SPLIT_DECOMPOSITION_H
#define GRAPH_RECOGNITION_SPLIT_DECOMPOSITION_H

/**
 * @file split_decomposition.h
 * @brief Splits and Cunningham's split decomposition
 *
 * A *split* of a connected graph is a bipartition (A, B) with at least two
 * vertices on each side such that the edges crossing between them are exactly
 * all pairs of A1 x B1, where A1 is the part of A seeing B and B1 the part of
 * B seeing A. Splitting along it replaces the graph by two smaller ones, each
 * carrying a *marker* vertex standing for the other side; recursing until no
 * bag has a split, then merging back the pairs of degenerate bags that a split
 * had needlessly separated, produces Cunningham's canonical decomposition.
 *
 * A graph is distance-hereditary exactly when that decomposition has no prime
 * bag -- every bag is a clique or a star -- which is what
 * DistanceHereditaryAlgorithm::SPLIT_DECOMPOSITION checks.
 *
 * References:
 *   - W. H. Cunningham, "Decomposition of directed graphs", SIAM J. Alg.
 *     Disc. Meth. 3 (1982) 214-228.
 *   - E. Gioan, C. Paul, "Split decomposition and graph-labelled trees",
 *     Discrete Applied Mathematics 160 (2012) 708-733.
 *   - H.-J. Bandelt, H. M. Mulder, "Distance-hereditary graphs", JCTB 41
 *     (1986) 182-208.
 */

#include "decompositions/components.h"
#include "util/graph.h"
#include "util/graph_utils.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Label of a bag in a split decomposition
 */
enum class SplitNodeKind {
    CLIQUE, /**< the skeleton is a complete graph (degenerate) */
    STAR,   /**< the skeleton is a star (degenerate) */
    PRIME   /**< the skeleton has no split */
};

/**
 * @brief One bag of a split decomposition
 */
struct SplitBag {
    SplitNodeKind kind = SplitNodeKind::PRIME; /**< label of this bag */
    Graph skeleton;                            /**< the bag's graph, vertices 1..k */
    std::vector<int> label;                    /**< label[i] = original vertex, 0 for a marker (size k+1) */
    int center = 0;                            /**< STAR: local index of the centre; 0 otherwise */
};

/**
 * @brief One edge of a split decomposition tree, joining two marker vertices
 */
struct SplitTreeEdge {
    int bag_u = 0;    /**< bag on one end */
    int marker_u = 0; /**< its marker's local index */
    int bag_v = 0;    /**< bag on the other end */
    int marker_v = 0; /**< its marker's local index */
};

/**
 * @brief A split decomposition
 */
struct SplitDecompositionResult {
    bool success = false;                    /**< false if the input was not connected */
    std::vector<SplitBag> bags;              /**< the bags; valid only when success */
    std::vector<SplitTreeEdge> tree_edges;   /**< marker pairs joining the bags */
    bool totally_decomposable = false;       /**< no PRIME bag, i.e. distance-hereditary */
};

/**
 * @brief A split of a graph
 */
struct SplitResultPair {
    bool found = false;      /**< true if a split exists */
    std::vector<int> side;   /**< side[v] = 0 or 1 (size n+1); valid only when found */
};

namespace detail_split {

/**
 * @brief Whether a bipartition really is a split
 *
 * Checks the definition directly: with A1 the vertices of side 0 that see
 * side 1 and B1 the vertices of side 1 that see side 0, every pair of
 * A1 x B1 must be an edge. Both sides must hold at least two vertices.
 */
inline bool is_split(const Graph& g, const std::vector<int>& side) {
    int n = g.n;
    std::vector<int> a1, b1;
    int count0 = 0, count1 = 0;
    for (int v = 1; v <= n; ++v) {
        if (side[v] == 0) ++count0;
        else ++count1;
        bool sees_other = false;
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            if (side[g.adj[v][i]] != side[v]) {
                sees_other = true;
                break;
            }
        }
        if (!sees_other) continue;
        if (side[v] == 0) a1.push_back(v);
        else b1.push_back(v);
    }
    if (count0 < 2 || count1 < 2) return false;

    for (size_t i = 0; i < a1.size(); ++i) {
        for (size_t j = 0; j < b1.size(); ++j) {
            if (!g.has_edge(a1[i], b1[j])) return false;
        }
    }
    return true;
}

/**
 * @brief Runs the forcing closure for a seeded split search
 * @param g Input graph
 * @param b The vertex fixed on side 1; a (already in `in_a`) is a neighbour of b
 * @param comp_of comp_of[v] = component index of v in g - N[b], -1 inside N[b]
 * @param in_a Vertices committed to side 0; updated in place
 * @param comp_forced comp_forced[c] = 1 when component c is committed to side 0
 *
 * Three rules, applied until nothing changes. Each only commits a vertex that
 * lies on side 0 in *every* split with a in A1 and b in B1:
 *
 *  - a neighbour y of b adjacent to some but not all of the committed set must
 *    be on side 0: a side-1 vertex seeing side 0 sees all of A1, and one not
 *    seeing side 0 sees none of it;
 *  - a component of g - N[b] holding such a vertex is likewise on side 0,
 *    since a component lies wholly on one side;
 *  - and once a component is on side 0 it sees no side-1 vertex, so its
 *    neighbours inside N(b) are on side 0 too.
 */
inline void force_closure(const Graph& g, int b, const std::vector<int>& comp_of,
                          std::vector<char>& in_a, std::vector<char>& comp_forced) {
    int n = g.n;
    bool changed = true;
    while (changed) {
        changed = false;
        int committed = 0;
        for (int v = 1; v <= n; ++v) {
            if (in_a[v]) ++committed;
        }

        for (int y = 1; y <= n; ++y) {
            if (in_a[y] || y == b) continue;
            int seen = 0;
            for (size_t i = 0; i < g.adj[y].size(); ++i) {
                if (in_a[g.adj[y][i]]) ++seen;
            }
            if (seen == 0 || seen == committed) continue;

            if (g.has_edge(y, b)) {
                in_a[y] = 1;
                changed = true;
            } else if (comp_of[y] >= 0 && !comp_forced[comp_of[y]]) {
                comp_forced[comp_of[y]] = 1;
                changed = true;
            }
        }

        // A component on side 0 has all its neighbours on side 0.
        for (int v = 1; v <= n; ++v) {
            if (comp_of[v] < 0 || !comp_forced[comp_of[v]]) continue;
            if (!in_a[v]) {
                in_a[v] = 1;
                changed = true;
            }
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (in_a[u]) continue;
                in_a[u] = 1;
                changed = true;
            }
        }
    }
}

} // namespace detail_split

/**
 * @brief Finds a split of a connected graph
 * @param g Input graph (must be connected; otherwise no split is reported)
 * @return SplitResultPair; found is false exactly when g is prime or too small
 *
 * Every split has a crossing edge (a, b) and a second vertex a2 on a's side,
 * so seeding the closure with those three and running it for every choice
 * finds one whenever one exists. The closure only ever commits vertices that
 * every such split puts on a's side, so its fixpoint is contained in that
 * side; the committed components end up seeing nothing on the far side, which
 * makes the fixpoint itself a split. Each candidate is checked against the
 * definition regardless.
 *
 * Costs O(n * m) seeds times an O(n^2) closure.
 */
inline SplitResultPair find_split(const Graph& g) {
    SplitResultPair res;
    int n = g.n;
    if (n < 4) return res;
    if (connected_components(g).count != 1) return res;

    std::vector<char> in_a(n + 1, 0);
    std::vector<char> comp_forced;
    std::vector<int> comp_of(n + 1, -1);
    std::vector<int> side(n + 1, 0);

    for (int a = 1; a <= n; ++a) {
        for (size_t bi = 0; bi < g.adj[a].size(); ++bi) {
            int b = g.adj[a][bi];

            // Components of g - N[b]: each lies wholly on one side, because a
            // path leaving a's side would have to cross an edge whose side-0
            // endpoint sees side 1 and therefore lies in N(b).
            std::vector<int> outside;
            for (int v = 1; v <= n; ++v) {
                if (v != b && !g.has_edge(v, b)) outside.push_back(v);
            }
            std::vector<std::vector<int>> comps = induced_components(g, outside);
            std::fill(comp_of.begin(), comp_of.end(), -1);
            for (size_t c = 0; c < comps.size(); ++c) {
                for (size_t j = 0; j < comps[c].size(); ++j) comp_of[comps[c][j]] = (int)c;
            }

            for (int a2 = 1; a2 <= n; ++a2) {
                if (a2 == a || a2 == b) continue;

                std::fill(in_a.begin(), in_a.end(), 0);
                comp_forced.assign(comps.size(), 0);
                in_a[a] = 1;
                if (g.has_edge(a2, b)) {
                    in_a[a2] = 1;
                } else {
                    comp_forced[comp_of[a2]] = 1;
                }

                detail_split::force_closure(g, b, comp_of, in_a, comp_forced);
                if (in_a[b]) continue;

                for (int v = 1; v <= n; ++v) side[v] = in_a[v] ? 0 : 1;
                if (!detail_split::is_split(g, side)) continue;

                res.side = side;
                res.found = true;
                return res;
            }
        }
    }
    return res;
}

namespace detail_split {

/**
 * @brief A bag while the decomposition is still being built
 *
 * Adjacency is a matrix because bags are small and get rebuilt on every split
 * and merge; edge_id links a marker to the tree edge it belongs to, so moving
 * a marker between bags only has to rewrite that edge's endpoint.
 */
struct WorkBag {
    std::vector<int> label;             /**< 1..k; 0 marks a marker vertex */
    std::vector<std::vector<char>> adj; /**< (k+1) x (k+1) */
    std::vector<int> edge_id;           /**< tree edge of a marker, -1 for a real vertex */
    std::vector<int> edge_side;         /**< 0 or 1: which end of that edge */
    bool alive = true;

    int size() const { return (int)label.size() - 1; }
};

/** @brief Classifies a bag and, for a star, reports its centre */
inline SplitNodeKind classify(const WorkBag& w, int* center) {
    int k = w.size();
    *center = 0;
    if (k <= 2) return SplitNodeKind::CLIQUE;

    bool complete = true;
    for (int i = 1; i <= k && complete; ++i) {
        for (int j = i + 1; j <= k && complete; ++j) {
            if (!w.adj[i][j]) complete = false;
        }
    }
    if (complete) return SplitNodeKind::CLIQUE;

    std::vector<int> degree(k + 1, 0);
    for (int i = 1; i <= k; ++i) {
        for (int j = 1; j <= k; ++j) {
            if (w.adj[i][j]) ++degree[i];
        }
    }
    for (int c = 1; c <= k; ++c) {
        if (degree[c] != k - 1) continue;
        bool star = true;
        for (int i = 1; i <= k && star; ++i) {
            if (i != c && degree[i] != 1) star = false;
        }
        if (star) {
            *center = c;
            return SplitNodeKind::STAR;
        }
    }
    return SplitNodeKind::PRIME;
}

/** @brief The bag's skeleton as a Graph, for handing to find_split() */
inline Graph work_bag_graph(const WorkBag& w) {
    int k = w.size();
    std::vector<std::pair<int, int>> edges;
    for (int i = 1; i <= k; ++i) {
        for (int j = i + 1; j <= k; ++j) {
            if (w.adj[i][j]) edges.push_back(std::make_pair(i, j));
        }
    }
    return Graph(k, edges);
}

/**
 * @brief Extracts one side of a split into a new bag with a marker attached
 * @param src The bag being split
 * @param side side[i] for local i in 1..k
 * @param want Which side to extract
 * @param new_index Index the new bag will occupy
 * @param new_edge Tree edge the new marker belongs to
 * @param new_side Which end of that edge the new marker is
 * @param edges The tree edges, whose endpoints are rewritten for moved markers
 * @param marker_local Receives the new marker's local index
 */
inline WorkBag extract_side(const WorkBag& src, const std::vector<int>& side, int want,
                            int new_index, int new_edge, int new_side,
                            std::vector<SplitTreeEdge>& edges, int* marker_local) {
    int k = src.size();
    std::vector<int> local(k + 1, 0);
    int count = 0;
    for (int i = 1; i <= k; ++i) {
        if (side[i] == want) local[i] = ++count;
    }
    int marker = count + 1;
    *marker_local = marker;

    WorkBag out;
    out.label.assign(marker + 1, 0);
    out.edge_id.assign(marker + 1, -1);
    out.edge_side.assign(marker + 1, 0);
    out.adj.assign(marker + 1, std::vector<char>(marker + 1, 0));

    for (int i = 1; i <= k; ++i) {
        if (side[i] != want) continue;
        int x = local[i];
        out.label[x] = src.label[i];
        out.edge_id[x] = src.edge_id[i];
        out.edge_side[x] = src.edge_side[i];
        if (out.edge_id[x] >= 0) {
            SplitTreeEdge& e = edges[out.edge_id[x]];
            if (out.edge_side[x] == 0) {
                e.bag_u = new_index;
                e.marker_u = x;
            } else {
                e.bag_v = new_index;
                e.marker_v = x;
            }
        }
        for (int j = 1; j <= k; ++j) {
            if (side[j] != want || !src.adj[i][j]) continue;
            out.adj[x][local[j]] = 1;
        }
        // The marker stands for the far side, so it takes over this vertex's
        // edges that crossed the split.
        bool crosses = false;
        for (int j = 1; j <= k && !crosses; ++j) {
            if (side[j] != want && src.adj[i][j]) crosses = true;
        }
        if (crosses) {
            out.adj[x][marker] = 1;
            out.adj[marker][x] = 1;
        }
    }

    out.edge_id[marker] = new_edge;
    out.edge_side[marker] = new_side;
    return out;
}

/**
 * @brief Joins two bags across a tree edge, undoing one split
 *
 * The two markers disappear and their neighbourhoods are completely joined,
 * which restores exactly the edges the split had replaced by markers.
 */
inline WorkBag join_bags(const WorkBag& u, int marker_u, const WorkBag& v, int marker_v,
                         int new_index, std::vector<SplitTreeEdge>& edges) {
    int ku = u.size(), kv = v.size();
    std::vector<int> lu(ku + 1, 0), lv(kv + 1, 0);
    int count = 0;
    for (int i = 1; i <= ku; ++i) {
        if (i != marker_u) lu[i] = ++count;
    }
    for (int i = 1; i <= kv; ++i) {
        if (i != marker_v) lv[i] = ++count;
    }

    WorkBag out;
    out.label.assign(count + 1, 0);
    out.edge_id.assign(count + 1, -1);
    out.edge_side.assign(count + 1, 0);
    out.adj.assign(count + 1, std::vector<char>(count + 1, 0));

    const WorkBag* src[2] = {&u, &v};
    const std::vector<int>* maps[2] = {&lu, &lv};
    int skip[2] = {marker_u, marker_v};
    for (int t = 0; t < 2; ++t) {
        int k = src[t]->size();
        for (int i = 1; i <= k; ++i) {
            if (i == skip[t]) continue;
            int x = (*maps[t])[i];
            out.label[x] = src[t]->label[i];
            out.edge_id[x] = src[t]->edge_id[i];
            out.edge_side[x] = src[t]->edge_side[i];
            if (out.edge_id[x] >= 0) {
                SplitTreeEdge& e = edges[out.edge_id[x]];
                if (out.edge_side[x] == 0) {
                    e.bag_u = new_index;
                    e.marker_u = x;
                } else {
                    e.bag_v = new_index;
                    e.marker_v = x;
                }
            }
            for (int j = 1; j <= k; ++j) {
                if (j == skip[t] || !src[t]->adj[i][j]) continue;
                out.adj[x][(*maps[t])[j]] = 1;
            }
        }
    }

    for (int i = 1; i <= ku; ++i) {
        if (i == marker_u || !u.adj[marker_u][i]) continue;
        for (int j = 1; j <= kv; ++j) {
            if (j == marker_v || !v.adj[marker_v][j]) continue;
            out.adj[lu[i]][lv[j]] = 1;
            out.adj[lv[j]][lu[i]] = 1;
        }
    }
    return out;
}

} // namespace detail_split

/**
 * @brief Computes the canonical split decomposition of a connected graph
 * @param g Input graph; must be connected
 * @return SplitDecompositionResult; success is false for a disconnected input
 *
 * Bags are split until none has a split left, then the pairs of degenerate
 * bags that a split had needlessly separated are merged back: two cliques
 * joined by a marker pair are one clique, and a star whose centre is the
 * marker joined to a star whose extremity is the marker is one star. By
 * Cunningham's uniqueness theorem the result does not depend on which splits
 * were taken, so no particular order has to be chosen.
 *
 * Cliques and stars are not split even though they have splits: doing so and
 * merging back returns the same bag, so stopping there just skips the round
 * trip.
 */
inline SplitDecompositionResult split_decomposition(const Graph& g) {
    SplitDecompositionResult res;
    if (g.n == 0) {
        res.success = true;
        res.totally_decomposable = true;
        return res;
    }
    if (connected_components(g).count != 1) return res;

    std::vector<detail_split::WorkBag> bags;
    std::vector<SplitTreeEdge> edges;

    {
        detail_split::WorkBag root;
        root.label.assign(g.n + 1, 0);
        root.edge_id.assign(g.n + 1, -1);
        root.edge_side.assign(g.n + 1, 0);
        root.adj.assign(g.n + 1, std::vector<char>(g.n + 1, 0));
        for (int v = 1; v <= g.n; ++v) {
            root.label[v] = v;
            for (size_t i = 0; i < g.adj[v].size(); ++i) root.adj[v][g.adj[v][i]] = 1;
        }
        bags.push_back(root);
    }

    // Split until every bag is degenerate or prime.
    bool progress = true;
    while (progress) {
        progress = false;
        for (size_t bi = 0; bi < bags.size(); ++bi) {
            if (!bags[bi].alive) continue;
            int center = 0;
            if (detail_split::classify(bags[bi], &center) != SplitNodeKind::PRIME) continue;

            SplitResultPair sp = find_split(detail_split::work_bag_graph(bags[bi]));
            if (!sp.found) continue;

            int edge_index = (int)edges.size();
            edges.push_back(SplitTreeEdge());
            int other_index = (int)bags.size();
            int marker_a = 0, marker_b = 0;
            detail_split::WorkBag a = detail_split::extract_side(
                bags[bi], sp.side, 0, (int)bi, edge_index, 0, edges, &marker_a);
            detail_split::WorkBag b = detail_split::extract_side(
                bags[bi], sp.side, 1, other_index, edge_index, 1, edges, &marker_b);
            edges[edge_index].bag_u = (int)bi;
            edges[edge_index].marker_u = marker_a;
            edges[edge_index].bag_v = other_index;
            edges[edge_index].marker_v = marker_b;

            bags[bi] = a;
            bags.push_back(b);
            progress = true;
            break;
        }
    }

    // Merge the degenerate pairs back.
    progress = true;
    while (progress) {
        progress = false;
        for (size_t ei = 0; ei < edges.size(); ++ei) {
            int u = edges[ei].bag_u, v = edges[ei].bag_v;
            if (u < 0 || v < 0) continue;
            if (!bags[u].alive || !bags[v].alive) continue;

            int cu = 0, cv = 0;
            SplitNodeKind ku = detail_split::classify(bags[u], &cu);
            SplitNodeKind kv = detail_split::classify(bags[v], &cv);
            bool merge = false;
            if (ku == SplitNodeKind::CLIQUE && kv == SplitNodeKind::CLIQUE) {
                merge = true;
            } else if (ku == SplitNodeKind::STAR && kv == SplitNodeKind::STAR) {
                // Exactly one of the two markers must be its star's centre;
                // centre-to-centre or extremity-to-extremity is not a star.
                bool u_center = edges[ei].marker_u == cu;
                bool v_center = edges[ei].marker_v == cv;
                merge = u_center != v_center;
            }
            if (!merge) continue;

            detail_split::WorkBag merged = detail_split::join_bags(
                bags[u], edges[ei].marker_u, bags[v], edges[ei].marker_v, u, edges);
            bags[u] = merged;
            bags[v].alive = false;
            edges[ei].bag_u = -1;
            edges[ei].bag_v = -1;
            progress = true;
            break;
        }
    }

    // Compact away the merged-out bags and edges.
    std::vector<int> remap(bags.size(), -1);
    for (size_t i = 0; i < bags.size(); ++i) {
        if (!bags[i].alive) continue;
        remap[i] = (int)res.bags.size();
        SplitBag bag;
        int center = 0;
        bag.kind = detail_split::classify(bags[i], &center);
        bag.center = bag.kind == SplitNodeKind::STAR ? center : 0;
        bag.label = bags[i].label;
        bag.skeleton = detail_split::work_bag_graph(bags[i]);
        res.bags.push_back(bag);
    }
    for (size_t ei = 0; ei < edges.size(); ++ei) {
        if (edges[ei].bag_u < 0 || edges[ei].bag_v < 0) continue;
        SplitTreeEdge e = edges[ei];
        e.bag_u = remap[e.bag_u];
        e.bag_v = remap[e.bag_v];
        res.tree_edges.push_back(e);
    }

    res.totally_decomposable = true;
    for (size_t i = 0; i < res.bags.size(); ++i) {
        if (res.bags[i].kind == SplitNodeKind::PRIME) res.totally_decomposable = false;
    }
    res.success = true;
    return res;
}

/**
 * @brief Whether every bag of the split decomposition is degenerate
 * @param g Input graph
 * @return true if g is totally decomposable by splits
 *
 * Handles disconnected graphs by taking each component on its own, since a
 * split is only defined for a connected graph.
 */
inline bool is_totally_decomposable(const Graph& g) {
    ComponentsResult cc = connected_components(g);
    for (int c = 0; c < cc.count; ++c) {
        std::vector<int> orig;
        Graph sub = induced_subgraph(g, cc.vertices[c], &orig);
        SplitDecompositionResult sd = split_decomposition(sub);
        if (!sd.success || !sd.totally_decomposable) return false;
    }
    return true;
}

} // namespace graph_recognition

#endif
