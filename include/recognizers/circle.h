#ifndef GRAPH_RECOGNITION_CIRCLE_H
#define GRAPH_RECOGNITION_CIRCLE_H

/**
 * @file circle.h
 * @brief Circle graph recognition (Naji's linear system / DOW backtracking)
 *
 * A circle graph is the intersection graph of chords of a circle.
 * G is a circle graph iff there exists a double occurrence word (DOW) of length 2n
 * such that vertices i, j are adjacent iff the occurrences of i, j interleave in the DOW.
 *
 * Two algorithms are provided:
 *
 * - NAJI_SYSTEM (default): polynomial-time decision via Naji's theorem.
 *   G is a circle graph iff the following system over GF(2), with one
 *   variable beta(u, v) for every ordered pair of distinct vertices, is
 *   solvable:
 *     NS1: beta(v,w) + beta(w,v) = 1
 *          for every edge vw
 *     NS2: beta(x,v) + beta(x,w) = 0
 *          for every edge vw and every x adjacent to neither v nor w
 *     NS3: beta(v,w) + beta(w,v) + beta(x,v) + beta(x,w) = 1
 *          for every non-adjacent pair {v,w} and every x adjacent to both
 *   Solvability is decided by bitset Gaussian elimination, so the whole
 *   check runs in polynomial time (roughly O(#NS3 * V + rank^2 * V) bit
 *   operations / 64, where V = m + sum_x #components(G - N[x]) is the
 *   number of variables left after the reductions described below).
 *   This algorithm decides membership only; it does not produce a DOW.
 *
 * - DOW_BACKTRACKING: constructs an explicit DOW by backtracking.
 *   Produces a certificate but takes exponential time in the worst case
 *   (practical up to roughly n = 9; NO answers are the expensive side).
 *   The search counts its steps against a budget (default
 *   circle_dow_default_budget, 0 = unlimited) and throws
 *   std::runtime_error when it is exhausted, so a hard instance raises an
 *   explicit error instead of running forever.
 *
 * Resource limits of NAJI_SYSTEM: the reduced basis is stored densely, so
 * memory grows as Theta(rank * V) bits with V = m + sum_x components(G -
 * N[x]) -- roughly V^2/8 bytes in the worst case (a few hundred MB around
 * n ~ 600 on dense random graphs). Two mitigations apply:
 *   - The input is first shrunk by twin contraction (circle graphs are
 *     closed under adding/removing both true and false twins), which
 *     collapses e.g. the star K_{1,n-1} -- whose raw system has (n-1)^2
 *     variables -- to a single edge.
 *   - The solver tracks its actual basis allocation against a limit
 *     (default circle_naji_default_memory_limit_words, 0 = unlimited) and
 *     throws std::runtime_error when it is exceeded: an explicit refusal,
 *     never an OOM kill.
 *
 * References:
 * - W. Naji, "Reconnaissance des graphes de cordes",
 *   Discrete Mathematics 54 (1985) 329-337.
 * - E. Gasse, "A proof of a circle graph characterization",
 *   Discrete Mathematics 173 (1997) 277-283.
 * - J. Geelen and E. Lee, "Naji's characterization of circle graphs",
 *   Journal of Graph Theory 93 (2020) 21-33 (arXiv:1807.10988).
 */

#include <algorithm>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "util/graph.h"
#include "decompositions/twins.h"

