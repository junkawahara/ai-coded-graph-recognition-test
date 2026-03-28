#ifndef GRAPH_RECOGNITION_PQ_TREE_H
#define GRAPH_RECOGNITION_PQ_TREE_H

/**
 * @file pq_tree.h
 * @brief PQ-tree による Consecutive Ones Property (C1P) 判定
 *
 * Booth & Lueker (1976) の原論文に忠実な実装。
 * "Testing for the Consecutive Ones Property, Interval Graphs,
 *  and Graph Planarity Using PQ-Tree Algorithms"
 * Journal of Computer and System Sciences, 13, 335-379.
 *
 * ノードタイプ:
 *   - Leaf: 列を表す葉ノード
 *   - P-node: 子の順序を任意に並べ替え可能
 *   - Q-node: 子の順序は反転のみ可能
 *
 * テンプレート (原論文 Figure 5-13):
 *   P1: P-node の全子が FULL → FULL に
 *   P2: pertinent root P-node: FULL 子を新 P-node にまとめる
 *   P3: 非 root P-node: Q-node [empty, full] に変換
 *   P4: pertinent root P-node + PARTIAL 子1つ
 *   P5: 非 root P-node + PARTIAL 子1つ
 *   P6: pertinent root P-node + PARTIAL 子2つ
 *   Q1: Q-node の全子が FULL → FULL に
 *   Q2: Q-node + PARTIAL 子1つ
 *   Q3: Q-node + PARTIAL 子2つ (pertinent root のみ)
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
    PQTree() : root_(nullptr), num_cols_(0) {}

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
        if (S.empty() || (int)S.size() <= 1 || (int)S.size() >= num_cols_)
            return true;

        // 前回の dirty ノードのみリセット
        for (size_t i = 0; i < dirty_nodes_.size(); ++i) {
            dirty_nodes_[i]->label = PQLabel::EMPTY;
            dirty_nodes_[i]->pertinent_leaf_count = 0;
            dirty_nodes_[i]->pertinent_child_count = 0;
            dirty_nodes_[i]->mark = false;
        }
        dirty_nodes_.clear();

        int total_s = (int)S.size();

        // pertinent_leaf_count と pertinent_child_count を伝搬
        // pertinent_child_count: pertinent な子(plc > 0)の数
        for (size_t i = 0; i < S.size(); ++i) {
            leaves_[S[i]]->label = PQLabel::FULL;
            leaves_[S[i]]->pertinent_leaf_count = 1;
            dirty_nodes_.push_back(leaves_[S[i]]);
        }

        // ボトムアップ伝搬: 各リーフから root へ
        for (size_t i = 0; i < S.size(); ++i) {
            PQNode* node = leaves_[S[i]]->parent;
            while (node != nullptr) {
                node->pertinent_leaf_count++;
                if (!node->mark) {
                    // 初めて pertinent な子を持った
                    node->mark = true;
                    dirty_nodes_.push_back(node);
                }
                node = node->parent;
            }
        }

        // pertinent_child_count を正確に計算
        for (size_t i = 0; i < dirty_nodes_.size(); ++i) {
            PQNode* node = dirty_nodes_[i];
            if (node->pertinent_leaf_count > 0 && node->parent != nullptr) {
                node->parent->pertinent_child_count++;
            }
        }

        // pertinent root を特定
        PQNode* pertinent_root = find_pertinent_root(total_s);
        if (!pertinent_root) return false;

        // ボトムアップ reduce: pertinent なリーフから始めて
        // pertinent_child_count が 0 になったノードを処理
        std::vector<PQNode*> queue;
        for (size_t i = 0; i < S.size(); ++i) {
            queue.push_back(leaves_[S[i]]);
        }

        size_t head = 0;
        while (head < queue.size()) {
            PQNode* x = queue[head++];

            // リーフはラベル済み (FULL)、テンプレート不要
            if (x->type != PQNodeType::LEAF) {
                bool is_root = (x == pertinent_root);
                if (!apply_template(x, is_root)) return false;
            }

            if (x == pertinent_root) continue;

            // 親にカウントを伝搬
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
    // pertinent root 特定
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
    // テンプレート適用
    // ================================================================

    bool apply_template(PQNode* node, bool is_root) {
        // 子のラベル集計
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
    // P-node テンプレート
    // ================================================================

    bool apply_p(PQNode* node, int full_count, int partial_count,
                 int empty_count, PQNode* partial_ch[2], bool is_root) {
        // P1: 全 FULL
        if (partial_count == 0 && empty_count == 0) {
            node->label = PQLabel::FULL;
            return true;
        }
        // P2: FULL + EMPTY, pertinent root
        if (partial_count == 0 && is_root)
            return template_P2(node, full_count);
        // P3: FULL + EMPTY, 非 root
        if (partial_count == 0 && !is_root)
            return template_P3(node);
        // P4: PARTIAL 1つ, pertinent root
        if (partial_count == 1 && is_root)
            return template_P4(node, partial_ch[0], full_count, empty_count);
        // P5: PARTIAL 1つ, 非 root
        if (partial_count == 1 && !is_root)
            return template_P5(node, partial_ch[0]);
        // P6: PARTIAL 2つ, pertinent root
        if (partial_count == 2 && is_root)
            return template_P6(node, partial_ch[0], partial_ch[1], empty_count);
        // partial >= 3 or partial == 2 && !is_root → 不可能
        return false;
    }

    /**
     * P2: pertinent root, FULL + EMPTY 子のみ。
     * FULL 子が2つ以上なら新 P-node にまとめる。
     */
    bool template_P2(PQNode* node, int full_count) {
        if (full_count <= 1) return true;

        PQNode* fp = make_node(PQNodeType::P_NODE);
        fp->label = PQLabel::FULL;

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
     * P3: 非 root, FULL + EMPTY 子のみ。
     * node を Q-node [empty_group, full_group] に変換。
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
     * P4: pertinent root + PARTIAL 子1つ。
     * FULL 子を PARTIAL Q-node の FULL 端に追加。
     * EMPTY 子は P-node にそのまま残す。
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
     * P5: 非 root + PARTIAL 子1つ。
     * FULL を PARTIAL Q-node の FULL 端、EMPTY を EMPTY 端に追加。
     * node を partial Q-node に置換。
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
     * P6: pertinent root + PARTIAL 子2つ。
     * 2つの PARTIAL を結合し FULL 子を間に挟んだ Q-node を作成。
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

        // p1: [empty..., full...], p2: reverse → [full..., empty...]
        orient_partial(p1);
        orient_partial(p2);
        p2->children.reverse();

        // 新 Q-node: [p1 の子..., full_group, p2 の子...]
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
    // Q-node テンプレート
    // ================================================================

    bool apply_q(PQNode* node, int /*full_count*/, int partial_count,
                 int empty_count, PQNode* partial_ch[2], bool is_root) {
        // Q1: 全 FULL
        if (partial_count == 0 && empty_count == 0) {
            node->label = PQLabel::FULL;
            return true;
        }

        if (partial_count > 2) return false;
        if (partial_count == 2 && !is_root) return false;

        // Q-node の子の配置を検証: pertinent (FULL/PARTIAL) 子が連続しているか
        if (!validate_q_ordering(node, partial_count, is_root)) return false;

        // FULL + EMPTY のみ (PARTIAL なし)
        if (partial_count == 0) {
            if (!is_root) node->label = PQLabel::PARTIAL;
            return true;
        }

        // Q2: PARTIAL 子1つ
        if (partial_count == 1)
            return template_Q2(node, partial_ch[0], is_root);

        // Q3: PARTIAL 子2つ (is_root 確定)
        return template_Q3(node, partial_ch[0], partial_ch[1]);
    }

    // ================================================================
    // Q-node パターン検証
    // ================================================================

    /**
     * Q-node の子の並びが有効か検証。
     *
     * Root:     [E*] [P?] [F*] [P?] [E*]  (F* は0個可)
     * Non-root: [E*] [P?] [F*]  またはその反転
     *
     * 検証前に必要に応じて reverse して正規化する。
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
            // 正規化: FULL/PARTIAL が末尾に来るように
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
     * [E*] [P?] [F*] の形か検証。
     * FULL (と PARTIAL) が末尾に寄っている。
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
    // Q2, Q3 テンプレート
    // ================================================================

    /**
     * Q2: Q-node + PARTIAL 子1つ。
     * PARTIAL を展開して Q-node に吸収。
     */
    bool template_Q2(PQNode* node, PQNode* partial, bool is_root) {
        // validate_q_ordering で正規化済み:
        // non-root: [E*, P?, F*] (pertinent が末尾方向)
        // root: [E*, P?, F*, P?, E*]

        // partial の位置を探す
        std::list<PQNode*>::iterator ppos;
        for (ppos = node->children.begin(); ppos != node->children.end(); ++ppos)
            if (*ppos == partial) break;

        // partial の FULL 端をどちらに向けるか判定。
        // FULL 子がある場合はそちらに向ける。
        // FULL 子がない場合は Q-node の端方向に向ける。
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

        // partial を [empty..., full...] に配向
        orient_partial(partial);

        // FULL 端の方向を決定:
        // - FULL 子が後方 → FULL 端を後方 (orient のまま)
        // - FULL 子が前方 → FULL 端を前方 (reverse)
        // - FULL 子なし → FULL 端を Q-node の最寄りの端に向ける
        //   validate 正規化後、non-root: partial は末尾近く → FULL を後方 (端方向)
        //   root: partial 位置に依存
        bool full_toward_back;
        if (full_after) {
            full_toward_back = true;
        } else if (full_before) {
            full_toward_back = false;
        } else {
            // FULL 子なし: partial の FULL 端を Q-node の末尾に向ける
            full_toward_back = true;
        }

        if (!full_toward_back) {
            partial->children.reverse(); // [full..., empty...]
        }
        // else: orient 済みで [empty..., full...] → FULL が後方

        // partial の子を node に展開
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
     * Q3: Q-node + PARTIAL 子2つ (pertinent root のみ)。
     */
    bool template_Q3(PQNode* node, PQNode* p1, PQNode* p2) {
        // validate_q_ordering で正規化済み: [E*, P, F*, P, E*]
        // p1 が先に出現するよう整列
        bool p1_first = false;
        for (std::list<PQNode*>::iterator it = node->children.begin();
             it != node->children.end(); ++it) {
            if (*it == p1) { p1_first = true; break; }
            if (*it == p2) break;
        }
        if (!p1_first) std::swap(p1, p2);

        // p1 (左側): FULL が右向き (内側) → [empty..., full...]
        orient_partial(p1);
        // p2 (右側): FULL が左向き (内側) → [full..., empty...]
        orient_partial(p2);
        p2->children.reverse();

        // p1 展開
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

        // p2 展開
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
    // ユーティリティ
    // ================================================================

    /**
     * PARTIAL Q-node を [empty..., full...] の向きに配向。
     */
    void orient_partial(PQNode* q) {
        if (q->children.empty()) return;
        // 先頭が FULL なら反転して [empty..., full...] にする
        if (q->children.front()->label == PQLabel::FULL) {
            q->children.reverse();
        }
    }

    /**
     * Q-node が [E*, F+] or [F+, E*] の形か検証。
     * FULL 子が一端に連続して寄っていること。
     * [E, F, E] (FULL が中央) は不可。
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
        // group_nodes のラベルは子に基づく
        // 全 FULL → FULL, 全 EMPTY → EMPTY
        bool all_full = true, all_empty = true;
        for (std::list<PQNode*>::iterator it = nodes.begin();
             it != nodes.end(); ++it) {
            if ((*it)->label != PQLabel::FULL) all_full = false;
            if ((*it)->label != PQLabel::EMPTY) all_empty = false;
        }
        if (all_full) p->label = PQLabel::FULL;
        else if (all_empty) p->label = PQLabel::EMPTY;
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
        // fallback
        root_ = new_node;
        new_node->parent = nullptr;
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
 * @brief PQ-tree による C1P 判定
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
