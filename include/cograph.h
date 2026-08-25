#ifndef GRAPH_RECOGNITION_COGRAPH_H
#define GRAPH_RECOGNITION_COGRAPH_H

/**
 * @file cograph.h
 * @brief Cograph recognition
 *
 * Recognizes cographs by recursive decomposition into connected components / complement connected components.
 *
 * COTREE: original cotree decomposition algorithm. Scans all unvisited vertices for complement component search.
 * PARTITION_REFINEMENT: fast complement component search using doubly-linked lists.
 *   At each BFS step, temporarily removes adjacent vertices and moves all remaining vertices at once.
 * MODULAR: builds the modular decomposition tree and checks it has no PRIME
 *   node. A cograph's modular decomposition is exactly its cotree, so this is
 *   the same structure arrived at by different machinery -- useful as an
 *   independent cross-check, but slower (O(n^4)).
 *
 * build_cotree() returns the decomposition itself as an MDTree (see md_tree.h):
 * a union step becomes a PARALLEL node, a join step a SERIES node, and no
 * PRIME node ever appears -- which is what makes the tree a cotree. Both
 * algorithms share the worklist that drives the decomposition, so both build
 * the same tree; only the complement-component search differs.
 */

#include "forbidden_subgraph.h"
#include "graph.h"
#include "md_tree.h"
#include "obstruction_extract.h"
#include "modular_decomposition.h"
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for cograph recognition
 */
enum class CographAlgorithm {
    COTREE,               /**< cotree decomposition */
    PARTITION_REFINEMENT, /**< fast cotree decomposition via partition refinement (default) */
    MODULAR               /**< modular decomposition with no PRIME node, O(n^4) */
};

/**
 * @brief Result of cograph recognition
 */
struct CographResult {
    bool is_cograph = false; /**< true if the graph is a cograph */
    Obstruction obstruction; /**< NO certificate: a P4. Valid only when
                                  is_cograph == false; filled by every variant */
};

/**
 * @brief Result of cotree construction
 */
struct CotreeResult {
    bool is_cograph = false; /**< true if the graph is a cograph */
    MDTree cotree;           /**< the cotree; valid only when is_cograph == true */
};

namespace detail {

/**
 * @brief Shared cotree decomposition driver
 *
 * The worklist loop and the ordinary connected-component search are common
 * to both algorithms; derived classes only supply the complement-component
 * search (the part the two algorithms actually differ in).
 */
class CographCheckerBase {
public:
    explicit CographCheckerBase(const Graph& graph)
        : g(graph),
          in_subset(graph.n + 1, 0),
          seen(graph.n + 1, 0),
          subset_token(0),
          seen_token(0) {}

    virtual ~CographCheckerBase() {}

    /**
     * @brief The subproblem the decomposition got stuck on
     *
     * Non-empty only after a failed run(). The set induces a subgraph that is
     * connected and co-connected, which by the cograph theorem forces an
     * induced P4 inside it -- and inside it only, so the witness search stays
     * on the subproblem instead of scanning the whole graph.
     */
    const std::vector<int>& stuck_vertices() const { return stuck; }

    /**
     * @brief Runs the decomposition
     * @param out If non-null, receives the decomposition tree (nodes only;
     *            the caller finishes it with md_finalize)
     * @return true if the graph is a cograph
     */
    bool run(MDTree* out = 0) {
        tree = out;
        if (tree) {
            tree->nodes.clear();
            tree->root = -1;
        }
        std::vector<int> verts;
        verts.reserve(g.n);
        for (int v = 1; v <= g.n; ++v) verts.push_back(v);
        return solve(verts);
    }

protected:
    const Graph& g;

    /** @brief Finds the connected components of the complement of the
     *         induced subgraph on verts */
    virtual void complement_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) = 0;

private:
    std::vector<int> stuck;
    std::vector<long long> in_subset;
    std::vector<long long> seen;
    long long subset_token;
    long long seen_token;
    MDTree* tree;

