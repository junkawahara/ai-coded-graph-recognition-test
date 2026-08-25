#ifndef GRAPH_RECOGNITION_FORBIDDEN_SUBGRAPH_H
#define GRAPH_RECOGNITION_FORBIDDEN_SUBGRAPH_H

/**
 * @file forbidden_subgraph.h
 * @brief Shared vocabulary for NO-side certificates (obstructions)
 *
 * A recognizer that answers NO can return the concrete structure that rules the
 * graph out of the class: a hole, an asteroidal triple, a forbidden induced
 * subgraph, a minor model. This header defines the single type all recognizers
 * use for that, so composed classes (interval = chordal + AT-free, ptolemaic =
 * chordal + gem-free, ...) can propagate a sub-certificate unchanged.
 *
 * Conventions:
 * - `vertices` holds 1-indexed vertex numbers of the input graph. It is a plain
 *   vertex *list*, not a vertex-indexed array, so the n+1 zero-fill rule that
 *   applies to fields like `side` or `parent` does not apply here.
 * - `in_complement` says the pattern is induced in the complement of the input
 *   graph rather than in the graph itself. This keeps antiholes, co-chordal and
 *   the other co-* classes from needing kinds of their own.
 * - An `Obstruction` in a `<Camel>Result` is valid only when `is_<type>` is
 *   false. Recognizers that answer YES, and algorithm variants that cannot
 *   produce a witness, leave `kind == ObstructionKind::NONE`.
 */

#include <vector>

namespace graph_recognition {

/**
 * @brief The structure a NO certificate exhibits
 *
 * The meaning of Obstruction::vertices is fixed per kind; the canonical vertex
 * order for each fixed pattern is documented on its enumerator.
 */
enum class ObstructionKind {
    NONE, /**< No witness recorded (YES answer, or a variant that produces none) */

    // --- Cycle-shaped: `vertices` lists the cycle in cyclic order. ---
    HOLE,      /**< Chordless cycle of length >= 4 (callers needing >= 5 say so) */
    ODD_CYCLE, /**< Simple odd cycle of length >= 3; chords are allowed */
    ODD_HOLE,  /**< Chordless odd cycle of length >= 5 */
    EVEN_HOLE, /**< Chordless even cycle of length >= 4 */
    ODD_CYCLE_LE1_CHORD, /**< Odd cycle of length >= 5 with at most one chord */
    TWO_CYCLES_SHARING_EDGE, /**< Two distinct cycles sharing an edge;
                                  `vertex_sets` = {C1, C2}, `vertices` = the shared edge */

    // --- Path-shaped. ---
    INDUCED_PATH_WRONG_PARITY, /**< Two induced u-v paths of different length
                                    parity; `vertices` = {u, v} and `vertex_sets`
                                    = the two paths, each listed from u to v */
    NON_SHORTEST_INDUCED_PATH, /**< Induced u-v path longer than the distance
                                    d(u,v); `vertices` lists it from u to v */

    // --- Fixed small patterns. ---
    TRIANGLE, /**< K3: {a, b, c}, all pairs adjacent */
    P3,       /**< Induced path a-b-c */
    P4,       /**< Induced path a-b-c-d */
    P5,       /**< Induced path a-b-c-d-e */
    C4,       /**< Induced 4-cycle a-b-c-d-a */
    C5,       /**< Induced 5-cycle a-b-c-d-e-a */
    TWO_K2,   /**< 2K2: {a, b, c, d} with edges ab, cd only */
    CLAW,     /**< K_{1,3}: {center, leaf, leaf, leaf} */
    DIAMOND,  /**< K4 minus an edge: {deg3, deg3, deg2, deg2}; the two deg2
                   vertices are the non-adjacent pair */
    BULL,     /**< Triangle {a, b, c} plus pendants: {a, b, c, x, y} with x-a, y-b */
    GEM,      /**< Induced P4 a-b-c-d plus a vertex adjacent to all four:
                   {a, b, c, d, apex} */

