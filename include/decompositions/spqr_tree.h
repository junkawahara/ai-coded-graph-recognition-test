#ifndef GRAPH_RECOGNITION_SPQR_TREE_H
#define GRAPH_RECOGNITION_SPQR_TREE_H

/**
 * @file spqr_tree.h
 * @brief Tutte's decomposition into 3-connected components (SPQR tree)
 *
 * A biconnected graph decomposes uniquely into *polygons* (cycles),
 * *bonds* (two vertices joined by several parallel edges) and simple
 * 3-connected graphs, glued along pairs of *virtual* edges. In SPQR
 * terminology those are the S, P and R nodes; Q nodes (single edges) are not
 * used here, following the modern convention.
 *
 * The construction repeatedly splits at a *split pair* {x, y}: the edges fall
 * into separation classes -- the pieces that stay connected when x and y are
 * removed, plus one class per x-y edge -- and any way of dealing those classes
 * into two groups of at least two edges each is a split. Each side keeps a
 * virtual x-y edge standing for the other. Splitting a triangle or a K4 is
 * impossible in that sense, which is what stops the recursion.
 *
 * Splitting alone does not give the canonical decomposition: it can cut a long
 * cycle into two shorter ones and a large bond into two smaller ones. A merge
 * pass puts those back -- two polygons or two bonds sharing a virtual pair
 * become one. By Tutte's uniqueness theorem the result then does not depend on
 * which splits were taken, so no particular order has to be chosen.
 *
 * Hopcroft & Tarjan's linear-time algorithm is deliberately not implemented:
 * it is the textbook example of a published algorithm that needed correcting
 * (Gutwenger & Mutzel 2001), and its speed is not what this library is short
 * of. The quadratic split-pair search here is checked exhaustively against
 * brute-force separation pairs instead.
 *
 * References:
 *   - W. T. Tutte, "Connectivity in Graphs", University of Toronto Press, 1966.
 *   - J. E. Hopcroft, R. E. Tarjan, "Dividing a graph into triconnected
 *     components", SIAM J. Comput. 2 (1973) 135-158.
 *   - C. Gutwenger, P. Mutzel, "A linear time implementation of SPQR-trees",
 *     Graph Drawing 2000, LNCS 1984, 77-90.
 *   - G. Di Battista, R. Tamassia, "On-line planarity testing", SIAM J.
 *     Comput. 25 (1996) 956-997.
 */

#include "recognizers/biconnected.h"
#include "util/graph.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Label of an SPQR node
 */
enum class SPQRNodeKind {
    S, /**< polygon: a cycle */
    P, /**< bond: two vertices joined by parallel edges */
    R  /**< rigid: a simple 3-connected graph */
};

/**
 * @brief One node of an SPQR tree
 *
 * Skeletons may have parallel edges (a P node is nothing but parallel edges),
 * so they are kept as edge lists rather than as a Graph.
 */
struct SPQRNode {
    SPQRNodeKind kind = SPQRNodeKind::R;   /**< label of this node */
    std::vector<int> vertices;             /**< the skeleton's vertices, ascending */
    std::vector<std::pair<int, int>> edges;/**< skeleton edges, as original vertex pairs */
    std::vector<int> edge_orig;            /**< index into the input edge list, -1 for a virtual edge */
};

/**
 * @brief One edge of an SPQR tree, pairing two virtual edges
 */
struct SPQRTreeEdge {
    int node_u = 0; /**< node on one end */
    int edge_u = 0; /**< index into that node's edges of its virtual edge */
    int node_v = 0; /**< node on the other end */
    int edge_v = 0; /**< index into that node's edges of its virtual edge */
};

/**
 * @brief An SPQR tree
 */
struct SPQRTreeResult {
    bool success = false;                  /**< false if the input was not biconnected */
    std::vector<SPQRNode> nodes;           /**< the S, P and R nodes */
    std::vector<SPQRTreeEdge> tree_edges;  /**< the virtual edge pairs joining them */
};

