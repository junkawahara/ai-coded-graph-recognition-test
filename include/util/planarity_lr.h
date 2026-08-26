#ifndef GRAPH_RECOGNITION_PLANARITY_LR_H
#define GRAPH_RECOGNITION_PLANARITY_LR_H

/**
 * @file planarity_lr.h
 * @brief Linear-time planarity test (left-right criterion)
 *
 * Implements the left-right planarity criterion of de Fraysseix, Ossona de
 * Mendez and Rosenstiehl, following the formulation of
 *   U. Brandes, "The left-right planarity test", 2009.
 *
 * The test runs in O(n + m) time and only decides planarity; it does not build
 * an embedding (see planar_embedding.h for that).
 *
 * Two depth-first passes over the graph are used:
 *   1. Orientation: root the graph, compute heights, lowpoints and the nesting
 *      depth of every oriented edge.
 *   2. Testing: process edges in order of nesting depth while maintaining a
 *      stack of conflict pairs; a conflict that cannot be resolved by placing
 *      return edges on the left or the right side proves non-planarity.
 *
 * Both passes are iterative, so the recursion depth of the input graph does not
 * limit the size of graphs that can be handled.
 */

#include "util/graph.h"

#include <algorithm>
#include <climits>
#include <utility>
#include <vector>

namespace graph_recognition {
namespace detail_planar_lr {

/**
 * @brief Interval of return edges on one side of a conflict pair
 *
 * An edge identifier of -1 denotes "none"; an interval is empty when both ends
 * are -1.
 */
struct Interval {
    int low;  /**< Return edge with the lowest lowpoint, or -1 */
    int high; /**< Return edge with the highest lowpoint, or -1 */

    Interval() : low(-1), high(-1) {}
    Interval(int l, int h) : low(l), high(h) {}

    bool empty() const { return low == -1 && high == -1; }
};

/** @brief Pair of intervals that must be placed on opposite sides */
struct ConflictPair {
    Interval left;
    Interval right;

    ConflictPair() {}
    ConflictPair(const Interval& l, const Interval& r) : left(l), right(r) {}

    void swap_sides() {
        Interval tmp = left;
        left = right;
        right = tmp;
    }
};

/**
 * @brief Left-right planarity test
 *
 * Each undirected edge is oriented exactly once during the first pass, so the
 * index of an undirected edge doubles as the identifier of the oriented edge.
 */
class LRPlanarity {
public:
    explicit LRPlanarity(const Graph& g) : n_(g.n) {
        build(g);
    }

    /** @brief Returns true if the graph is planar */
    bool run() {
        for (int v = 1; v <= n_; ++v) {
            if (height_[v] == -1) {
                height_[v] = 0;
                roots_.push_back(v);
                dfs_orientation(v);
            }
        }

        order_adjacencies();

        for (size_t i = 0; i < roots_.size(); ++i) {
            S_.clear();
            if (!dfs_testing(roots_[i])) return false;
        }
        return true;
    }

private:
    int n_;
    int m_;

    /* Undirected incidence structure: adj_[v] = (neighbour, edge id) */
    std::vector<std::vector<std::pair<int, int> > > adj_;
    std::vector<unsigned char> oriented_;

    /* Per oriented edge */
    std::vector<int> src_;
    std::vector<int> dst_;
    std::vector<int> lowpt_;
    std::vector<int> lowpt2_;
    std::vector<int> nesting_depth_;
    std::vector<int> ref_;
    std::vector<int> side_;
    std::vector<int> lowpt_edge_;
    std::vector<int> stack_bottom_;
    std::vector<unsigned char> skip_init1_;
    std::vector<unsigned char> skip_init2_;

    /* Per vertex */
    std::vector<int> height_;
    std::vector<int> parent_edge_;
    std::vector<std::vector<int> > out_;
    std::vector<int> ind1_;
    std::vector<int> ind2_;
    std::vector<int> roots_;

    std::vector<ConflictPair> S_;

    void build(const Graph& g) {
        adj_.assign(n_ + 1, std::vector<std::pair<int, int> >());
        m_ = 0;
        for (int u = 1; u <= n_; ++u) {
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int v = g.adj[u][i];
                if (v <= u) continue;
                int id = m_++;
                adj_[u].push_back(std::make_pair(v, id));
                adj_[v].push_back(std::make_pair(u, id));
            }
        }