    // --- Structural. ---
    ASTEROIDAL_TRIPLE, /**< {a, b, c}: pairwise non-adjacent, and each pair is
                            connected by a path avoiding the closed neighborhood
                            of the third */
    CUT_VERTEX,        /**< {v}: removing v raises the component count */
    DISCONNECTED_PAIR, /**< {u, v}: u and v lie in different components */
    FORCING_CYCLE,     /**< Gamma-forcing chain certifying non-comparability.
                            `vertices` is a flattened arc list u1,v1,u2,v2,...,uk,vk
                            where each arc Gamma-forces the next and the last arc
                            is the reverse of the first */
    K5_MINOR,          /**< `vertex_sets` = 5 branch sets, pairwise adjacent */
    K33_MINOR          /**< `vertex_sets` = 6 branch sets; the first three and the
                            last three form the bipartition */
};

/**
 * @brief A NO-side certificate
 *
 * Valid only when the accompanying `is_<type>` is false. See the file comment
 * for the meaning of each field.
 */
struct Obstruction {
    ObstructionKind kind = ObstructionKind::NONE; /**< Which structure is exhibited */
    bool in_complement = false; /**< true if the pattern is induced in the complement */
    std::vector<int> vertices;  /**< 1-indexed vertex list; meaning depends on kind */
    std::vector<std::vector<int>> vertex_sets; /**< Branch sets, cycles or paths; used
                                                    only by the *_MINOR,
                                                    TWO_CYCLES_SHARING_EDGE and
                                                    INDUCED_PATH_WRONG_PARITY kinds,
                                                    empty otherwise */

    /** @brief True if a witness is present */
    bool has_witness() const { return kind != ObstructionKind::NONE; }
};

/**
 * @brief Builds an obstruction from a kind and a vertex list
 * @param kind Structure exhibited
 * @param vertices 1-indexed vertex list in the canonical order for `kind`
 * @param in_complement true if the pattern is induced in the complement
 * @return The obstruction
 */
inline Obstruction make_obstruction(ObstructionKind kind, const std::vector<int>& vertices,
                                    bool in_complement = false) {
    Obstruction o;
    o.kind = kind;
    o.vertices = vertices;
    o.in_complement = in_complement;
    return o;
}

/**
 * @brief Name of an obstruction kind, for CLI output and diagnostics
 * @param kind Obstruction kind
 * @return Stable lowercase identifier (e.g. "hole", "asteroidal_triple")
 */
inline const char* obstruction_kind_name(ObstructionKind kind) {
    switch (kind) {
        case ObstructionKind::NONE: return "none";
        case ObstructionKind::HOLE: return "hole";
        case ObstructionKind::ODD_CYCLE: return "odd_cycle";
        case ObstructionKind::ODD_HOLE: return "odd_hole";
        case ObstructionKind::EVEN_HOLE: return "even_hole";
        case ObstructionKind::ODD_CYCLE_LE1_CHORD: return "odd_cycle_le1_chord";
        case ObstructionKind::TWO_CYCLES_SHARING_EDGE: return "two_cycles_sharing_edge";
        case ObstructionKind::INDUCED_PATH_WRONG_PARITY: return "induced_path_wrong_parity";
        case ObstructionKind::NON_SHORTEST_INDUCED_PATH: return "non_shortest_induced_path";
        case ObstructionKind::TRIANGLE: return "triangle";
        case ObstructionKind::P3: return "p3";
        case ObstructionKind::P4: return "p4";
        case ObstructionKind::P5: return "p5";
        case ObstructionKind::C4: return "c4";
        case ObstructionKind::C5: return "c5";
        case ObstructionKind::TWO_K2: return "2k2";
        case ObstructionKind::CLAW: return "claw";
        case ObstructionKind::DIAMOND: return "diamond";
        case ObstructionKind::BULL: return "bull";
        case ObstructionKind::GEM: return "gem";
        case ObstructionKind::ASTEROIDAL_TRIPLE: return "asteroidal_triple";
        case ObstructionKind::CUT_VERTEX: return "cut_vertex";
        case ObstructionKind::DISCONNECTED_PAIR: return "disconnected_pair";
        case ObstructionKind::FORCING_CYCLE: return "forcing_cycle";
        case ObstructionKind::K5_MINOR: return "k5_minor";
        case ObstructionKind::K33_MINOR: return "k33_minor";
    }
    return "none";
}

} // namespace graph_recognition

#endif
