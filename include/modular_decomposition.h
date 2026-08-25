#ifndef GRAPH_RECOGNITION_MODULAR_DECOMPOSITION_H
#define GRAPH_RECOGNITION_MODULAR_DECOMPOSITION_H

/**
 * @file modular_decomposition.h
 * @brief Modular decomposition tree
 *
 * A *module* is a vertex set whose members are indistinguishable from the
 * outside: every vertex not in the set is adjacent to all of it or to none of
 * it. The modular decomposition tree records the strong modules -- those that
 * overlap no other module -- as a tree whose leaves are the vertices.
 *
 * The construction follows Gallai's recursion. On a vertex set V:
 *   - one vertex: a leaf;
 *   - G[V] disconnected: a PARALLEL node whose children are the components;
 *   - the complement disconnected: a SERIES node whose children are the
 *     co-components;
 *   - both connected: a PRIME node whose children are the maximal proper
 *     modules, which by Gallai's theorem are pairwise disjoint and cover V.
 *
 * The only nontrivial primitive is the smallest module containing a given
 * pair. It is computed by closure: start from the pair and repeatedly absorb
 * any *splitter*, a vertex adjacent to some but not all of the current set.
 * Every module containing the pair must contain each absorbed vertex, so the
 * fixpoint is the smallest one. Two vertices then lie in the same maximal
 * proper module exactly when their closure is not all of V, and since the
 * maximal proper modules of a prime graph are disjoint, union-find over that
 * relation recovers them.
 *
 * Complexity: O(n^4) in the worst case -- O(n^2) closures per prime node, each
 * O(n^2). The linear-time algorithms (Tedder et al.) are not implemented: the
 * bookkeeping that makes them fast is exactly where they go wrong, and this
 * library checks structures rather than trusting them.
 *
 * References:
 *   - T. Gallai, "Transitiv orientierbare Graphen", Acta Math. Acad. Sci.
 *     Hungar. 18 (1967) 25-66.
 *   - M. Habib, C. Paul, "A survey of the algorithmic aspects of modular
 *     decomposition", Computer Science Review 4 (2010) 41-59.
 */

#include "components.h"
#include "dsu.h"
#include "graph.h"
#include "md_tree.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

namespace detail_modular {

/**
 * @brief Scratch state for the module closure, reused across calls
 *
 * The closure runs once per pair of vertices at a prime node, so the arrays
 * are allocated once and cleared only over the vertices actually touched.
 */
struct ClosureState {
    std::vector<char> in_verts;
    std::vector<char> in_module;
    std::vector<int> count;

    explicit ClosureState(int n) : in_verts(n + 1, 0), in_module(n + 1, 0), count(n + 1, 0) {}
};

/**
 * @brief Smallest module of the subgraph induced on `verts` containing a and b
 * @return The module, in no particular order
 *
 * A vertex outside the current set that is adjacent to some but not all of it
 * cannot be left out of any module containing the set, so absorbing every such
 * splitter until none is left yields the smallest module.
 */
inline std::vector<int> min_module(const Graph& g, const std::vector<int>& verts,
                                   ClosureState& st, int a, int b) {
    std::vector<int> module;
    module.reserve(verts.size());

    // Adding a vertex updates the neighbour counts of everything still outside.
    for (size_t i = 0; i < 2; ++i) {
        int v = i == 0 ? a : b;
        if (st.in_module[v]) continue;
        st.in_module[v] = 1;
        module.push_back(v);
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int u = g.adj[v][j];
            if (st.in_verts[u]) st.count[u]++;
        }
    }

    bool changed = true;
    while (changed && module.size() < verts.size()) {
        changed = false;
        for (size_t i = 0; i < verts.size(); ++i) {
            int x = verts[i];
            if (st.in_module[x]) continue;
            int c = st.count[x];
            if (c == 0 || c == (int)module.size()) continue;
            st.in_module[x] = 1;
            module.push_back(x);
            for (size_t j = 0; j < g.adj[x].size(); ++j) {
                int u = g.adj[x][j];
                if (st.in_verts[u]) st.count[u]++;
            }
            changed = true;
        }
    }

