#ifndef GRAPH_RECOGNITION_PQ_TREE_H
#define GRAPH_RECOGNITION_PQ_TREE_H

/**
 * @file pq_tree.h
 * @brief Consecutive Ones Property (C1P) testing using PQ-tree
 *
 * Implements the reduction templates of the original paper by Booth & Lueker
 * (1976). "Testing for the Consecutive Ones Property, Interval Graphs,
 *  and Graph Planarity Using PQ-Tree Algorithms"
 * Journal of Computer and System Sciences, 13, 335-379.
 *
 * Complexity: this implementation is correct but not linear time. The BUBBLE
 * pass of the original paper (which restricts the bottom-up sweep to the
 * pertinent subtree) is not implemented; instead every reduction walks from
 * each pertinent leaf all the way up to the tree root. A reduction of a row
 * with |S| ones therefore costs O(|S| * depth), giving O(n * m) in the worst
 * case over a matrix with m ones and n columns, rather than the O(n + m) of
 * Booth & Lueker.
 *
 * Node types:
 *   - Leaf: Leaf node representing a column
 *   - P-node: Children can be reordered arbitrarily
 *   - Q-node: Children can only be reversed in order
 *
 * Templates (from original paper Figures 5-13):
 *   P1: All children of P-node are FULL -> mark as FULL
 *   P2: Pertinent root P-node: group FULL children into a new P-node
 *   P3: Non-root P-node: convert to Q-node [empty, full]
 *   P4: Pertinent root P-node + 1 PARTIAL child
 *   P5: Non-root P-node + 1 PARTIAL child
 *   P6: Pertinent root P-node + 2 PARTIAL children
 *   Q1: All children of Q-node are FULL -> mark as FULL
 *   Q2: Q-node + 1 PARTIAL child
 *   Q3: Q-node + 2 PARTIAL children (pertinent root only)
 */

#include <algorithm>
#include <cassert>
#include <list>
#include <vector>

namespace graph_recognition {
namespace detail {

enum class PQNodeType { LEAF, P_NODE, Q_NODE };
enum class PQLabel { EMPTY, FULL, PARTIAL };

struct PQNode {
    PQNodeType type;
    PQLabel label;
    int leaf_value;

    PQNode* parent;
    std::list<PQNode*> children;

    int pertinent_leaf_count;
    int pertinent_child_count;
    bool mark;

    PQNode()
        : type(PQNodeType::LEAF), label(PQLabel::EMPTY), leaf_value(-1),
          parent(nullptr), pertinent_leaf_count(0), pertinent_child_count(0),
          mark(false) {}
};

class PQTree {
public:
    PQTree() : root_(nullptr), num_cols_(0), corrupt_(false) {}

    PQTree(const PQTree&) = delete;
    PQTree& operator=(const PQTree&) = delete;

    ~PQTree() {
        for (size_t i = 0; i < all_nodes_.size(); ++i)
            delete all_nodes_[i];
    }

    void initialize(int num_cols) {
        num_cols_ = num_cols;
        leaves_.resize(num_cols, nullptr);

        if (num_cols == 0) { root_ = nullptr; return; }
        if (num_cols == 1) {
            root_ = make_leaf(0);
            return;
        }

        PQNode* p = make_node(PQNodeType::P_NODE);
        for (int i = 0; i < num_cols; ++i) {
            PQNode* leaf = make_leaf(i);
            add_child(p, leaf);
        }
        root_ = p;
    }