namespace graph_recognition {

enum class CircleAlgorithm {
    NAJI_SYSTEM,     /**< Naji's GF(2) linear system (polynomial time, decision only) (default) */
    DOW_BACKTRACKING /**< DOW backtracking (exponential time, produces a DOW certificate) */
};

/** @brief Default cap on the Naji solver's dense basis: 2^27 64-bit words
 *         (1 GiB). Exceeding it throws std::runtime_error. 0 = unlimited. */
const std::size_t circle_naji_default_memory_limit_words = (std::size_t)1 << 27;

/** @brief Default step budget for DOW backtracking (a step is one dow_dfs
 *         call, costing O(n^2) constraint checks; the default bounds a hard
 *         NO instance to roughly ten seconds). Exceeding it throws
 *         std::runtime_error. 0 = unlimited. */
const unsigned long long circle_dow_default_budget = 20000000ULL;

struct CircleResult {
    bool is_circle = false;
    std::vector<int> dow; /**< double occurrence word (length 2n); only filled by DOW_BACKTRACKING */
};

namespace detail_circle {

typedef unsigned long long U64;

/** @brief Index of the lowest set bit (x must be nonzero) */
inline int lowest_bit_index(U64 x) {
#if defined(__GNUC__)
    return __builtin_ctzll(x);
#else
    int i = 0;
    while (!(x & 1ULL)) { x >>= 1; ++i; }
    return i;
#endif
}

/**
 * @brief Incremental GF(2) linear system solver (bitset Gaussian elimination)
 *
 * Rows are added one at a time and reduced against a basis kept in reduced
 * row echelon form (every basis row has a 0 in every other basis row's pivot
 * column). Keeping the basis reduced bounds the number of XOR passes per
 * added equation by the number of nonzero coefficients of that equation.
 * The constant term lives in an extra column past the variable columns, so
 * an inconsistency shows up as a row whose only remaining bit is that
 * column.
 */
class NajiGf2System {
public:
    NajiGf2System(int num_vars, std::size_t max_words)
        : cols_(num_vars), words_((num_vars + 1 + 63) / 64),
          max_words_(max_words), used_words_(0),
          rows_(), pivot_row_(num_vars, -1), scratch_(words_, 0) {}

    /**
     * @brief Adds the equation "sum of vars = rhs" to the system
     * @param vars Variable indices (a variable appearing twice cancels)
     * @param count Number of entries in vars
     * @param rhs Right-hand side (0 or 1)
     * @return false iff the equation is inconsistent with the system
     */
    bool add_equation(const int* vars, int count, int rhs) {
        std::fill(scratch_.begin(), scratch_.end(), 0ULL);
        for (int i = 0; i < count; ++i) toggle(scratch_, vars[i]);
        if (rhs) toggle(scratch_, cols_);
        reduce(scratch_);
        int c = lowest_set_column(scratch_);
        if (c < 0) return true;        // linear combination of earlier rows
        if (c == cols_) return false;  // reduced to "0 = 1"
        // Keep the basis reduced: clear the new pivot column in older rows.
        for (size_t r = 0; r < rows_.size(); ++r) {
            if (test(rows_[r], c)) xor_from(rows_[r], scratch_, c >> 6);
        }
        // The dense basis is the dominant allocation; refuse to grow past
        // the limit (explicit error instead of an OOM kill).
        used_words_ += (std::size_t)words_;
        if (max_words_ != 0 && used_words_ > max_words_)
            throw std::runtime_error(
                "circle: Naji linear system exceeded its memory limit; "
                "result unknown");
        pivot_row_[c] = (int)rows_.size();
        rows_.push_back(scratch_);
        return true;
    }

private:
    int cols_;   /**< constant column index; variable columns are [0, cols_) */
    int words_;
    std::size_t max_words_;  /**< basis size cap in 64-bit words (0 = unlimited) */
    std::size_t used_words_; /**< words currently held by the basis rows */
    std::vector<std::vector<U64> > rows_;  /**< basis rows (reduced row echelon form) */
    std::vector<int> pivot_row_;           /**< variable column -> basis row index (-1: free) */
    std::vector<U64> scratch_;

    static void toggle(std::vector<U64>& row, int c) {
        row[c >> 6] ^= (1ULL << (c & 63));
    }
    static bool test(const std::vector<U64>& row, int c) {
        return ((row[c >> 6] >> (c & 63)) & 1ULL) != 0;
    }
    static void xor_from(std::vector<U64>& dst, const std::vector<U64>& src,
                         int first_word) {
        for (size_t w = (size_t)first_word; w < dst.size(); ++w) dst[w] ^= src[w];
    }