    struct Task {
        std::vector<int> verts;
        int parent;
    };

    /** @brief Appends a node, links it under its parent, and returns its index
     *         (-1 when no tree is being built) */
    int add_node(MDNodeKind kind, int vertex, int parent) {
        if (!tree) return -1;
        MDNode node;
        node.kind = kind;
        node.vertex = vertex;
        node.parent = parent;
        int idx = (int)tree->nodes.size();
        tree->nodes.push_back(node);
        if (parent >= 0) {
            tree->nodes[parent].children.push_back(idx);
        } else {
            tree->root = idx;
        }
        return idx;
    }

    void push(std::vector<Task>& pending, std::vector<int>& verts, int parent) {
        pending.push_back(Task());
        pending.back().verts.swap(verts);
        pending.back().parent = parent;
    }

    /* Iterative worklist instead of recursion: the decomposition tree can be
       a path of depth O(n) (e.g. threshold graphs), which overflows the call
       stack for large n. Order of subproblems does not matter -- the tree is
       put in canonical order by md_finalize afterwards. */
    bool solve(const std::vector<int>& all_verts) {
        std::vector<Task> pending;
        pending.push_back(Task());
        pending.back().verts = all_verts;
        pending.back().parent = -1;

        while (!pending.empty()) {
            std::vector<int> verts;
            verts.swap(pending.back().verts);
            int parent = pending.back().parent;
            pending.pop_back();
            if (verts.empty()) continue;
            if (verts.size() == 1) {
                add_node(MDNodeKind::LEAF, verts[0], parent);
                continue;
            }

            std::vector<std::vector<int>> comps;
            graph_components(verts, comps);
            if ((int)comps.size() > 1) {
                int node = add_node(MDNodeKind::PARALLEL, 0, parent);
                for (size_t i = 0; i < comps.size(); ++i) {
                    push(pending, comps[i], node);
                }
                continue;
            }

            std::vector<std::vector<int>> cocomps;
            complement_components(verts, cocomps);
            if ((int)cocomps.size() > 1) {
                int node = add_node(MDNodeKind::SERIES, 0, parent);
                for (size_t i = 0; i < cocomps.size(); ++i) {
                    push(pending, cocomps[i], node);
                }
                continue;
            }

            stuck = verts;
            return false;
        }
        return true;
    }

    void graph_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) {
        comps.clear();
        subset_token++;
        seen_token++;
        for (size_t i = 0; i < verts.size(); ++i) {
            in_subset[verts[i]] = subset_token;
        }

        std::queue<int> q;
        for (size_t i = 0; i < verts.size(); ++i) {
            int s = verts[i];
            if (seen[s] == seen_token) continue;
            std::vector<int> comp;
            seen[s] = seen_token;
            q.push(s);
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                comp.push_back(v);
                for (size_t j = 0; j < g.adj[v].size(); ++j) {
                    int u = g.adj[v][j];
                    if (in_subset[u] != subset_token) continue;
                    if (seen[u] == seen_token) continue;
                    seen[u] = seen_token;
                    q.push(u);
                }
            }
            comps.push_back(comp);
        }
    }
};

/** @brief Worklist cograph checker (original algorithm: complement
 *         components by scanning all unvisited vertices) */
class CographChecker : public CographCheckerBase {
public:
    explicit CographChecker(const Graph& graph) : CographCheckerBase(graph) {}

protected:
    void complement_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) override {
        comps.clear();

        std::vector<int> unvisited = verts;
        std::vector<unsigned char> alive(g.n + 1, 0);
        for (size_t i = 0; i < verts.size(); ++i) alive[verts[i]] = 1;

        std::queue<int> q;
        while (!unvisited.empty()) {
            int s = unvisited.back();
            unvisited.pop_back();
            if (!alive[s]) continue;
            alive[s] = 0;

            std::vector<int> comp;
            comp.push_back(s);
            q.push(s);

            while (!q.empty()) {
                int v = q.front();
                q.pop();

                std::vector<int> next_unvisited;
                next_unvisited.reserve(unvisited.size());
                for (size_t i = 0; i < unvisited.size(); ++i) {
                    int u = unvisited[i];
                    if (!alive[u]) continue;
                    if (!g.has_edge(v, u)) {
                        alive[u] = 0;
                        comp.push_back(u);
                        q.push(u);
                    } else {
                        next_unvisited.push_back(u);
                    }
                }
                unvisited.swap(next_unvisited);
            }

            comps.push_back(comp);
        }
    }
};

