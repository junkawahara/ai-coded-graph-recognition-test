#ifndef GRAPH_RECOGNITION_INTERVAL_MPQ_TREE_H
#define GRAPH_RECOGNITION_INTERVAL_MPQ_TREE_H

// Internal MPQ-tree primitives for interval enumeration. References:
// Mikos, DMTCS 23:1 (2021), https://doi.org/10.46298/dmtcs.6164,
// Sections 2--4 and 6. Vertices and section indices here are zero-based.
//
// We build the clique PQ-tree from an endpoint string with the existing
// Booth--Lueker reductions, then move vertices to their lowest owning node.
// This is polynomial, but is NOT Saitoh's linear-time MPQ construction.
// Section masks and bottom-up strings also trade space/time for simplicity;
// the paper's O(n^3 log n) enumeration delay is not claimed.

#include <algorithm>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "decompositions/pq_tree.h"

namespace graph_recognition {
namespace detail {

class IntervalMPQTree {
public:
    typedef unsigned long long Mask;

    struct Node {
        bool q;
        Mask vertices;
        std::vector<int> children;
        std::vector<Mask> sections;
        Mask subtree;
        int parent;
        int position;

        Node(bool is_q = false, Mask owned = 0)
            : q(is_q), vertices(owned), subtree(0), parent(-1), position(-1) {}
    };

    int n;
    int root;
    std::vector<Node> nodes;
    std::vector<int> owner;
    std::vector<int> string;

    // Every vertex occurs twice in endpoints, whose values are in [0,n).
    explicit IntervalMPQTree(const std::vector<int>& endpoints)
        : n(static_cast<int>(endpoints.size() / 2)), root(-1) {
        if (endpoints.size() % 2 || n > 63)
            throw std::invalid_argument("IntervalMPQTree: invalid endpoint string size");
        std::vector<int> occurrences(n, 0);
        std::vector<Mask> cliques;
        Mask active = 0;
        bool previous_open = false;
        for (int v : endpoints) {
            if (v < 0 || v >= n || ++occurrences[v] > 2)
                throw std::invalid_argument("IntervalMPQTree: invalid endpoint string");
            const Mask bit = Mask(1) << v;
            if (occurrences[v] == 1) {
                active |= bit;
                previous_open = true;
            } else {
                // A maximal clique occurs at each transition from opening
                // endpoints to closing endpoints (all endpoints are distinct).
                if (previous_open) cliques.push_back(active);
                active &= ~bit;
                previous_open = false;
            }
        }
        if (active) throw std::invalid_argument("IntervalMPQTree: unclosed interval");
        if (!n) return;

        PQTree pq;
        pq.initialize(static_cast<int>(cliques.size()));
        for (int v = 0; v < n; ++v) {
            std::vector<int> row;
            for (size_t i = 0; i < cliques.size(); ++i)
                if (cliques[i] & (Mask(1) << v)) row.push_back(static_cast<int>(i));
            if (!pq.reduce(row))
                throw std::logic_error("IntervalMPQTree: consecutive clique reduction failed");
        }
        std::map<const PQNode*, Mask> common;
        clique_intersections(pq.root(), cliques, common);
        root = import_node(pq.root(), 0, common);
        canonicalize(root);
        std::vector<int> raw;
        emit(root, raw);
        std::vector<int> labels(n, -1);
        int next = 0;
        for (int v : raw) {
            if (labels[v] < 0) labels[v] = next++;
            string.push_back(labels[v]);
        }
        if (next != n || raw.size() != endpoints.size())
            throw std::logic_error("IntervalMPQTree: lost vertex during construction");
        for (Node& node : nodes) {
            node.vertices = relabel(node.vertices, labels);
            for (Mask& section : node.sections) section = relabel(section, labels);
        }
        owner.assign(n, -1);
        index(root, -1, -1);
    }

    std::vector<Mask> adjacency() const {
        std::vector<Mask> adj(n, 0);
        Mask active = 0, seen = 0;
        for (int v : string) {
            const Mask bit = Mask(1) << v;
            if (seen & bit) {
                active &= ~bit;
            } else {
                adj[v] |= active;
                for (int u = 0; u < n; ++u)
                    if (active & (Mask(1) << u)) adj[u] |= bit;
                active |= bit;
                seen |= bit;
            }
        }
        return adj;
    }