    /**
     * @brief XORs away every bit of row lying on a pivot column
     *
     * Scans columns in ascending order. A basis row's lowest set bit is its
     * pivot, so XORing it in never touches columns below the current one;
     * bits already scanned stay cleared.
     */
    void reduce(std::vector<U64>& row) const {
        for (int w = 0; w < words_; ++w) {
            U64 pending = row[w];
            while (pending) {
                int b = lowest_bit_index(pending);
                int c = (w << 6) + b;
                if (c >= cols_) break;  // constant column and padding carry no pivots
                if (pivot_row_[c] >= 0) {
                    xor_from(row, rows_[pivot_row_[c]], w);
                    pending = (b == 63) ? 0ULL : (row[w] & (~0ULL << (b + 1)));
                } else {
                    pending &= pending - 1;  // free column: keep the bit, move on
                }
            }
        }
    }

    int lowest_set_column(const std::vector<U64>& row) const {
        for (int w = 0; w < words_; ++w) {
            if (row[w]) return (w << 6) + lowest_bit_index(row[w]);
        }
        return -1;
    }
};

/**
 * @brief Naji's linear system on the graph as given (no twin contraction)
 *
 * The raw Naji system has n(n-1) variables, which this routine shrinks
 * before elimination:
 *
 * - NS1 fixes beta(w,v) = beta(v,w) + 1 on every edge, so one variable
 *   X_uv per edge {u,v} (u < v) suffices: beta(u,v) = X_uv + [u > v].
 * - NS2 says exactly that beta(x, .) is constant on every connected
 *   component of G - N[x] (its equations are the edges of that graph), so
 *   one variable per (vertex x, component of G - N[x]) pair suffices and
 *   all NS2 equations disappear.
 *
 * Only the NS3 equations remain; each involves two edge variables and two
 * component variables. The system is solvable iff the reduced system is.
 */
inline CircleResult check_circle_naji_system(const Graph& g,
                                             std::size_t memory_limit_words) {
    CircleResult res;
    res.is_circle = false;
    int n = g.n;
    if (n <= 1) {
        res.is_circle = true;
        return res;
    }

    std::vector<std::vector<char> > adj(n + 1, std::vector<char>(n + 1, 0));
    for (int u = 1; u <= n; ++u)
        for (size_t j = 0; j < g.adj[u].size(); ++j) adj[u][g.adj[u][j]] = 1;

    int num_vars = 0;

    // One variable per edge {u, v} with u < v (NS1 elimination).
    std::vector<std::vector<int> > edge_var(n + 1, std::vector<int>(n + 1, -1));
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v)
            if (adj[u][v]) edge_var[u][v] = num_vars++;

    // One variable per connected component of G - N[x] (NS2 quotient):
    // comp_var[x][u] is the variable of beta(x, u) for u outside N[x], u != x.
    std::vector<std::vector<int> > comp_var(n + 1, std::vector<int>(n + 1, -1));
    std::vector<int> stack_buf;
    stack_buf.reserve(n);
    for (int x = 1; x <= n; ++x) {
        for (int s = 1; s <= n; ++s) {
            if (s == x || adj[x][s] || comp_var[x][s] != -1) continue;
            int id = num_vars++;
            comp_var[x][s] = id;
            stack_buf.push_back(s);
            while (!stack_buf.empty()) {
                int u = stack_buf.back();
                stack_buf.pop_back();
                for (size_t j = 0; j < g.adj[u].size(); ++j) {
                    int v = g.adj[u][j];
                    if (v == x || adj[x][v] || comp_var[x][v] != -1) continue;
                    comp_var[x][v] = id;
                    stack_buf.push_back(v);
                }
            }
        }
    }

    NajiGf2System system(num_vars, memory_limit_words);