/** @brief Cograph recognition (original algorithm) */
inline CographResult check_cograph_cotree(const Graph& g) {
    CographResult res;
    CographChecker checker(g);
    res.is_cograph = checker.run();
    if (!res.is_cograph) {
        res.obstruction = make_obstruction(
            ObstructionKind::P4,
            detail_obstruction::find_induced_p4_in(g, checker.stuck_vertices()));
    }
    return res;
}

/** @brief Worklist cograph checker (fast version: complement components by
 *         partition refinement over a doubly-linked list) */
class CographCheckerFast : public CographCheckerBase {
public:
    explicit CographCheckerFast(const Graph& graph)
        : CographCheckerBase(graph),
          ll_nxt(graph.n + 1, 0),
          ll_prv(graph.n + 1, 0),
          in_remaining(graph.n + 1, 0) {}

protected:
    /**
     * @brief Finds connected components of the complement graph efficiently
     *
     * Manages the remaining set using a doubly-linked list.
     * At each BFS step:
     *   1. Temporarily remove adjacent vertices of the dequeued v from remaining
     *   2. Move all vertices remaining (= complement graph neighbors) to the component
     *   3. Restore the temporarily removed vertices to remaining
     */
    void complement_components(
        const std::vector<int>& verts,
        std::vector<std::vector<int>>& comps) override {
        comps.clear();
        int k = (int)verts.size();
        if (k == 0) return;

        // Build doubly-linked list (sentinel = 0)
        int sentinel = 0;
        ll_nxt[sentinel] = verts[0];
        ll_prv[verts[0]] = sentinel;
        for (int i = 0; i < k - 1; ++i) {
            ll_nxt[verts[i]] = verts[i + 1];
            ll_prv[verts[i + 1]] = verts[i];
        }
        ll_nxt[verts[k - 1]] = sentinel;
        ll_prv[sentinel] = verts[k - 1];
        for (int i = 0; i < k; ++i) in_remaining[verts[i]] = 1;

        std::queue<int> q;
        std::vector<int> temp_removed;

        while (ll_nxt[sentinel] != sentinel) {
            int s = ll_nxt[sentinel];
            ll_remove(s);

            std::vector<int> comp;
            comp.push_back(s);
            q.push(s);

            while (!q.empty()) {
                int v = q.front();
                q.pop();

                // Step 1: Temporarily remove v's neighbors in G from remaining
                temp_removed.clear();
                for (size_t j = 0; j < g.adj[v].size(); ++j) {
                    int u = g.adj[v][j];
                    if (in_remaining[u]) {
                        ll_remove(u);
                        temp_removed.push_back(u);
                    }
                }

                // Step 2: Move all vertices remaining to component
                // (These are v's neighbors in the complement graph)
                while (ll_nxt[sentinel] != sentinel) {
                    int u = ll_nxt[sentinel];
                    ll_remove(u);
                    comp.push_back(u);
                    q.push(u);
                }

                // Step 3: Restore temporarily removed neighbors to remaining
                for (size_t j = 0; j < temp_removed.size(); ++j) {
                    ll_insert_front(temp_removed[j]);
                }
            }

            comps.push_back(comp);
        }
    }

private:
    std::vector<int> ll_nxt;
    std::vector<int> ll_prv;
    std::vector<unsigned char> in_remaining;

    /** @brief Removes a vertex from the doubly-linked list */
    void ll_remove(int v) {
        ll_nxt[ll_prv[v]] = ll_nxt[v];
        ll_prv[ll_nxt[v]] = ll_prv[v];
        in_remaining[v] = 0;
    }