    bool reduce(const std::vector<int>& S) {
        if (corrupt_) return false;
        if (S.empty() || (int)S.size() <= 1 || (int)S.size() >= num_cols_)
            return true;

        // Reset only previously dirty nodes
        for (size_t i = 0; i < dirty_nodes_.size(); ++i) {
            dirty_nodes_[i]->label = PQLabel::EMPTY;
            dirty_nodes_[i]->pertinent_leaf_count = 0;
            dirty_nodes_[i]->pertinent_child_count = 0;
            dirty_nodes_[i]->mark = false;
        }
        dirty_nodes_.clear();

        int total_s = (int)S.size();

        // Propagate pertinent_leaf_count and pertinent_child_count
        // pertinent_child_count: number of pertinent children (plc > 0)
        for (size_t i = 0; i < S.size(); ++i) {
            leaves_[S[i]]->label = PQLabel::FULL;
            leaves_[S[i]]->pertinent_leaf_count = 1;
            dirty_nodes_.push_back(leaves_[S[i]]);
        }

        // Bottom-up propagation: from each leaf to root
        for (size_t i = 0; i < S.size(); ++i) {
            PQNode* node = leaves_[S[i]]->parent;
            while (node != nullptr) {
                node->pertinent_leaf_count++;
                if (!node->mark) {
                    // First time having a pertinent child
                    node->mark = true;
                    dirty_nodes_.push_back(node);
                }
                node = node->parent;
            }
        }

        // Compute pertinent_child_count accurately
        for (size_t i = 0; i < dirty_nodes_.size(); ++i) {
            PQNode* node = dirty_nodes_[i];
            if (node->pertinent_leaf_count > 0 && node->parent != nullptr) {
                node->parent->pertinent_child_count++;
            }
        }

        // Identify pertinent root
        PQNode* pertinent_root = find_pertinent_root(total_s);
        if (!pertinent_root) return false;

        // Bottom-up reduce: starting from pertinent leaves,
        // process nodes whose pertinent_child_count becomes 0
        std::vector<PQNode*> queue;
        for (size_t i = 0; i < S.size(); ++i) {
            queue.push_back(leaves_[S[i]]);
        }

        size_t head = 0;
        while (head < queue.size()) {
            PQNode* x = queue[head++];

            // Leaf is already labeled (FULL), no template needed
            if (x->type != PQNodeType::LEAF) {
                bool is_root = (x == pertinent_root);
                if (!apply_template(x, is_root) || corrupt_) return false;
            }

            if (x == pertinent_root) continue;

            // Propagate count to parent
            PQNode* par = x->parent;
            if (par != nullptr) {
                par->pertinent_child_count--;
                if (par->pertinent_child_count == 0) {
                    queue.push_back(par);
                }
            }
        }

        return true;
    }

    void frontier(std::vector<int>& out) const {
        out.clear();
        out.reserve(num_cols_);
        if (root_) collect_frontier(root_, out);
    }

private:
    PQNode* root_;
    int num_cols_;
    bool corrupt_; /**< Set when a structural invariant violation is detected */
    std::vector<PQNode*> leaves_;
    std::vector<PQNode*> all_nodes_;
    std::vector<PQNode*> dirty_nodes_;

    PQNode* make_node(PQNodeType type) {
        PQNode* n = new PQNode();
        n->type = type;
        all_nodes_.push_back(n);
        return n;
    }

    PQNode* make_leaf(int val) {
        PQNode* n = make_node(PQNodeType::LEAF);
        n->leaf_value = val;
        leaves_[val] = n;
        return n;
    }

    void add_child(PQNode* par, PQNode* child) {
        child->parent = par;
        par->children.push_back(child);
    }

    void add_child_front(PQNode* par, PQNode* child) {
        child->parent = par;
        par->children.push_front(child);
    }

    void add_child_back(PQNode* par, PQNode* child) {
        child->parent = par;
        par->children.push_back(child);
    }

    // ================================================================
    // Pertinent root identification
    // ================================================================

    PQNode* find_pertinent_root(int total_s) {
        // Walk up from any pertinent leaf to find the deepest node
        // with pertinent_leaf_count >= total_s. O(depth) instead of O(tree_size).
        PQNode* start = nullptr;
        for (size_t i = 0; i < dirty_nodes_.size(); ++i) {
            if (dirty_nodes_[i]->type == PQNodeType::LEAF &&
                dirty_nodes_[i]->pertinent_leaf_count > 0) {
                start = dirty_nodes_[i];
                break;
            }
        }
        if (!start) return nullptr;
        PQNode* node = start;
        while (node != nullptr) {
            if (node->pertinent_leaf_count >= total_s) return node;
            node = node->parent;
        }
        return nullptr;
    }