    // NS3: for every non-adjacent pair {v, w} and every common neighbor x,
    //   beta(v,w) + beta(w,v) + beta(x,v) + beta(x,w) = 1.
    // Substituting beta(x,v) = X_xv + [x > v] moves [x > v] + [x > w] to the
    // right-hand side.
    int vars[4];
    for (int v = 1; v <= n; ++v) {
        for (int w = v + 1; w <= n; ++w) {
            if (adj[v][w]) continue;
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int x = g.adj[v][j];
                if (!adj[x][w]) continue;
                vars[0] = comp_var[v][w];
                vars[1] = comp_var[w][v];
                vars[2] = x < v ? edge_var[x][v] : edge_var[v][x];
                vars[3] = x < w ? edge_var[x][w] : edge_var[w][x];
                int rhs = 1 ^ (x > v ? 1 : 0) ^ (x > w ? 1 : 0);
                if (!system.add_equation(vars, 4, rhs)) return res;
            }
        }
    }

    res.is_circle = true;
    return res;
}

/**
 * @brief Circle graph recognition via Naji's linear system
 *
 * Contracts twin classes first, then solves the Naji system of the contracted
 * graph under the given basis memory limit (0 = unlimited).
 *
 * The contraction preserves circle membership in both directions: a false twin
 * duplicates a chord in parallel, a true twin duplicates it crossing, and
 * induced subgraphs of circle graphs are circle graphs. It matters because the
 * raw Naji system has n(n-1) variables -- the star K_{1,n-1} alone contributes
 * (n-1)^2 of them and contracts to a single edge.
 * @throws std::runtime_error if the limit is exceeded (result unknown)
 */
inline CircleResult check_circle_naji(const Graph& g,
    std::size_t memory_limit_words = circle_naji_default_memory_limit_words) {
    return check_circle_naji_system(
        graph_recognition::contract_twins(g).quotient, memory_limit_words);
}

/**
 * @brief Internal state for DOW backtracking
 */
struct DowState {
    int n;
    std::vector<std::vector<char>> adj; /**< adjacency matrix (1-indexed) */
    std::vector<int> word;              /**< DOW (0-indexed positions) */
    std::vector<int> first_pos;         /**< first occurrence position of each vertex (-1: unplaced) */
    std::vector<int> second_pos;        /**< second occurrence position of each vertex (-1: unplaced) */
    std::vector<int> placement;         /**< 0: unplaced, 1: first placed, 2: completed */
    std::vector<int> order;             /**< placement order (descending by degree) */
    bool found;
    unsigned long long steps;           /**< dow_dfs invocations so far */
    unsigned long long step_limit;      /**< abort threshold; 0 = unlimited */
    bool aborted;                       /**< set when step_limit was exhausted */

    explicit DowState(int n_)
        : n(n_), adj(n_ + 1, std::vector<char>(n_ + 1, 0)),
          word(2 * n_, -1), first_pos(n_ + 1, -1),
          second_pos(n_ + 1, -1), placement(n_ + 1, 0),
          order(), found(false), steps(0), step_limit(0), aborted(false) {}
};

/**
 * @brief Constraint check when placing vertex v for the second time
 *
 * When v's occurrence positions are [f, p], checks whether the fully placed vertex u's
 * occurrence positions [a, b] interleave, and verifies consistency with adj[u][v].
 */
inline bool check_second_placement(const DowState& state, int v, int pos) {
    int f = state.first_pos[v];
    int lo = f < pos ? f : pos;
    int hi = f < pos ? pos : f;
    for (int u = 1; u <= state.n; ++u) {
        if (u == v || state.placement[u] != 2) continue;
        int a = state.first_pos[u];
        int b = state.second_pos[u];
        if (a > b) { int tmp = a; a = b; b = tmp; }
        // u and v interleave iff exactly one of {a, b} lies in (lo, hi)
        bool a_in = (lo < a && a < hi);
        bool b_in = (lo < b && b < hi);
        bool interleave = (a_in != b_in);
        bool adjacent = (state.adj[u][v] != 0);
        if (interleave != adjacent) return false;
    }
    return true;
}