        oriented_.assign(m_, 0);
        src_.assign(m_, -1);
        dst_.assign(m_, -1);
        lowpt_.assign(m_, 0);
        lowpt2_.assign(m_, 0);
        nesting_depth_.assign(m_, 0);
        ref_.assign(m_, -1);
        side_.assign(m_, 1);
        lowpt_edge_.assign(m_, -1);
        stack_bottom_.assign(m_, 0);
        skip_init1_.assign(m_, 0);
        skip_init2_.assign(m_, 0);

        height_.assign(n_ + 1, -1);
        parent_edge_.assign(n_ + 1, -1);
        out_.assign(n_ + 1, std::vector<int>());
        ind1_.assign(n_ + 1, 0);
        ind2_.assign(n_ + 1, 0);
    }

    /** @brief First pass: orient edges and compute lowpoints and nesting depths */
    void dfs_orientation(int root) {
        std::vector<int> stack;
        stack.push_back(root);

        while (!stack.empty()) {
            int v = stack.back();
            stack.pop_back();
            int e = parent_edge_[v];
            bool descended = false;

            while (ind1_[v] < (int)adj_[v].size()) {
                int w = adj_[v][ind1_[v]].first;
                int id = adj_[v][ind1_[v]].second;

                if (!skip_init1_[id]) {
                    if (oriented_[id]) {
                        ++ind1_[v];
                        continue;
                    }
                    oriented_[id] = 1;
                    src_[id] = v;
                    dst_[id] = w;
                    out_[v].push_back(id);
                    lowpt_[id] = height_[v];
                    lowpt2_[id] = height_[v];

                    if (height_[w] == -1) { /* tree edge */
                        parent_edge_[w] = id;
                        height_[w] = height_[v] + 1;
                        skip_init1_[id] = 1;
                        stack.push_back(v);
                        stack.push_back(w);
                        descended = true;
                        break;
                    }
                    /* back edge */
                    lowpt_[id] = height_[w];
                }

                nesting_depth_[id] = 2 * lowpt_[id];
                if (lowpt2_[id] < height_[v]) ++nesting_depth_[id]; /* chordal */

                if (e != -1) {
                    if (lowpt_[id] < lowpt_[e]) {
                        lowpt2_[e] = std::min(lowpt_[e], lowpt2_[id]);
                        lowpt_[e] = lowpt_[id];
                    } else if (lowpt_[id] > lowpt_[e]) {
                        lowpt2_[e] = std::min(lowpt2_[e], lowpt_[id]);
                    } else {
                        lowpt2_[e] = std::min(lowpt2_[e], lowpt2_[id]);
                    }
                }
                ++ind1_[v];
            }
            (void)descended;
        }
    }

    void order_adjacencies() {
        for (int v = 1; v <= n_; ++v) {
            std::vector<int>& lst = out_[v];
            const std::vector<int>& depth = nesting_depth_;
            std::stable_sort(lst.begin(), lst.end(),
                [&depth](int a, int b) { return depth[a] < depth[b]; });
        }
    }

    int lowest(const ConflictPair& P) const {
        if (P.left.empty() && P.right.empty()) return INT_MAX;
        if (P.left.empty()) return lowpt_[P.right.low];
        if (P.right.empty()) return lowpt_[P.left.low];
        return std::min(lowpt_[P.left.low], lowpt_[P.right.low]);
    }

    bool conflicting(const Interval& I, int b) const {
        return !I.empty() && lowpt_[I.high] > lowpt_[b];
    }

    /** @brief Second pass: test whether all return edges can be placed consistently */
    bool dfs_testing(int root) {
        std::vector<int> stack;
        stack.push_back(root);

        while (!stack.empty()) {
            int v = stack.back();
            stack.pop_back();
            int e = parent_edge_[v];
            bool skip_final = false;

            while (ind2_[v] < (int)out_[v].size()) {
                int ei = out_[v][ind2_[v]];
                int w = dst_[ei];

                if (!skip_init2_[ei]) {
                    stack_bottom_[ei] = (int)S_.size();
                    if (parent_edge_[w] == ei) { /* tree edge */
                        skip_init2_[ei] = 1;
                        stack.push_back(v);
                        stack.push_back(w);
                        skip_final = true;
                        break;
                    }
                    /* back edge */
                    lowpt_edge_[ei] = ei;
                    S_.push_back(ConflictPair(Interval(), Interval(ei, ei)));
                }

                if (lowpt_[ei] < height_[v]) { /* ei has a return edge */
                    if (ind2_[v] == 0) {
                        if (e != -1) lowpt_edge_[e] = lowpt_edge_[ei];
                    } else {
                        if (!add_constraints(ei, e)) return false;
                    }
                }
                ++ind2_[v];
            }

            if (!skip_final && e != -1) remove_back_edges(e);
        }
        return true;
    }