    // ================================================================
    // Template application
    // ================================================================

    bool apply_template(PQNode* node, bool is_root) {
        // Tally child labels
        int full_count = 0, partial_count = 0, empty_count = 0;
        PQNode* partial_ch[2] = {nullptr, nullptr};

        for (std::list<PQNode*>::iterator it = node->children.begin();
             it != node->children.end(); ++it) {
            PQLabel lb = (*it)->label;
            if (lb == PQLabel::FULL) {
                full_count++;
            } else if (lb == PQLabel::PARTIAL) {
                if (partial_count < 2) partial_ch[partial_count] = *it;
                partial_count++;
            } else {
                empty_count++;
            }
        }

        if (node->type == PQNodeType::P_NODE) {
            return apply_p(node, full_count, partial_count, empty_count,
                          partial_ch, is_root);
        } else {
            return apply_q(node, full_count, partial_count, empty_count,
                          partial_ch, is_root);
        }
    }

    // ================================================================
    // P-node templates
    // ================================================================

    bool apply_p(PQNode* node, int full_count, int partial_count,
                 int empty_count, PQNode* partial_ch[2], bool is_root) {
        // P1: all FULL
        if (partial_count == 0 && empty_count == 0) {
            node->label = PQLabel::FULL;
            return true;
        }
        // P2: FULL + EMPTY, pertinent root
        if (partial_count == 0 && is_root)
            return template_P2(node, full_count);
        // P3: FULL + EMPTY, non-root
        if (partial_count == 0 && !is_root)
            return template_P3(node);
        // P4: 1 PARTIAL, pertinent root
        if (partial_count == 1 && is_root)
            return template_P4(node, partial_ch[0], full_count, empty_count);
        // P5: 1 PARTIAL, non-root
        if (partial_count == 1 && !is_root)
            return template_P5(node, partial_ch[0]);
        // P6: 2 PARTIAL, pertinent root
        if (partial_count == 2 && is_root)
            return template_P6(node, partial_ch[0], partial_ch[1], empty_count);
        // partial >= 3 or partial == 2 && !is_root -> impossible
        return false;
    }

    /**
     * P2: pertinent root, FULL + EMPTY children only.
     * If there are 2 or more FULL children, group them into a new P-node.
     */
    bool template_P2(PQNode* node, int full_count) {
        if (full_count <= 1) return true;

        PQNode* fp = make_node(PQNodeType::P_NODE);
        fp->label = PQLabel::FULL;
        dirty_nodes_.push_back(fp);

        std::list<PQNode*>::iterator it = node->children.begin();
        while (it != node->children.end()) {
            if ((*it)->label == PQLabel::FULL) {
                (*it)->parent = fp;
                fp->children.push_back(*it);
                it = node->children.erase(it);
            } else {
                ++it;
            }
        }
        add_child(node, fp);
        return true;
    }

    /**
     * P3: non-root, FULL + EMPTY children only.
     * Convert node to Q-node [empty_group, full_group].
     */
    bool template_P3(PQNode* node) {
        std::list<PQNode*> full_ch, empty_ch;
        for (std::list<PQNode*>::iterator it = node->children.begin();
             it != node->children.end(); ++it) {
            if ((*it)->label == PQLabel::FULL) full_ch.push_back(*it);
            else empty_ch.push_back(*it);
        }

        PQNode* fg = group_nodes(full_ch);
        PQNode* eg = group_nodes(empty_ch);

        node->type = PQNodeType::Q_NODE;
        node->label = PQLabel::PARTIAL;
        node->children.clear();

        add_child(node, eg);
        add_child(node, fg);
        return true;
    }