    // Mikos Theorem 32: skip the universal prefix, extend the next interval
    // across the first opening endpoint to its right. Exactly one edge is added.
    std::vector<int> parent_string() const {
        std::vector<int> result = string;
        int j = 0;
        while (j < n && string[j] == j && string[2 * n - 1 - j] == j) ++j;
        if (j == n) return result; // K_n (also the empty graph).
        size_t right = second_position(result, j);
        std::vector<bool> seen(n, false);
        size_t target = result.size();
        for (size_t i = 0; i < result.size(); ++i) {
            if (i > right && !seen[result[i]]) { target = i; break; }
            seen[result[i]] = true;
        }
        if (target == result.size())
            throw std::logic_error("IntervalMPQTree: non-complete tree has no parent edge");
        std::rotate(result.begin() + right, result.begin() + right + 1,
                    result.begin() + target + 1);
        return result;
    }

    // Mikos Lemmas 9, 11--14 and Corollary 18. Returns false exactly when
    // deleting the given edge destroys the interval property. A successful
    // result is an endpoint string for G-xy, without a recognition call.
    bool delete_edge(int x, int y, std::vector<int>& result) const {
        result.clear();
        int a = owner[x], b = owner[y];
        if (a == b) {
            if (!nodes[a].q) {
                if (!nodes[a].children.empty()) return false;
                result = string;
                compact(result, y);
                separate(result, x, y);
                return true;
            }
            const Node& q = nodes[a];
            const auto rx = range(q, x), ry = range(q, y);
            const int l = std::max(rx.first, ry.first);
            const int r = std::min(rx.second, ry.second);
            if (l != r || !clique(q.children[l])) return false;
            // The vertex ending at the common section goes on the left.
            if (rx.second != l) std::swap(x, y);
            result = string;
            move_left_endpoint_right(result, y);
            move_right_endpoint_left(result, x);
            const size_t i = first_position(result, y);
            if (i + 1 != second_position(result, x))
                throw std::logic_error("IntervalMPQTree: nonadjacent Q endpoints");
            std::swap(result[i], result[i + 1]);
            return true;
        }

        // Edges always join vertices in ancestor/descendant owning nodes.
        if (!ancestor(a, b)) { std::swap(a, b); std::swap(x, y); }
        if (!ancestor(a, b) || nodes[b].q || !nodes[b].children.empty()) return false;
        int child = b;
        while (nodes[child].parent != a) {
            const int p = nodes[child].parent;
            if (nodes[p].q && nodes[child].position != 0 &&
                nodes[child].position + 1 != static_cast<int>(nodes[p].children.size()))
                return false; // Not almost rotable (Lemma 17).
            child = p;
        }

        IntervalMPQTree changed(*this);
        if (!nodes[a].q) {
            changed.orient_path(a, b);
            changed.emit(root, result);
            compact(result, y);
            separate(result, x, y);
            return true;
        }

        const Node& q = nodes[a];
        const int section = nodes[child].position;
        const auto rx = range(q, x);
        if (section == rx.first || section == rx.second) {
            if (section == rx.second) changed.reverse(a);
            changed.orient_path(child, b);
            changed.emit(root, result);
            compact(result, y);
            separate(result, x, y);
            return true;
        }

        // Central start: Lemma 13 (isolated y in T_a) or Lemma 14.
        // Test set inclusions directly; unlike Section 5 we do not build
        // endpoint heaps to amortize these tests across all edges.
        const Mask sa = q.sections[section];
        const Mask without_x = sa & ~(Mask(1) << x);
        const bool neighbor = has_neighbor_below(child, b, y);
        int insertion = -1;
        bool from_right = false;
        for (int side = 0; side < 2 && insertion < 0; ++side) {
            const int start = side ? rx.second : 0;
            const int end = side ? static_cast<int>(q.children.size()) - 1 : rx.first;
            for (int s = start; s <= end; ++s) {
                if (neighbor && s != (side ? rx.second : rx.first)) continue;
                if (without_x & ~q.sections[s]) continue;
                const Mask crossing = side
                    ? (s + 1 < static_cast<int>(q.sections.size())
                           ? q.sections[s] & q.sections[s + 1] : 0)
                    : (s ? q.sections[s - 1] & q.sections[s] : 0);
                if (crossing & ~sa) continue;
                insertion = side ? s + 1 : s;
                from_right = side != 0;
                break;
            }
        }
        if (insertion < 0) return false;
        if (!neighbor) {
            changed.nodes[b].vertices &= ~(Mask(1) << y);
            const int leaf = static_cast<int>(changed.nodes.size());
            changed.nodes.push_back(Node(false, Mask(1) << y));
            changed.nodes[a].children.insert(changed.nodes[a].children.begin() + insertion, leaf);
            changed.nodes[a].sections.insert(changed.nodes[a].sections.begin() + insertion, without_x);
            changed.emit(root, result);
        } else {
            int old_section = section;
            if (from_right) {
                changed.reverse(a);
                old_section = static_cast<int>(q.children.size()) - 1 - section;
                insertion = static_cast<int>(q.children.size()) - insertion;
            }
            Node& cq = changed.nodes[a];
            cq.children.erase(cq.children.begin() + old_section);
            cq.sections.erase(cq.sections.begin() + old_section);
            cq.children.insert(cq.children.begin() + insertion, child);
            cq.sections.insert(cq.sections.begin() + insertion, without_x);
            changed.orient_path(child, b);
            changed.emit(root, result);
            compact(result, y);
            const size_t left = first_position(result, x);
            const size_t after_y = second_position(result, y) + 1;
            if (after_y > left)
                throw std::logic_error("IntervalMPQTree: invalid central endpoint move");
            std::rotate(result.begin() + after_y, result.begin() + left,
                        result.begin() + left + 1);
        }
        return true;
    }

private:
    static Mask relabel(Mask mask, const std::vector<int>& labels) {
        Mask result = 0;
        for (size_t v = 0; v < labels.size(); ++v)
            if (mask & (Mask(1) << v)) result |= Mask(1) << labels[v];
        return result;
    }