    bool add_constraints(int ei, int e) {
        if (e == -1) return true; /* unreachable: a root has no return edges */

        ConflictPair P;

        /* Merge the return edges of ei into P.right */
        while (true) {
            if (S_.empty()) return false;
            ConflictPair Q = S_.back();
            S_.pop_back();

            if (!Q.left.empty()) Q.swap_sides();
            if (!Q.left.empty()) return false; /* not planar */

            if (Q.right.low != -1 && lowpt_[Q.right.low] > lowpt_[e]) {
                if (P.right.empty()) {
                    P.right.high = Q.right.high;
                } else {
                    ref_[P.right.low] = Q.right.high;
                }
                P.right.low = Q.right.low;
            } else if (Q.right.low != -1) {
                ref_[Q.right.low] = lowpt_edge_[e]; /* align */
            }

            if ((int)S_.size() == stack_bottom_[ei]) break;
        }

        /* Merge the conflicting return edges of e_1..e_{i-1} into P.left */
        while (!S_.empty() &&
               (conflicting(S_.back().left, ei) || conflicting(S_.back().right, ei))) {
            ConflictPair Q = S_.back();
            S_.pop_back();

            if (conflicting(Q.right, ei)) Q.swap_sides();
            if (conflicting(Q.right, ei)) return false; /* not planar */

            if (P.right.low != -1) ref_[P.right.low] = Q.right.high;
            if (Q.right.low != -1) P.right.low = Q.right.low;

            if (P.left.empty()) {
                P.left.high = Q.left.high;
            } else {
                ref_[P.left.low] = Q.left.high;
            }
            P.left.low = Q.left.low;
        }

        if (!(P.left.empty() && P.right.empty())) S_.push_back(P);
        return true;
    }

    void remove_back_edges(int e) {
        int u = src_[e];

        /* Drop conflict pairs whose return edges all end at u */
        while (!S_.empty() && lowest(S_.back()) == height_[u]) {
            ConflictPair P = S_.back();
            S_.pop_back();
            if (P.left.low != -1) side_[P.left.low] = -1;
        }

        if (!S_.empty()) {
            ConflictPair P = S_.back();
            S_.pop_back();

            while (P.left.high != -1 && dst_[P.left.high] == u) {
                P.left.high = ref_[P.left.high];
            }
            if (P.left.high == -1 && P.left.low != -1) {
                ref_[P.left.low] = P.right.low;
                side_[P.left.low] = -1;
                P.left.low = -1;
            }

            while (P.right.high != -1 && dst_[P.right.high] == u) {
                P.right.high = ref_[P.right.high];
            }
            if (P.right.high == -1 && P.right.low != -1) {
                ref_[P.right.low] = P.left.low;
                side_[P.right.low] = -1;
                P.right.low = -1;
            }

            S_.push_back(P);
        }

        /* The side of e is the side of its highest return edge */
        if (lowpt_[e] < height_[u] && !S_.empty()) {
            int hl = S_.back().left.high;
            int hr = S_.back().right.high;
            if (hl != -1 && (hr == -1 || lowpt_[hl] > lowpt_[hr])) {
                ref_[e] = hl;
            } else {
                ref_[e] = hr;
            }
        }
    }
};

/**
 * @brief Determines planarity in O(n + m) time via the left-right criterion
 * @param g Input graph (simple, 1-indexed; may be disconnected)
 * @return true if g is planar
 */
inline bool is_planar_lr(const Graph& g) {
    int n = g.n;
    if (n <= 4) return true; /* every graph on at most 4 vertices is planar */

    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    /* Euler bound for simple planar graphs */
    if (m > 3LL * n - 6) return false;

    LRPlanarity lr(g);
    return lr.run();
}

} // namespace detail_planar_lr
} // namespace graph_recognition

#endif