    /**
     * P4: pertinent root + 1 PARTIAL child.
     * Append FULL children to the FULL end of the PARTIAL Q-node.
     * EMPTY children remain in the P-node.
     */
    bool template_P4(PQNode* node, PQNode* partial,
                     int full_count, int empty_count) {
        orient_partial(partial); // [empty..., full...]

        if (full_count > 0) {
            std::list<PQNode*> full_ch;
            std::list<PQNode*>::iterator it = node->children.begin();
            while (it != node->children.end()) {
                if (*it != partial && (*it)->label == PQLabel::FULL) {
                    full_ch.push_back(*it);
                    it = node->children.erase(it);
                } else {
                    ++it;
                }
            }
            PQNode* fg = group_nodes(full_ch);
            add_child_back(partial, fg);
        }

        if (empty_count == 0) {
            replace_in_parent(node, partial);
        }
        return true;
    }

    /**
     * P5: non-root + 1 PARTIAL child.
     * Append FULL children to the FULL end, EMPTY children to the EMPTY end of the PARTIAL Q-node.
     * Replace node with the partial Q-node.
     */
    bool template_P5(PQNode* node, PQNode* partial) {
        std::list<PQNode*> full_ch, empty_ch;
        for (std::list<PQNode*>::iterator it = node->children.begin();
             it != node->children.end(); ++it) {
            if (*it == partial) continue;
            if ((*it)->label == PQLabel::FULL) full_ch.push_back(*it);
            else empty_ch.push_back(*it);
        }

        orient_partial(partial); // [empty..., full...]

        if (!full_ch.empty()) {
            PQNode* fg = group_nodes(full_ch);
            add_child_back(partial, fg);
        }
        if (!empty_ch.empty()) {
            PQNode* eg = group_nodes(empty_ch);
            add_child_front(partial, eg);
        }

        partial->label = PQLabel::PARTIAL;
        replace_in_parent(node, partial);
        return true;
    }

    /**
     * P6: pertinent root + 2 PARTIAL children.
     * Merge 2 PARTIAL children and create a Q-node with FULL children sandwiched between them.
     */
    bool template_P6(PQNode* node, PQNode* p1, PQNode* p2, int empty_count) {
        std::list<PQNode*> full_ch;
        std::list<PQNode*>::iterator it = node->children.begin();
        while (it != node->children.end()) {
            if (*it != p1 && *it != p2 && (*it)->label == PQLabel::FULL) {
                full_ch.push_back(*it);
                it = node->children.erase(it);
            } else {
                ++it;
            }
        }

        node->children.remove(p1);
        node->children.remove(p2);

        // p1: [empty..., full...], p2: reverse -> [full..., empty...]
        orient_partial(p1);
        orient_partial(p2);
        p2->children.reverse();

        // New Q-node: [children of p1..., full_group, children of p2...]
        PQNode* qn = make_node(PQNodeType::Q_NODE);

        for (std::list<PQNode*>::iterator ci = p1->children.begin();
             ci != p1->children.end(); ++ci)
            (*ci)->parent = qn;
        qn->children.splice(qn->children.end(), p1->children);

        if (!full_ch.empty()) {
            PQNode* fg = group_nodes(full_ch);
            add_child_back(qn, fg);
        }

        for (std::list<PQNode*>::iterator ci = p2->children.begin();
             ci != p2->children.end(); ++ci)
            (*ci)->parent = qn;
        qn->children.splice(qn->children.end(), p2->children);

        add_child(node, qn);

        if (empty_count == 0) {
            replace_in_parent(node, qn);
        }

        return true;
    }

    // ================================================================
    // Q-node templates
    // ================================================================

    bool apply_q(PQNode* node, int /*full_count*/, int partial_count,
                 int empty_count, PQNode* partial_ch[2], bool is_root) {
        // Q1: all FULL
        if (partial_count == 0 && empty_count == 0) {
            node->label = PQLabel::FULL;
            return true;
        }

        if (partial_count > 2) return false;
        if (partial_count == 2 && !is_root) return false;

        // Verify Q-node child ordering: pertinent (FULL/PARTIAL) children must be consecutive
        if (!validate_q_ordering(node, partial_count, is_root)) return false;

        // FULL + EMPTY only (no PARTIAL)
        if (partial_count == 0) {
            if (!is_root) node->label = PQLabel::PARTIAL;
            return true;
        }

        // Q2: 1 PARTIAL child
        if (partial_count == 1)
            return template_Q2(node, partial_ch[0], is_root);

        // Q3: 2 PARTIAL children (is_root guaranteed)
        return template_Q3(node, partial_ch[0], partial_ch[1]);
    }