namespace detail_spqr {

/** @brief An edge of the working multigraph */
struct TriEdge {
    int u = 0;
    int v = 0;
    int twin = -1; /**< the paired virtual edge, -1 for a real edge */
    int orig = -1; /**< index into the input edge list, -1 for a virtual edge */
};

/**
 * @brief Separation classes of a component with respect to a vertex pair
 * @return class index per edge of `comp`, or an empty vector if fewer than two
 *
 * Deleting x and y leaves the rest of the component in pieces; every edge with
 * an endpoint outside {x, y} belongs to that endpoint's piece, and every x-y
 * edge is a class of its own.
 */
inline std::vector<int> separation_classes(const std::vector<TriEdge>& edges,
                                           const std::vector<int>& comp, int x, int y,
                                           int* num_classes) {
    // Union the vertices outside {x, y} that share an edge.
    std::vector<int> parent;
    std::vector<int> index;
    int max_vertex = 0;
    for (size_t i = 0; i < comp.size(); ++i) {
        max_vertex = std::max(max_vertex, std::max(edges[comp[i]].u, edges[comp[i]].v));
    }
    index.assign(max_vertex + 1, -1);
    for (size_t i = 0; i < comp.size(); ++i) {
        int ends[2] = {edges[comp[i]].u, edges[comp[i]].v};
        for (int k = 0; k < 2; ++k) {
            int w = ends[k];
            if (w == x || w == y) continue;
            if (index[w] == -1) {
                index[w] = (int)parent.size();
                parent.push_back((int)parent.size());
            }
        }
    }
    // Tiny union-find over the outside vertices.
    std::vector<int>* p = &parent;
    struct Find {
        static int run(std::vector<int>& p, int a) {
            while (p[a] != a) {
                p[a] = p[p[a]];
                a = p[a];
            }
            return a;
        }
    };
    for (size_t i = 0; i < comp.size(); ++i) {
        int a = edges[comp[i]].u, b = edges[comp[i]].v;
        if (a == x || a == y || b == x || b == y) continue;
        int ra = Find::run(*p, index[a]), rb = Find::run(*p, index[b]);
        if (ra != rb) (*p)[ra] = rb;
    }

    std::vector<int> class_of(comp.size(), -1);
    std::vector<int> root_class(parent.size(), -1);
    int next = 0;
    for (size_t i = 0; i < comp.size(); ++i) {
        int a = edges[comp[i]].u, b = edges[comp[i]].v;
        int outside = -1;
        if (a != x && a != y) outside = a;
        else if (b != x && b != y) outside = b;
        if (outside < 0) {
            class_of[i] = next++;  // an x-y edge is its own class
            continue;
        }
        int root = Find::run(*p, index[outside]);
        if (root_class[root] == -1) root_class[root] = next++;
        class_of[i] = root_class[root];
    }
    *num_classes = next;
    return class_of;
}

/**
 * @brief Splits the classes into two groups of at least two edges each
 * @return true and the group of each class, or false if no such split exists
 *
 * With class sizes summing to the component's edge count, this asks for a
 * subset summing to between 2 and count-2. A subset-sum pass over the class
 * sizes settles it; the sizes are tiny, so the table is too.
 */
inline bool choose_split(const std::vector<int>& class_size, int total,
                         std::vector<char>* group) {
    int k = (int)class_size.size();
    if (k < 2 || total < 4) return false;

    // reach[i][s] = the first i classes can make sum s.
    std::vector<std::vector<char>> reach(k + 1, std::vector<char>(total + 1, 0));
    reach[0][0] = 1;
    for (int i = 0; i < k; ++i) {
        for (int s = 0; s <= total; ++s) {
            if (!reach[i][s]) continue;
            reach[i + 1][s] = 1;
            if (s + class_size[i] <= total) reach[i + 1][s + class_size[i]] = 1;
        }
    }

    int want = -1;
    for (int s = 2; s <= total - 2; ++s) {
        if (reach[k][s]) {
            want = s;
            break;
        }
    }
    if (want < 0) return false;

    group->assign(k, 0);
    int s = want;
    for (int i = k; i-- > 0;) {
        if (s >= class_size[i] && reach[i][s - class_size[i]]) {
            (*group)[i] = 1;
            s -= class_size[i];
        }
    }
    return true;
}

/** @brief Classifies a component of the working multigraph */
inline SPQRNodeKind classify(const std::vector<TriEdge>& edges, const std::vector<int>& comp) {
    std::vector<int> verts;
    for (size_t i = 0; i < comp.size(); ++i) {
        verts.push_back(edges[comp[i]].u);
        verts.push_back(edges[comp[i]].v);
    }
    std::sort(verts.begin(), verts.end());
    verts.erase(std::unique(verts.begin(), verts.end()), verts.end());

    if (verts.size() == 2) return SPQRNodeKind::P;
    if (comp.size() == verts.size()) {
        // A cycle: as many edges as vertices, every degree 2.
        std::vector<int> degree(verts.size(), 0);
        for (size_t i = 0; i < comp.size(); ++i) {
            int a = (int)(std::lower_bound(verts.begin(), verts.end(), edges[comp[i]].u) - verts.begin());
            int b = (int)(std::lower_bound(verts.begin(), verts.end(), edges[comp[i]].v) - verts.begin());
            ++degree[a];
            ++degree[b];
        }
        bool cycle = true;
        for (size_t i = 0; i < degree.size() && cycle; ++i) {
            if (degree[i] != 2) cycle = false;
        }
        if (cycle) return SPQRNodeKind::S;
    }
    return SPQRNodeKind::R;
}

} // namespace detail_spqr