    /** @brief Inserts a vertex right after the sentinel in the doubly-linked list */
    void ll_insert_front(int v) {
        ll_nxt[v] = ll_nxt[0];
        ll_prv[v] = 0;
        ll_prv[ll_nxt[0]] = v;
        ll_nxt[0] = v;
        in_remaining[v] = 1;
    }
};

/** @brief Cograph recognition (fast version: partition refinement) */
inline CographResult check_cograph_partition(const Graph& g) {
    CographResult res;
    CographCheckerFast checker(g);
    res.is_cograph = checker.run();
    if (!res.is_cograph) {
        res.obstruction = make_obstruction(
            ObstructionKind::P4,
            detail_obstruction::find_induced_p4_in(g, checker.stuck_vertices()));
    }
    return res;
}

/** @brief Cograph recognition via the modular decomposition tree */
inline CographResult check_cograph_modular(const Graph& g) {
    CographResult res;
    res.is_cograph = md_is_cotree(modular_decomposition(g));
    if (!res.is_cograph) {
        // A PRIME node is what failed; its leaves hide the P4, but this
        // variant is O(n^4) so a whole-graph search costs nothing next to it.
        res.obstruction = make_obstruction(ObstructionKind::P4,
                                           detail_obstruction::find_induced_p4(g));
    }
    return res;
}

} // namespace detail

/**
 * @brief Builds the cotree of a graph
 * @param g Input graph
 * @param algo Algorithm to use (default: PARTITION_REFINEMENT)
 * @return CotreeResult
 *
 * The decomposition both recognition algorithms perform already is the cotree;
 * this entry point keeps it instead of discarding it. On a graph that is not a
 * cograph the decomposition gets stuck at an induced P4 and no tree is
 * returned.
 *
 * check_cograph() does not build the tree, so recognizing a cograph costs the
 * same as before.
 */
inline CotreeResult build_cotree(const Graph& g,
    CographAlgorithm algo = CographAlgorithm::PARTITION_REFINEMENT) {
    CotreeResult res;
    switch (algo) {
        case CographAlgorithm::COTREE: {
            detail::CographChecker checker(g);
            res.is_cograph = checker.run(&res.cotree);
            break;
        }
        case CographAlgorithm::PARTITION_REFINEMENT: {
            detail::CographCheckerFast checker(g);
            res.is_cograph = checker.run(&res.cotree);
            break;
        }
        case CographAlgorithm::MODULAR: {
            // The modular decomposition already is the cotree when it has no
            // PRIME node, and md_finalize has run, so return it as it stands.
            MDTree tree = modular_decomposition(g);
            if (!md_is_cotree(tree)) return res;
            res.cotree = tree;
            res.is_cograph = true;
            return res;
        }
        default:
            return res;
    }
    if (!res.is_cograph) {
        res.cotree = MDTree();
        return res;
    }
    md_finalize(res.cotree, g);
    return res;
}

/**
 * @brief Determines whether a graph is a cograph
 * @param g Input graph
 * @param algo Algorithm selector (COTREE or PARTITION_REFINEMENT)
 * @return CographResult
 *
 * A graph is a cograph if every induced subgraph on 2 or more vertices is disconnected
 * or has a disconnected complement. Determined by recursively decomposing into
 * connected components / complement connected components.
 */
inline CographResult check_cograph(const Graph& g,
    CographAlgorithm algo = CographAlgorithm::PARTITION_REFINEMENT) {
    switch (algo) {
        case CographAlgorithm::COTREE:
            return detail::check_cograph_cotree(g);
        case CographAlgorithm::PARTITION_REFINEMENT:
            return detail::check_cograph_partition(g);
        case CographAlgorithm::MODULAR:
            return detail::check_cograph_modular(g);
        default:
            break;
    }
    return CographResult();
}

} // namespace graph_recognition

#endif