    // ================================================================
    // Q-node pattern verification
    // ================================================================

    /**
     * Verify whether the ordering of Q-node children is valid.
     *
     * Root:     [E*] [P?] [F*] [P?] [E*]  (F* can be 0)
     * Non-root: [E*] [P?] [F*]  or its reverse
     *
     * Normalizes by reversing if needed before verification.
     */
    bool validate_q_ordering(PQNode* node, int /*partial_count*/, bool is_root) {
        if (is_root) {
            // [E* P? F* P? E*] or reverse
            if (check_q_root_pattern(node)) return true;
            node->children.reverse();
            if (check_q_root_pattern(node)) return true;
            return false;
        } else {
            // [E* P? F*] or reverse
            // Normalize: place FULL/PARTIAL at the end
            if (check_q_nonroot_pattern(node)) return true;
            node->children.reverse();
            if (check_q_nonroot_pattern(node)) return true;
            return false;
        }
    }

    bool check_q_root_pattern(PQNode* node) {
        // [E*] [P?] [F*] [P?] [E*]
        int phase = 0;
        for (std::list<PQNode*>::iterator it = node->children.begin();
             it != node->children.end(); ++it) {
            PQLabel lb = (*it)->label;
            switch (phase) {
            case 0: // E_left
                if (lb == PQLabel::EMPTY) break;
                if (lb == PQLabel::PARTIAL) { phase = 1; break; }
                if (lb == PQLabel::FULL) { phase = 2; break; }
                return false;
            case 1: // after 1st PARTIAL
                if (lb == PQLabel::FULL) { phase = 2; break; }
                if (lb == PQLabel::PARTIAL) { phase = 3; break; }
                if (lb == PQLabel::EMPTY) { phase = 4; break; }
                return false;
            case 2: // FULL region
                if (lb == PQLabel::FULL) break;
                if (lb == PQLabel::PARTIAL) { phase = 3; break; }
                if (lb == PQLabel::EMPTY) { phase = 4; break; }
                return false;
            case 3: // after 2nd PARTIAL
                if (lb == PQLabel::EMPTY) { phase = 4; break; }
                return false;
            case 4: // E_right
                if (lb == PQLabel::EMPTY) break;
                return false;
            }
        }
        return true;
    }

    /**
     * Verify whether the pattern is [E*] [P?] [F*].
     * FULL (and PARTIAL) are grouped toward the end.
     */
    bool check_q_nonroot_pattern(PQNode* node) {
        int phase = 0;
        for (std::list<PQNode*>::iterator it = node->children.begin();
             it != node->children.end(); ++it) {
            PQLabel lb = (*it)->label;
            switch (phase) {
            case 0: // EMPTY region
                if (lb == PQLabel::EMPTY) break;
                if (lb == PQLabel::PARTIAL) { phase = 1; break; }
                if (lb == PQLabel::FULL) { phase = 2; break; }
                return false;
            case 1: // after PARTIAL
                if (lb == PQLabel::FULL) { phase = 2; break; }
                return false;
            case 2: // FULL region
                if (lb == PQLabel::FULL) break;
                return false;
            }
        }
        return true;
    }

    // ================================================================
    // Q2, Q3 templates
    // ================================================================