/**
 * @brief Backtracking DFS for DOW construction
 */
inline void dow_dfs(DowState& state, int pos) {
    if (state.found || state.aborted) return;
    if (state.step_limit != 0 && ++state.steps >= state.step_limit) {
        state.aborted = true;
        return;
    }
    if (pos == 2 * state.n) {
        state.found = true;
        return;
    }

    // (A) Place the second occurrence of half-placed vertices (stronger constraints -> try first)
    for (size_t idx = 0; idx < state.order.size(); ++idx) {
        int v = state.order[idx];
        if (state.placement[v] != 1) continue;
        if (!check_second_placement(state, v, pos)) continue;
        state.word[pos] = v;
        state.second_pos[v] = pos;
        state.placement[v] = 2;
        dow_dfs(state, pos + 1);
        if (state.found) return;
        state.placement[v] = 1;
        state.second_pos[v] = -1;
        state.word[pos] = -1;
        if (state.aborted) return;
    }

    // (B) Place the first occurrence of unplaced vertices
    for (size_t idx = 0; idx < state.order.size(); ++idx) {
        int v = state.order[idx];
        if (state.placement[v] != 0) continue;
        state.word[pos] = v;
        state.first_pos[v] = pos;
        state.placement[v] = 1;
        dow_dfs(state, pos + 1);
        if (state.found) return;
        state.placement[v] = 0;
        state.first_pos[v] = -1;
        state.word[pos] = -1;
        if (state.aborted) return;
    }
}

/**
 * @brief Circle graph recognition via DOW backtracking
 * @throws std::runtime_error if the step budget is exhausted before the
 *         search decides (result unknown; 0 = unlimited)
 */
inline CircleResult check_circle_dow(const Graph& g,
    unsigned long long step_budget = circle_dow_default_budget) {
    CircleResult res;
    res.is_circle = false;
    int n = g.n;

    if (n == 0) {
        res.is_circle = true;
        return res;
    }

    DowState state(n);
    state.step_limit = step_budget;
    for (int u = 1; u <= n; ++u)
        for (size_t j = 0; j < g.adj[u].size(); ++j) {
            int v = g.adj[u][j];
            state.adj[u][v] = 1;
        }

    // Determine placement order by descending degree (higher degree -> more constraints -> better pruning).
    // Ties are broken by ascending vertex id via the std::pair<int,int> ordering used below.
    std::vector<std::pair<int, int>> deg_v;
    for (int v = 1; v <= n; ++v) {
        deg_v.push_back(std::make_pair(-(int)g.adj[v].size(), v));
    }
    std::sort(deg_v.begin(), deg_v.end());
    for (size_t i = 0; i < deg_v.size(); ++i) {
        state.order.push_back(deg_v[i].second);
    }

    // Fix the first occurrence of vertex order[0] at position 0 (break circular symmetry)
    int first_v = state.order[0];
    state.word[0] = first_v;
    state.first_pos[first_v] = 0;
    state.placement[first_v] = 1;

    dow_dfs(state, 1);

    if (state.aborted)
        throw std::runtime_error(
            "circle: DOW backtracking step budget exceeded (the search is "
            "exponential; practical up to roughly n = 9); result unknown");
    if (state.found) {
        res.is_circle = true;
        res.dow = state.word;
    }
    return res;
}

}  // namespace detail_circle

/**
 * @brief Circle graph recognition
 * @param g Input graph (1-indexed)
 * @param algo Algorithm selection (default: Naji's polynomial-time linear system)
 * @return CircleResult (dow is filled only by DOW_BACKTRACKING)
 */
inline CircleResult check_circle(const Graph& g,
    CircleAlgorithm algo = CircleAlgorithm::NAJI_SYSTEM) {
    if (algo == CircleAlgorithm::DOW_BACKTRACKING) {
        return detail_circle::check_circle_dow(g);
    }
    return detail_circle::check_circle_naji(g);
}

}  // namespace graph_recognition

#endif