    for (size_t i = 0; i < module.size(); ++i) st.in_module[module[i]] = 0;
    for (size_t i = 0; i < verts.size(); ++i) st.count[verts[i]] = 0;
    return module;
}

/**
 * @brief Maximal proper modules of a connected, co-connected induced subgraph
 *
 * Gallai's theorem makes these pairwise disjoint, so "some proper module holds
 * both" is an equivalence relation and union-find recovers the classes. When a
 * pair's closure is proper, every vertex of it joins the same class at once,
 * which is what keeps the number of closures down.
 */
inline std::vector<std::vector<int>> maximal_proper_modules(const Graph& g,
                                                            const std::vector<int>& verts,
                                                            ClosureState& st) {
    DSU dsu(g.n);
    for (size_t i = 0; i < verts.size(); ++i) {
        for (size_t j = i + 1; j < verts.size(); ++j) {
            int x = verts[i], y = verts[j];
            if (dsu.find(x) == dsu.find(y)) continue;
            std::vector<int> module = min_module(g, verts, st, x, y);
            if (module.size() >= verts.size()) continue;
            for (size_t t = 1; t < module.size(); ++t) dsu.unite(module[0], module[t]);
        }
    }

    std::vector<std::vector<int>> classes;
    std::vector<int> index(g.n + 1, -1);
    for (size_t i = 0; i < verts.size(); ++i) {
        int root = dsu.find(verts[i]);
        if (index[root] == -1) {
            index[root] = (int)classes.size();
            classes.push_back(std::vector<int>());
        }
        classes[index[root]].push_back(verts[i]);
    }
    return classes;
}

} // namespace detail_modular

/**
 * @brief Computes the modular decomposition tree
 * @param g Input graph
 * @return MDTree; empty (root == -1) for the empty graph
 *
 * The tree is in canonical form: children are ordered by their smallest
 * vertex, and each internal node carries the quotient graph on its children.
 * A cograph's tree has no PRIME node and is exactly its cotree.
 */
inline MDTree modular_decomposition(const Graph& g) {
    MDTree tree;
    if (g.n == 0) return tree;

    detail_modular::ClosureState st(g.n);

    struct Task {
        std::vector<int> verts;
        int parent;
    };
    std::vector<Task> pending;
    pending.push_back(Task());
    pending.back().parent = -1;
    pending.back().verts.reserve(g.n);
    for (int v = 1; v <= g.n; ++v) pending.back().verts.push_back(v);

    // An explicit worklist rather than recursion: the tree can be a path of
    // depth O(n), which would overflow the call stack for large n.
    while (!pending.empty()) {
        std::vector<int> verts;
        verts.swap(pending.back().verts);
        int parent = pending.back().parent;
        pending.pop_back();
        if (verts.empty()) continue;

        MDNode node;
        node.parent = parent;
        int index = (int)tree.nodes.size();

        if (verts.size() == 1) {
            node.kind = MDNodeKind::LEAF;
            node.vertex = verts[0];
            tree.nodes.push_back(node);
            if (parent >= 0) tree.nodes[parent].children.push_back(index);
            else tree.root = index;
            continue;
        }

        std::vector<std::vector<int>> children = induced_components(g, verts);
        MDNodeKind kind = MDNodeKind::PARALLEL;
        if (children.size() < 2) {
            children = induced_co_components(g, verts);
            kind = MDNodeKind::SERIES;
        }
        if (children.size() < 2) {
            for (size_t i = 0; i < verts.size(); ++i) st.in_verts[verts[i]] = 1;
            children = detail_modular::maximal_proper_modules(g, verts, st);
            for (size_t i = 0; i < verts.size(); ++i) st.in_verts[verts[i]] = 0;
            kind = MDNodeKind::PRIME;
        }

        node.kind = kind;
        tree.nodes.push_back(node);
        if (parent >= 0) tree.nodes[parent].children.push_back(index);
        else tree.root = index;

        for (size_t i = 0; i < children.size(); ++i) {
            pending.push_back(Task());
            pending.back().verts.swap(children[i]);
            pending.back().parent = index;
        }
    }

    md_finalize(tree, g);
    return tree;
}

} // namespace graph_recognition

#endif