    /**
     * Q2: Q-node + 1 PARTIAL child.
     * Expand PARTIAL and absorb into Q-node.
     */
    bool template_Q2(PQNode* node, PQNode* partial, bool is_root) {
        // Already normalized by validate_q_ordering:
        // non-root: [E*, P?, F*] (pertinent toward the end)
        // root: [E*, P?, F*, P?, E*]

        // Find position of partial
        std::list<PQNode*>::iterator ppos;
        for (ppos = node->children.begin(); ppos != node->children.end(); ++ppos)
            if (*ppos == partial) break;

        // Determine which direction to orient the FULL end of partial.
        // Orient toward existing FULL children if any.
        // Otherwise, orient toward the nearest end of the Q-node.
        bool full_after = false;
        bool full_before = false;
        {
            std::list<PQNode*>::iterator tmp = ppos;
            ++tmp;
            while (tmp != node->children.end()) {
                if ((*tmp)->label == PQLabel::FULL) { full_after = true; break; }
                ++tmp;
            }
        }
        if (!full_after) {
            std::list<PQNode*>::iterator tmp = ppos;
            while (tmp != node->children.begin()) {
                --tmp;
                if ((*tmp)->label == PQLabel::FULL) { full_before = true; break; }
            }
        }

        // Orient partial to [empty..., full...]
        orient_partial(partial);

        // Determine FULL end direction:
        // - FULL children after -> FULL end toward back (keep orientation)
        // - FULL children before -> FULL end toward front (reverse)
        // - No FULL children -> orient FULL end toward nearest end of Q-node
        //   After validate normalization, non-root: partial is near end -> FULL toward back (end direction)
        //   root: depends on partial position
        bool full_toward_back;
        if (full_after) {
            full_toward_back = true;
        } else if (full_before) {
            full_toward_back = false;
        } else {
            // No FULL children: orient FULL end of partial toward the end of Q-node
            full_toward_back = true;
        }

        if (!full_toward_back) {
            partial->children.reverse(); // [full..., empty...]
        }
        // else: already oriented as [empty..., full...] -> FULL toward back

        // Expand partial's children into node
        for (std::list<PQNode*>::iterator it = partial->children.begin();
             it != partial->children.end(); ++it)
            (*it)->parent = node;

        node->children.splice(ppos, partial->children);
        node->children.erase(ppos);

        if (!is_root) {
            if (!verify_partial_form(node)) return false;
            node->label = PQLabel::PARTIAL;
        }
        return true;
    }

    /**
     * Q3: Q-node + 2 PARTIAL children (pertinent root only).
     */
    bool template_Q3(PQNode* node, PQNode* p1, PQNode* p2) {
        // Already normalized by validate_q_ordering: [E*, P, F*, P, E*]
        // Ensure p1 appears first
        bool p1_first = false;
        for (std::list<PQNode*>::iterator it = node->children.begin();
             it != node->children.end(); ++it) {
            if (*it == p1) { p1_first = true; break; }
            if (*it == p2) break;
        }
        if (!p1_first) std::swap(p1, p2);

        // p1 (left): FULL end toward right (inward) -> [empty..., full...]
        orient_partial(p1);
        // p2 (right): FULL end toward left (inward) -> [full..., empty...]
        orient_partial(p2);
        p2->children.reverse();

        // Expand p1
        {
            std::list<PQNode*>::iterator pos;
            for (pos = node->children.begin(); pos != node->children.end(); ++pos)
                if (*pos == p1) break;
            for (std::list<PQNode*>::iterator it = p1->children.begin();
                 it != p1->children.end(); ++it)
                (*it)->parent = node;
            node->children.splice(pos, p1->children);
            node->children.erase(pos);
        }

        // Expand p2
        {
            std::list<PQNode*>::iterator pos;
            for (pos = node->children.begin(); pos != node->children.end(); ++pos)
                if (*pos == p2) break;
            for (std::list<PQNode*>::iterator it = p2->children.begin();
                 it != p2->children.end(); ++it)
                (*it)->parent = node;
            node->children.splice(pos, p2->children);
            node->children.erase(pos);
        }

        return true;
    }

    // ================================================================
    // Utilities
    // ================================================================

    /**
     * Orient a PARTIAL Q-node to [empty..., full...] ordering.
     */
    void orient_partial(PQNode* q) {
        if (q->children.empty()) return;
        // If the front is FULL, reverse to get [empty..., full...]
        if (q->children.front()->label == PQLabel::FULL) {
            q->children.reverse();
        }
    }