    static Mask clique_intersections(const PQNode* p, const std::vector<Mask>& cliques,
                                     std::map<const PQNode*, Mask>& common) {
        Mask mask = ~Mask(0);
        if (p->type == PQNodeType::LEAF) mask = cliques[p->leaf_value];
        else for (const PQNode* c : p->children) mask &= clique_intersections(c, cliques, common);
        common[p] = mask;
        return mask;
    }

    int import_node(const PQNode* p, Mask assigned, const std::map<const PQNode*, Mask>& common) {
        const Mask universal = common.at(p) & ~assigned;
        const bool q = p->type == PQNodeType::Q_NODE && p->children.size() > 2;
        const int id = static_cast<int>(nodes.size());
        nodes.push_back(Node(q, q ? 0 : universal));
        if (q) {
            Mask seen = 0, multiple = 0;
            for (const PQNode* c : p->children) {
                multiple |= seen & common.at(c);
                seen |= common.at(c);
            }
            nodes[id].vertices = multiple & ~assigned & ~universal;
        }
        const Mask below = assigned | universal | nodes[id].vertices;
        for (const PQNode* c : p->children) {
            const int child = import_node(c, below, common);
            nodes[id].children.push_back(child);
            if (q) nodes[id].sections.push_back(common.at(c) & nodes[id].vertices);
        }
        if (q && universal) {
            const int wrapper = static_cast<int>(nodes.size());
            nodes.push_back(Node(false, universal));
            nodes[wrapper].children.push_back(id);
            return wrapper;
        }
        if (!q) {
            // Suppress empty grouping P-nodes and merge a sole P child.
            std::vector<int> children;
            for (int c : nodes[id].children) {
                if (!nodes[c].q && !nodes[c].vertices)
                    children.insert(children.end(), nodes[c].children.begin(), nodes[c].children.end());
                else children.push_back(c);
            }
            nodes[id].children.swap(children);
            if (nodes[id].children.size() == 1) {
                const int c = nodes[id].children[0];
                if (!nodes[c].q) {
                    nodes[c].vertices |= nodes[id].vertices;
                    return c;
                }
                if (!nodes[id].vertices) return c;
            }
        }
        return id;
    }

    static std::pair<int, int> range(const Node& q, int v) {
        int l = -1, r = -1;
        for (size_t i = 0; i < q.sections.size(); ++i) {
            if (q.sections[i] & (Mask(1) << v)) {
                if (l < 0) l = static_cast<int>(i);
                r = static_cast<int>(i);
            }
        }
        return std::make_pair(l, r);
    }

    void emit(int id, std::vector<int>& out) const {
        if (id < 0) return;
        const Node& node = nodes[id];
        std::vector<int> vertices;
        for (int v = 0; v < n; ++v) if (node.vertices & (Mask(1) << v)) vertices.push_back(v);
        if (!node.q) {
            out.insert(out.end(), vertices.begin(), vertices.end());
            for (int c : node.children) emit(c, out);
            out.insert(out.end(), vertices.rbegin(), vertices.rend());
        } else {
            std::vector<std::pair<std::pair<int, int>, int> > intervals;
            for (int v : vertices) intervals.push_back(std::make_pair(range(node, v), v));
            std::sort(intervals.begin(), intervals.end());
            for (size_t s = 0; s < node.children.size(); ++s) {
                for (const auto& iv : intervals)
                    if (iv.first.first == static_cast<int>(s)) out.push_back(iv.second);
                emit(node.children[s], out);
                for (const auto& iv : intervals)
                    if (iv.first.second == static_cast<int>(s)) out.push_back(iv.second);
            }
        }
    }