/**
 * @brief Computes the SPQR tree of a biconnected graph
 * @param g Input graph; must be biconnected (at least 3 vertices, no cut vertex)
 * @return SPQRTreeResult; success is false for any other input
 */
inline SPQRTreeResult compute_spqr_tree(const Graph& g) {
    SPQRTreeResult res;
    if (!check_biconnected(g).is_biconnected) return res;

    std::vector<detail_spqr::TriEdge> edges;
    std::vector<std::pair<int, int>> input_edges;
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u >= v) continue;
            detail_spqr::TriEdge e;
            e.u = u;
            e.v = v;
            e.orig = (int)input_edges.size();
            input_edges.push_back(std::make_pair(u, v));
            edges.push_back(e);
        }
    }

    std::vector<std::vector<int>> comps;
    {
        std::vector<int> all;
        for (size_t i = 0; i < edges.size(); ++i) all.push_back((int)i);
        comps.push_back(all);
    }

    // Split until nothing can be split any further.
    bool progress = true;
    while (progress) {
        progress = false;
        for (size_t ci = 0; ci < comps.size() && !progress; ++ci) {
            const std::vector<int> comp = comps[ci];
            std::vector<int> verts;
            for (size_t i = 0; i < comp.size(); ++i) {
                verts.push_back(edges[comp[i]].u);
                verts.push_back(edges[comp[i]].v);
            }
            std::sort(verts.begin(), verts.end());
            verts.erase(std::unique(verts.begin(), verts.end()), verts.end());

            for (size_t xi = 0; xi < verts.size() && !progress; ++xi) {
                for (size_t yi = xi + 1; yi < verts.size() && !progress; ++yi) {
                    int x = verts[xi], y = verts[yi];
                    int num_classes = 0;
                    std::vector<int> class_of =
                        detail_spqr::separation_classes(edges, comp, x, y, &num_classes);
                    if (num_classes < 2) continue;

                    std::vector<int> class_size(num_classes, 0);
                    for (size_t i = 0; i < comp.size(); ++i) class_size[class_of[i]]++;
                    std::vector<char> group;
                    if (!detail_spqr::choose_split(class_size, (int)comp.size(), &group)) {
                        continue;
                    }

                    std::vector<int> part[2];
                    for (size_t i = 0; i < comp.size(); ++i) {
                        part[group[class_of[i]] ? 1 : 0].push_back(comp[i]);
                    }

                    // Each side keeps a virtual x-y edge standing for the other.
                    int va = (int)edges.size();
                    detail_spqr::TriEdge ea;
                    ea.u = x;
                    ea.v = y;
                    edges.push_back(ea);
                    int vb = (int)edges.size();
                    detail_spqr::TriEdge eb;
                    eb.u = x;
                    eb.v = y;
                    edges.push_back(eb);
                    edges[va].twin = vb;
                    edges[vb].twin = va;
                    part[0].push_back(va);
                    part[1].push_back(vb);

                    comps[ci] = part[0];
                    comps.push_back(part[1]);
                    progress = true;
                }
            }
        }
    }

    // Merge back the polygon and bond pairs a split had separated.
    std::vector<char> alive(comps.size(), 1);
    progress = true;
    while (progress) {
        progress = false;
        for (size_t ci = 0; ci < comps.size() && !progress; ++ci) {
            if (!alive[ci]) continue;
            SPQRNodeKind kc = detail_spqr::classify(edges, comps[ci]);
            if (kc == SPQRNodeKind::R) continue;

            for (size_t i = 0; i < comps[ci].size() && !progress; ++i) {
                int e = comps[ci][i];
                if (edges[e].twin < 0) continue;
                int twin = edges[e].twin;

                size_t cj = comps.size();
                size_t jpos = 0;
                for (size_t t = 0; t < comps.size() && cj == comps.size(); ++t) {
                    if (!alive[t] || t == ci) continue;
                    for (size_t s = 0; s < comps[t].size(); ++s) {
                        if (comps[t][s] == twin) {
                            cj = t;
                            jpos = s;
                            break;
                        }
                    }
                }
                if (cj == comps.size()) continue;
                if (detail_spqr::classify(edges, comps[cj]) != kc) continue;

                std::vector<int> merged;
                for (size_t s = 0; s < comps[ci].size(); ++s) {
                    if (s != i) merged.push_back(comps[ci][s]);
                }
                for (size_t s = 0; s < comps[cj].size(); ++s) {
                    if (s != jpos) merged.push_back(comps[cj][s]);
                }
                comps[ci] = merged;
                alive[cj] = 0;
                edges[e].twin = -1;
                edges[twin].twin = -1;
                progress = true;
            }
        }
    }

    // Emit the nodes, then pair up the virtual edges into tree edges.
    std::vector<int> node_of(comps.size(), -1);
    std::vector<std::pair<int, int>> place(edges.size(), std::make_pair(-1, -1));
    for (size_t ci = 0; ci < comps.size(); ++ci) {
        if (!alive[ci]) continue;
        node_of[ci] = (int)res.nodes.size();
        SPQRNode node;
        node.kind = detail_spqr::classify(edges, comps[ci]);
        for (size_t i = 0; i < comps[ci].size(); ++i) {
            int e = comps[ci][i];
            place[e] = std::make_pair(node_of[ci], (int)i);
            node.edges.push_back(std::make_pair(edges[e].u, edges[e].v));
            node.edge_orig.push_back(edges[e].orig);
            node.vertices.push_back(edges[e].u);
            node.vertices.push_back(edges[e].v);
        }
        std::sort(node.vertices.begin(), node.vertices.end());
        node.vertices.erase(std::unique(node.vertices.begin(), node.vertices.end()),
                            node.vertices.end());
        res.nodes.push_back(node);
    }
    for (size_t e = 0; e < edges.size(); ++e) {
        int twin = edges[e].twin;
        if (twin < 0 || twin < (int)e) continue;
        if (place[e].first < 0 || place[twin].first < 0) continue;
        SPQRTreeEdge te;
        te.node_u = place[e].first;
        te.edge_u = place[e].second;
        te.node_v = place[twin].first;
        te.edge_v = place[twin].second;
        res.tree_edges.push_back(te);
    }

    res.success = true;
    return res;
}

} // namespace graph_recognition

#endif