    /**
     * Verify whether Q-node has the form [E*, F+] or [F+, E*].
     * FULL children must be consecutively grouped at one end.
     * [E, F, E] (FULL in the middle) is invalid.
     */
    bool verify_partial_form(PQNode* node) {
        bool has_full = false, has_empty = false;
        int transitions = 0;
        PQLabel prev = PQLabel::EMPTY;
        bool first = true;

        for (std::list<PQNode*>::iterator it = node->children.begin();
             it != node->children.end(); ++it) {
            PQLabel lb = (*it)->label;
            if (lb == PQLabel::PARTIAL) return false;
            if (lb == PQLabel::FULL) has_full = true;
            if (lb == PQLabel::EMPTY) has_empty = true;
            if (!first && lb != prev) transitions++;
            prev = lb;
            first = false;
        }
        // Valid: [E+, F+] (1 transition) or [F+, E+] (1 transition)
        // Invalid: [E, F, E] (2 transitions)
        return has_full && has_empty && transitions <= 1;
    }

    PQNode* group_nodes(std::list<PQNode*>& nodes) {
        if (nodes.size() == 1) return nodes.front();
        PQNode* p = make_node(PQNodeType::P_NODE);
        for (std::list<PQNode*>::iterator it = nodes.begin();
             it != nodes.end(); ++it) {
            (*it)->parent = p;
            p->children.push_back(*it);
        }
        // Label of group_nodes is based on children
        // all FULL -> FULL, all EMPTY -> EMPTY
        bool all_full = true, all_empty = true;
        for (std::list<PQNode*>::iterator it = nodes.begin();
             it != nodes.end(); ++it) {
            if ((*it)->label != PQLabel::FULL) all_full = false;
            if ((*it)->label != PQLabel::EMPTY) all_empty = false;
        }
        if (all_full) {
            p->label = PQLabel::FULL;
            dirty_nodes_.push_back(p);
        } else if (all_empty) {
            p->label = PQLabel::EMPTY;
        }
        return p;
    }

    void replace_in_parent(PQNode* old_node, PQNode* new_node) {
        if (old_node == root_) {
            root_ = new_node;
            new_node->parent = nullptr;
            return;
        }
        PQNode* par = old_node->parent;
        for (std::list<PQNode*>::iterator it = par->children.begin();
             it != par->children.end(); ++it) {
            if (*it == old_node) {
                *it = new_node;
                new_node->parent = par;
                return;
            }
        }
        // Reaching here means the parent pointer and the parent's child list
        // disagree, which is a structural invariant violation. Fail loudly in
        // debug builds; under -DNDEBUG the assert vanishes, so also mark the
        // tree corrupt so that reduce() reports failure instead of silently
        // continuing with new_node detached from the tree.
        assert(false && "PQTree: old_node not found in its parent's child list");
        corrupt_ = true;
    }

    void collect_frontier(const PQNode* node, std::vector<int>& out) const {
        if (node->type == PQNodeType::LEAF) {
            out.push_back(node->leaf_value);
            return;
        }
        for (std::list<PQNode*>::const_iterator it = node->children.begin();
             it != node->children.end(); ++it) {
            collect_frontier(*it, out);
        }
    }

};

/**
 * @brief C1P testing using PQ-tree
 */
inline bool check_c1p_pq_tree(
    const std::vector<std::vector<int>>& rows,
    int num_cols,
    std::vector<int>& out_perm) {

    if (num_cols == 0) { out_perm.clear(); return true; }
    if (rows.empty()) {
        out_perm.resize(num_cols);
        for (int i = 0; i < num_cols; ++i) out_perm[i] = i;
        return true;
    }

    PQTree tree;
    tree.initialize(num_cols);

    for (size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].empty()) continue;
        if ((int)rows[i].size() >= num_cols) continue;
        if ((int)rows[i].size() <= 1) continue;
        if (!tree.reduce(rows[i])) return false;
    }

    tree.frontier(out_perm);
    return true;
}

} // namespace detail
} // namespace graph_recognition

#endif