    std::vector<int> normalized(int id) const {
        std::vector<int> result, labels(n, -1);
        emit(id, result);
        int next = 0;
        for (int& v : result) {
            if (labels[v] < 0) labels[v] = next++;
            v = labels[v];
        }
        return result;
    }

    std::vector<int> canonicalize(int id) {
        std::map<int, std::vector<int> > keys;
        for (int c : nodes[id].children) keys[c] = canonicalize(c);
        if (!nodes[id].q) {
            std::sort(nodes[id].children.begin(), nodes[id].children.end(),
                      [&](int a, int b) { return keys[a] < keys[b]; });
        } else {
            const std::vector<int> forward = normalized(id);
            reverse(id);
            if (forward < normalized(id)) reverse(id);
        }
        // A deterministic recursive variant of the ordering in Section 3:
        // (subtree vertex count, owned vertex count, child count, string).
        // Full strings replace the paper's compact ranks.
        std::vector<int> key = normalized(id);
        int owned = 0;
        for (int v = 0; v < n; ++v) if (nodes[id].vertices & (Mask(1) << v)) ++owned;
        const int count = static_cast<int>(key.size() / 2);
        key.insert(key.begin(), static_cast<int>(nodes[id].children.size()));
        key.insert(key.begin(), owned);
        key.insert(key.begin(), count);
        return key;
    }

    Mask index(int id, int parent, int position) {
        Node& node = nodes[id];
        node.parent = parent;
        node.position = position;
        node.subtree = node.vertices;
        for (int v = 0; v < n; ++v) if (node.vertices & (Mask(1) << v)) owner[v] = id;
        for (size_t i = 0; i < node.children.size(); ++i)
            node.subtree |= index(node.children[i], id, static_cast<int>(i));
        return node.subtree;
    }

    bool ancestor(int a, int b) const {
        for (int p = b; p >= 0; p = nodes[p].parent) if (p == a) return true;
        return false;
    }

    bool clique(int id) const { return !nodes[id].q && nodes[id].children.empty(); }

    bool has_neighbor_below(int top, int leaf, int y) const {
        Mask neighbors = nodes[leaf].vertices & ~(Mask(1) << y);
        for (int c = leaf; c != top;) {
            const int p = nodes[c].parent;
            neighbors |= nodes[p].q ? nodes[p].sections[nodes[c].position] : nodes[p].vertices;
            c = p;
        }
        return neighbors != 0;
    }

    void reverse(int id) {
        std::reverse(nodes[id].children.begin(), nodes[id].children.end());
        std::reverse(nodes[id].sections.begin(), nodes[id].sections.end());
    }

    // Only the children on this path are moved. Parent links still identify
    // the same path after rotations; positions are deliberately not consulted.
    void orient_path(int top, int leaf) {
        for (int c = leaf; c != top;) {
            const int p = nodes[c].parent;
            auto& children = nodes[p].children;
            auto at = std::find(children.begin(), children.end(), c);
            if (nodes[p].q) {
                if (at != children.begin()) reverse(p);
            } else std::rotate(children.begin(), at, at + 1);
            c = p;
        }
    }

    static size_t first_position(const std::vector<int>& s, int v) {
        return static_cast<size_t>(std::find(s.begin(), s.end(), v) - s.begin());
    }
    static size_t second_position(const std::vector<int>& s, int v) {
        return s.size() - 1 - static_cast<size_t>(std::find(s.rbegin(), s.rend(), v) - s.rbegin());
    }
    static void move_left_endpoint_right(std::vector<int>& s, int v) {
        size_t i = first_position(s, v);
        while (i + 1 < s.size() && first_position(s, s[i + 1]) == i + 1) {
            std::swap(s[i], s[i + 1]);
            ++i;
        }
    }
    static void move_right_endpoint_left(std::vector<int>& s, int v) {
        size_t i = second_position(s, v);
        while (i && second_position(s, s[i - 1]) == i - 1) {
            std::swap(s[i], s[i - 1]);
            --i;
        }
    }
    static void compact(std::vector<int>& s, int v) {
        move_left_endpoint_right(s, v);
        move_right_endpoint_left(s, v);
    }
    static void separate(std::vector<int>& s, int x, int y) {
        move_left_endpoint_right(s, x);
        const size_t left = first_position(s, x), right = second_position(s, y);
        if (left + 1 != right)
            throw std::logic_error("IntervalMPQTree: nonadjacent rotatable endpoints");
        std::swap(s[left], s[right]);
    }
};

} // namespace detail
} // namespace graph_recognition

#endif
