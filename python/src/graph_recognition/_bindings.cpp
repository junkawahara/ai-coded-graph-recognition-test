#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "util/graph.h"

// --- Decomposition headers ---
#include "decompositions/block_cut_tree.h"
#include "decompositions/clique.h"
#include "decompositions/components.h"
#include "decompositions/elimination_orderings.h"
#include "decompositions/md_tree.h"
#include "decompositions/modular_decomposition.h"
#include "decompositions/planar_embedding.h"
#include "decompositions/pq_tree.h"
#include "decompositions/split_decomposition.h"
#include "decompositions/spqr_tree.h"
#include "decompositions/transitive_orientation.h"
#include "decompositions/tree_decomposition.h"
#include "decompositions/tree_layout.h"
#include "decompositions/twins.h"

// --- Recognition headers ---
#include "recognizers/at_free.h"
#include "recognizers/biconvex_bipartite.h"
#include "recognizers/bipartite.h"
#include "recognizers/bipartite_permutation.h"
#include "recognizers/block.h"
#include "recognizers/cactus.h"
#include "recognizers/chain.h"
#include "recognizers/chordal.h"
#include "recognizers/chordal_bipartite.h"
#include "recognizers/circular_arc.h"
#include "recognizers/claw_free.h"
#include "recognizers/co_chordal.h"
#include "recognizers/co_comparability.h"
#include "recognizers/co_interval.h"
#include "recognizers/cochain.h"
#include "recognizers/cograph.h"
#include "recognizers/comparability.h"
#include "recognizers/convex_bipartite.h"
#include "recognizers/diamond_free.h"
#include "recognizers/distance_hereditary.h"
#include "recognizers/interval.h"
#include "recognizers/line_graph.h"
#include "recognizers/outer_planar.h"
#include "recognizers/perfect.h"
#include "recognizers/permutation.h"
#include "recognizers/planar.h"
#include "recognizers/proper_interval.h"
#include "recognizers/ptolemaic.h"
#include "recognizers/quasi_threshold.h"
#include "recognizers/series_parallel.h"
#include "recognizers/split.h"
#include "recognizers/strongly_chordal.h"
#include "recognizers/three_leaf_power.h"
#include "recognizers/threshold.h"
#include "recognizers/trapezoid.h"
#include "recognizers/trivially_perfect.h"
#include "recognizers/unit_interval.h"
#include "recognizers/weakly_chordal.h"
#include "recognizers/apex.h"
#include "recognizers/apollonian.h"
#include "recognizers/biconnected.h"
#include "recognizers/bull_free.h"
#include "recognizers/caterpillar.h"
#include "recognizers/circle.h"
#include "recognizers/cluster.h"
#include "recognizers/cubic.h"
#include "recognizers/cubic_planar.h"
#include "recognizers/digraph.h"
#include "recognizers/eulerian.h"
#include "recognizers/even_hole_free.h"
#include "recognizers/five_leaf_power.h"
#include "recognizers/forest.h"
#include "recognizers/four_leaf_power.h"
#include "recognizers/fullerene.h"
#include "recognizers/gem_free.h"
#include "recognizers/halin.h"
#include "recognizers/kregular.h"
#include "recognizers/ktree.h"
#include "recognizers/laman.h"
#include "recognizers/maximal_outer_planar.h"
#include "recognizers/maximal_planar.h"
#include "recognizers/meyniel.h"
#include "recognizers/odd_hole_free.h"
#include "recognizers/p5_free.h"
#include "recognizers/parity.h"
#include "recognizers/polyhedral.h"
#include "recognizers/poset.h"
#include "recognizers/proper_chordal.h"
#include "recognizers/proper_circular_arc.h"
#include "recognizers/self_complementary.h"
#include "recognizers/simple_quadrangulation.h"
#include "recognizers/snark.h"
#include "recognizers/strongly_regular.h"
#include "recognizers/tournament.h"
#include "recognizers/tree.h"
#include "recognizers/triangle_free.h"
#include "recognizers/triconnected.h"
#include "recognizers/unicyclic.h"

// --- Enumeration headers ---
#include "enumerators/apollonian_unlabeled_enum.h"
#include "enumerators/at_free_labeled_enum.h"
#include "enumerators/biconnected_unlabeled_enum.h"
#include "enumerators/biconvex_bipartite_labeled_enum.h"
#include "enumerators/bipartite_labeled_enum.h"
#include "enumerators/bipartite_unlabeled_enum.h"
#include "enumerators/bipartite_permutation_labeled_enum.h"
#include "enumerators/bipartite_permutation_unlabeled_enum.h"
#include "enumerators/block_labeled_enum.h"
#include "enumerators/cactus_labeled_enum.h"
#include "enumerators/cactus_unlabeled_enum.h"
#include "enumerators/chain_unlabeled_enum.h"
#include "enumerators/chordal_bipartite_labeled_enum.h"
#include "enumerators/chordal_bipartite_induced_subgraph_enum.h"
#include "enumerators/chordal_labeled_enum.h"
#include "enumerators/chordal_subgraph_enum.h"
#include "enumerators/chordal_unlabeled_enum.h"
#include "enumerators/circle_unlabeled_enum.h"
#include "enumerators/circular_arc_labeled_enum.h"
#include "enumerators/claw_free_labeled_enum.h"
#include "enumerators/co_chordal_labeled_enum.h"
#include "enumerators/co_chordal_unlabeled_enum.h"
#include "enumerators/co_comparability_labeled_enum.h"
#include "enumerators/co_comparability_unlabeled_enum.h"
#include "enumerators/co_interval_labeled_enum.h"
#include "enumerators/co_interval_unlabeled_enum.h"
#include "enumerators/cochain_unlabeled_enum.h"
#include "enumerators/cluster_unlabeled_enum.h"
#include "enumerators/cograph_labeled_enum.h"
#include "enumerators/cograph_unlabeled_enum.h"
#include "enumerators/comparability_labeled_enum.h"
#include "enumerators/comparability_unlabeled_enum.h"
#include "enumerators/convex_bipartite_labeled_enum.h"
#include "enumerators/cubic_planar_unlabeled_enum.h"
#include "enumerators/cubic_unlabeled_enum.h"
#include "enumerators/diamond_free_labeled_enum.h"
#include "enumerators/distance_hereditary_labeled_enum.h"
#include "enumerators/distance_hereditary_unlabeled_enum.h"
#include "enumerators/eulerian_unlabeled_enum.h"
#include "enumerators/interval_labeled_enum.h"
#include "enumerators/interval_unlabeled_enum.h"
#include "enumerators/line_graph_labeled_enum.h"
#include "enumerators/maximal_outer_planar_unlabeled_enum.h"
#include "enumerators/maximal_planar_unlabeled_enum.h"
#include "enumerators/outer_planar_labeled_enum.h"
#include "enumerators/outer_planar_unlabeled_enum.h"
#include "enumerators/perfect_labeled_enum.h"
#include "enumerators/permutation_labeled_enum.h"
#include "enumerators/permutation_unlabeled_enum.h"
#include "enumerators/planar_labeled_enum.h"
#include "enumerators/planar_unlabeled_enum.h"
#include "enumerators/polyhedral_unlabeled_enum.h"
#include "enumerators/proper_interval_labeled_enum.h"
#include "enumerators/proper_interval_unlabeled_enum.h"
#include "enumerators/ptolemaic_labeled_enum.h"
#include "enumerators/ptolemaic_unlabeled_enum.h"
#include "enumerators/self_complementary_unlabeled_enum.h"
#include "enumerators/series_parallel_labeled_enum.h"
#include "enumerators/series_parallel_unlabeled_enum.h"
#include "enumerators/split_labeled_enum.h"
#include "enumerators/split_unlabeled_enum.h"
#include "enumerators/strongly_chordal_labeled_enum.h"
#include "enumerators/three_leaf_power_labeled_enum.h"
#include "enumerators/three_leaf_power_unlabeled_enum.h"
#include "enumerators/threshold_unlabeled_enum.h"
#include "enumerators/trapezoid_labeled_enum.h"
#include "enumerators/triangle_free_unlabeled_enum.h"
#include "enumerators/trivially_perfect_labeled_enum.h"
#include "enumerators/trivially_perfect_unlabeled_enum.h"
#include "enumerators/weakly_chordal_labeled_enum.h"

namespace py = pybind11;
using namespace graph_recognition;

static Graph make_graph(int n, const std::vector<std::pair<int, int>>& edges) {
    return Graph(n, edges);
}

// Helper type for enumeration results
typedef std::vector<std::pair<int, std::vector<std::pair<int, int>>>> EnumResultPy;

// ============================================================
// NO-side certificates (obstructions)
// ============================================================

// The witness is a property of the graph, not of the algorithm variant that
// found the NO, so this dispatch always uses the route that produces one:
// the recognizer where it fills the field itself, the build_*_obstruction()
// builder where extraction costs more than recognition.
static py::object obstruction_to_dict(const Obstruction& o) {
    if (!o.has_witness()) return py::none();
    py::dict d;
    d["kind"] = std::string(obstruction_kind_name(o.kind));
    d["in_complement"] = o.in_complement;
    d["vertices"] = o.vertices;
    d["vertex_sets"] = o.vertex_sets;
    return d;
}

static Obstruction obstruction_for(const std::string& type_name, const Graph& g) {
    if (type_name == "at_free") return check_at_free(g).obstruction;
    if (type_name == "biconnected") return check_biconnected(g).obstruction;
    if (type_name == "bipartite") return check_bipartite(g).obstruction;
    if (type_name == "block") return build_block_obstruction(g);
    if (type_name == "bull_free") return check_bull_free(g).obstruction;
    if (type_name == "cactus") return build_cactus_obstruction(g);
    if (type_name == "chain") return check_chain(g).obstruction;
    if (type_name == "chordal") return check_chordal(g).obstruction;
    if (type_name == "chordal_bipartite") return check_chordal_bipartite(g).obstruction;
    if (type_name == "claw_free") return check_claw_free(g).obstruction;
    if (type_name == "cluster") return check_cluster(g).obstruction;
    if (type_name == "co_chordal") return check_co_chordal(g).obstruction;
    if (type_name == "co_comparability") return build_co_comparability_obstruction(g);
    if (type_name == "co_interval") return check_co_interval(g).obstruction;
    if (type_name == "cochain") return build_cochain_obstruction(g);
    if (type_name == "cograph") return check_cograph(g).obstruction;
    if (type_name == "comparability") return build_comparability_obstruction(g);
    if (type_name == "diamond_free") return check_diamond_free(g).obstruction;
    if (type_name == "distance_hereditary") return build_distance_hereditary_obstruction(g);
    if (type_name == "even_hole_free") return check_even_hole_free(g).obstruction;
    if (type_name == "gem_free") return check_gem_free(g).obstruction;
    if (type_name == "interval") return build_interval_obstruction(g);
    if (type_name == "meyniel") return check_meyniel(g).obstruction;
    if (type_name == "odd_hole_free") return check_odd_hole_free(g).obstruction;
    if (type_name == "p5_free") return check_p5_free(g).obstruction;
    if (type_name == "parity") return check_parity(g).obstruction;
    if (type_name == "perfect") return check_perfect(g).obstruction;
    if (type_name == "permutation") return build_permutation_obstruction(g);
    if (type_name == "planar") return build_planar_obstruction(g);
    if (type_name == "proper_interval") return build_proper_interval_obstruction(g);
    if (type_name == "ptolemaic") return check_ptolemaic(g).obstruction;
    if (type_name == "quasi_threshold") return check_quasi_threshold(g).obstruction;
    if (type_name == "split") return build_split_obstruction(g);
    if (type_name == "threshold") return build_threshold_obstruction(g);
    if (type_name == "triangle_free") return check_triangle_free(g).obstruction;
    if (type_name == "trivially_perfect") return check_trivially_perfect(g).obstruction;
    if (type_name == "unit_interval") return check_unit_interval(g).obstruction;
    if (type_name == "weakly_chordal") return check_weakly_chordal(g).obstruction;
    return Obstruction();
}

static py::object obstruction_py(const std::string& type_name, int n,
                                 const std::vector<std::pair<int, int>>& edges) {
    Graph g = make_graph(n, edges);
    return obstruction_to_dict(obstruction_for(type_name, g));
}

static std::vector<std::string> certified_types_py() {
    static const char* kNames[] = {
        "at_free", "biconnected", "bipartite", "block", "bull_free", "cactus",
        "chain", "chordal", "chordal_bipartite", "claw_free", "cluster",
        "co_chordal", "co_comparability", "co_interval", "cochain", "cograph",
        "comparability", "diamond_free", "distance_hereditary", "even_hole_free",
        "gem_free", "interval", "meyniel", "odd_hole_free", "p5_free", "parity",
        "perfect", "permutation", "planar", "proper_interval", "ptolemaic",
        "quasi_threshold", "split", "threshold", "triangle_free",
        "trivially_perfect", "unit_interval", "weakly_chordal"};
    return std::vector<std::string>(kNames,
                                    kNames + sizeof(kNames) / sizeof(kNames[0]));
}

// ============================================================
// Recognition functions
// ============================================================

// --- apollonian ---
static bool check_apollonian_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ApollonianAlgorithm a = ApollonianAlgorithm::MAXIMAL_PLANAR_CHORDAL;
    if (!algo.empty()) {
        if (algo == "maximal_planar_chordal") a = ApollonianAlgorithm::MAXIMAL_PLANAR_CHORDAL;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for apollonian. Valid: 'maximal_planar_chordal'");
    }
    return check_apollonian(g, a).is_apollonian;
}

// --- at_free ---
static bool check_at_free_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ATFreeAlgorithm a = ATFreeAlgorithm::BRUTE_FORCE;
    if (!algo.empty()) {
        if (algo == "brute_force") a = ATFreeAlgorithm::BRUTE_FORCE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for at_free. Valid: 'brute_force'");
    }
    return check_at_free(g, a).is_at_free;
}

// --- biconvex_bipartite ---
static bool check_biconvex_bipartite_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    BiconvexBipartiteAlgorithm a = BiconvexBipartiteAlgorithm::C1P;
    if (!algo.empty()) {
        if (algo == "brute_force") a = BiconvexBipartiteAlgorithm::BRUTE_FORCE;
        else if (algo == "c1p") a = BiconvexBipartiteAlgorithm::C1P;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for biconvex_bipartite. Valid: 'brute_force', 'c1p'");
    }
    return check_biconvex_bipartite(g, a).is_biconvex_bipartite;
}

// --- bipartite ---
static bool check_bipartite_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    BipartiteAlgorithm a = BipartiteAlgorithm::BFS;
    if (!algo.empty()) {
        if (algo == "bfs") a = BipartiteAlgorithm::BFS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for bipartite. Valid: 'bfs'");
    }
    return check_bipartite(g, a).is_bipartite;
}

// --- bipartite_permutation ---
static bool check_bipartite_permutation_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    BipartitePermutationAlgorithm a = BipartitePermutationAlgorithm::BIPARTITE_AND_PERMUTATION;
    if (!algo.empty()) {
        if (algo == "bipartite_and_permutation") a = BipartitePermutationAlgorithm::BIPARTITE_AND_PERMUTATION;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for bipartite_permutation. Valid: 'bipartite_and_permutation'");
    }
    return check_bipartite_permutation(g, a).is_bipartite_permutation;
}

// --- block ---
static bool check_block_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    BlockAlgorithm a = BlockAlgorithm::DFS;
    if (!algo.empty()) {
        if (algo == "dfs") a = BlockAlgorithm::DFS;
        else if (algo == "chordal_diamond_free") a = BlockAlgorithm::CHORDAL_DIAMOND_FREE;
        else if (algo == "block_cut_tree") a = BlockAlgorithm::BLOCK_CUT_TREE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for block. Valid: 'dfs', 'chordal_diamond_free', 'block_cut_tree'");
    }
    return check_block(g, a).is_block;
}

// --- cactus ---
static bool check_cactus_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CactusAlgorithm a = CactusAlgorithm::DFS;
    if (!algo.empty()) {
        if (algo == "dfs") a = CactusAlgorithm::DFS;
        else if (algo == "block_cut_tree") a = CactusAlgorithm::BLOCK_CUT_TREE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for cactus. Valid: 'dfs', 'block_cut_tree'");
    }
    return check_cactus(g, a).is_cactus;
}

// --- chain ---
static bool check_chain_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ChainAlgorithm a = ChainAlgorithm::DEGREE_SORT;
    if (!algo.empty()) {
        if (algo == "neighborhood_inclusion") a = ChainAlgorithm::NEIGHBORHOOD_INCLUSION;
        else if (algo == "degree_sort") a = ChainAlgorithm::DEGREE_SORT;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for chain. Valid: 'neighborhood_inclusion', 'degree_sort'");
    }
    return check_chain(g, a).is_chain;
}

// --- chordal ---
static bool check_chordal_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ChordalAlgorithm a = ChordalAlgorithm::BUCKET_MCS_PEO;
    if (!algo.empty()) {
        if (algo == "mcs_peo") a = ChordalAlgorithm::MCS_PEO;
        else if (algo == "bucket_mcs_peo") a = ChordalAlgorithm::BUCKET_MCS_PEO;
        else if (algo == "lexbfs_peo") a = ChordalAlgorithm::LEXBFS_PEO;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for chordal. Valid: 'mcs_peo', 'bucket_mcs_peo', 'lexbfs_peo'");
    }
    return check_chordal(g, a).is_chordal;
}

// --- chordal_bipartite ---
static bool check_chordal_bipartite_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ChordalBipartiteAlgorithm a = ChordalBipartiteAlgorithm::FAST_BISIMPLICIAL;
    if (!algo.empty()) {
        if (algo == "cycle_check") a = ChordalBipartiteAlgorithm::CYCLE_CHECK;
        else if (algo == "bisimplicial") a = ChordalBipartiteAlgorithm::BISIMPLICIAL;
        else if (algo == "fast_bisimplicial") a = ChordalBipartiteAlgorithm::FAST_BISIMPLICIAL;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for chordal_bipartite. Valid: 'cycle_check', 'bisimplicial', 'fast_bisimplicial'");
    }
    return check_chordal_bipartite(g, a).is_chordal_bipartite;
}

// --- circular_arc ---
static bool check_circular_arc_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CircularArcAlgorithm a = CircularArcAlgorithm::MCCONNELL;
    if (!algo.empty()) {
        if (algo == "mcconnell") a = CircularArcAlgorithm::MCCONNELL;
        else if (algo == "backtracking") a = CircularArcAlgorithm::BACKTRACKING;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for circular_arc. Valid: 'mcconnell', 'backtracking'");
    }
    return check_circular_arc(g, a).is_circular_arc;
}

// --- claw_free ---
static bool check_claw_free_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ClawFreeAlgorithm a = ClawFreeAlgorithm::EDGE_COUNT;
    if (!algo.empty()) {
        if (algo == "triple_loop") a = ClawFreeAlgorithm::TRIPLE_LOOP;
        else if (algo == "edge_count") a = ClawFreeAlgorithm::EDGE_COUNT;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for claw_free. Valid: 'triple_loop', 'edge_count'");
    }
    return check_claw_free(g, a).is_claw_free;
}

// --- co_chordal ---
static bool check_co_chordal_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CoChordalAlgorithm a = CoChordalAlgorithm::COMPLEMENT;
    if (!algo.empty()) {
        if (algo == "complement") a = CoChordalAlgorithm::COMPLEMENT;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for co_chordal. Valid: 'complement'");
    }
    return check_co_chordal(g, a).is_co_chordal;
}

// --- co_comparability ---
static bool check_co_comparability_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CoComparabilityAlgorithm a = CoComparabilityAlgorithm::COMPLEMENT;
    if (!algo.empty()) {
        if (algo == "complement") a = CoComparabilityAlgorithm::COMPLEMENT;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for co_comparability. Valid: 'complement'");
    }
    return check_co_comparability(g, a).is_co_comparability;
}

// --- co_interval ---
static bool check_co_interval_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CoIntervalAlgorithm a = CoIntervalAlgorithm::COMPLEMENT;
    if (!algo.empty()) {
        if (algo == "complement") a = CoIntervalAlgorithm::COMPLEMENT;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for co_interval. Valid: 'complement'");
    }
    return check_co_interval(g, a).is_co_interval;
}

// --- cochain ---
static bool check_cochain_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CochainAlgorithm a = CochainAlgorithm::DIRECT;
    if (!algo.empty()) {
        if (algo == "complement") a = CochainAlgorithm::COMPLEMENT;
        else if (algo == "direct") a = CochainAlgorithm::DIRECT;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for cochain. Valid: 'complement', 'direct'");
    }
    return check_cochain(g, a).is_cochain;
}

// --- cograph ---
static bool check_cograph_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CographAlgorithm a = CographAlgorithm::PARTITION_REFINEMENT;
    if (!algo.empty()) {
        if (algo == "cotree") a = CographAlgorithm::COTREE;
        else if (algo == "partition_refinement") a = CographAlgorithm::PARTITION_REFINEMENT;
        else if (algo == "modular") a = CographAlgorithm::MODULAR;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for cograph. Valid: 'cotree', 'partition_refinement', 'modular'");
    }
    return check_cograph(g, a).is_cograph;
}

// --- comparability ---
static bool check_comparability_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ComparabilityAlgorithm a = ComparabilityAlgorithm::TRANSITIVE_ORIENTATION;
    if (!algo.empty()) {
        if (algo == "transitive_orientation") a = ComparabilityAlgorithm::TRANSITIVE_ORIENTATION;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for comparability. Valid: 'transitive_orientation'");
    }
    return check_comparability(g, a).is_comparability;
}

// --- convex_bipartite ---
static bool check_convex_bipartite_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ConvexBipartiteAlgorithm a = ConvexBipartiteAlgorithm::C1P;
    if (!algo.empty()) {
        if (algo == "brute_force") a = ConvexBipartiteAlgorithm::BRUTE_FORCE;
        else if (algo == "c1p") a = ConvexBipartiteAlgorithm::C1P;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for convex_bipartite. Valid: 'brute_force', 'c1p'");
    }
    return check_convex_bipartite(g, a).is_convex_bipartite;
}

// --- diamond_free ---
static bool check_diamond_free_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    DiamondFreeAlgorithm a = DiamondFreeAlgorithm::EDGE_PAIR;
    if (!algo.empty()) {
        if (algo == "brute") a = DiamondFreeAlgorithm::BRUTE;
        else if (algo == "edge_pair") a = DiamondFreeAlgorithm::EDGE_PAIR;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for diamond_free. Valid: 'brute', 'edge_pair'");
    }
    return check_diamond_free(g, a).is_diamond_free;
}

// --- distance_hereditary ---
static bool check_distance_hereditary_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    DistanceHereditaryAlgorithm a = DistanceHereditaryAlgorithm::HASH_TWINS;
    if (!algo.empty()) {
        if (algo == "hashmap_twins") a = DistanceHereditaryAlgorithm::HASHMAP_TWINS;
        else if (algo == "sorted_twins") a = DistanceHereditaryAlgorithm::SORTED_TWINS;
        else if (algo == "hash_twins") a = DistanceHereditaryAlgorithm::HASH_TWINS;
        else if (algo == "split_decomposition") a = DistanceHereditaryAlgorithm::SPLIT_DECOMPOSITION;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for distance_hereditary. Valid: 'hashmap_twins', 'sorted_twins', 'hash_twins', 'split_decomposition'");
    }
    return check_distance_hereditary(g, a).is_distance_hereditary;
}

// --- interval ---
static bool check_interval_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    IntervalAlgorithm a = IntervalAlgorithm::AT_FREE;
    if (!algo.empty()) {
        if (algo == "backtracking") a = IntervalAlgorithm::BACKTRACKING;
        else if (algo == "at_free") a = IntervalAlgorithm::AT_FREE;
        else if (algo == "pq_tree") a = IntervalAlgorithm::PQ_TREE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for interval. Valid: 'backtracking', 'at_free', 'pq_tree'");
    }
    return check_interval(g, a).is_interval;
}

// --- line_graph ---
static bool check_line_graph_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    LineGraphAlgorithm a = LineGraphAlgorithm::KRAUSZ;
    if (!algo.empty()) {
        if (algo == "brute") a = LineGraphAlgorithm::BRUTE;
        else if (algo == "krausz") a = LineGraphAlgorithm::KRAUSZ;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for line_graph. Valid: 'brute', 'krausz'");
    }
    return check_line_graph(g, a).is_line_graph;
}

// --- outer_planar ---
static bool check_outer_planar_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    OuterPlanarAlgorithm a = OuterPlanarAlgorithm::AUGMENTED_PLANARITY;
    if (!algo.empty()) {
        if (algo == "minor_check") a = OuterPlanarAlgorithm::MINOR_CHECK;
        else if (algo == "augmented_planarity") a = OuterPlanarAlgorithm::AUGMENTED_PLANARITY;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for outer_planar. Valid: 'minor_check', 'augmented_planarity'");
    }
    return check_outer_planar(g, a).is_outer_planar;
}

// --- perfect ---
static bool check_perfect_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    if (!algo.empty()) {
        throw std::invalid_argument("Unknown algorithm '" + algo + "' for perfect. No algorithm options available.");
    }
    return check_perfect(g).is_perfect;
}

// --- permutation ---
static bool check_permutation_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    PermutationAlgorithm a = PermutationAlgorithm::CLASS_BASED;
    if (!algo.empty()) {
        if (algo == "backtracking") a = PermutationAlgorithm::BACKTRACKING;
        else if (algo == "class_based") a = PermutationAlgorithm::CLASS_BASED;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for permutation. Valid: 'backtracking', 'class_based'");
    }
    return check_permutation(g, a).is_permutation;
}

// --- planar ---
static bool check_planar_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    PlanarAlgorithm a = PlanarAlgorithm::LEFT_RIGHT;
    if (!algo.empty()) {
        if (algo == "left_right") a = PlanarAlgorithm::LEFT_RIGHT;
        else if (algo == "minor_check") a = PlanarAlgorithm::MINOR_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for planar. Valid: 'left_right', 'minor_check'");
    }
    return check_planar(g, a).is_planar;
}

// --- proper_interval ---
static bool check_proper_interval_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ProperIntervalAlgorithm a = ProperIntervalAlgorithm::FAST_CLAW_CHECK;
    if (!algo.empty()) {
        if (algo == "triple_loop_claw_check") a = ProperIntervalAlgorithm::TRIPLE_LOOP_CLAW_CHECK;
        else if (algo == "fast_claw_check") a = ProperIntervalAlgorithm::FAST_CLAW_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for proper_interval. Valid: 'triple_loop_claw_check', 'fast_claw_check'");
    }
    return check_proper_interval(g, a).is_proper_interval;
}

// --- ptolemaic ---
static bool check_ptolemaic_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    PtolemaicAlgorithm a = PtolemaicAlgorithm::DH_SORTED;
    if (!algo.empty()) {
        if (algo == "dh_hashmap") a = PtolemaicAlgorithm::DH_HASHMAP;
        else if (algo == "dh_sorted") a = PtolemaicAlgorithm::DH_SORTED;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for ptolemaic. Valid: 'dh_hashmap', 'dh_sorted'");
    }
    return check_ptolemaic(g, a).is_ptolemaic;
}

// --- quasi_threshold ---
static bool check_quasi_threshold_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    QuasiThresholdAlgorithm a = QuasiThresholdAlgorithm::DFS;
    if (!algo.empty()) {
        if (algo == "dfs") a = QuasiThresholdAlgorithm::DFS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for quasi_threshold. Valid: 'dfs'");
    }
    return check_quasi_threshold(g, a).is_quasi_threshold;
}

// --- series_parallel ---
static bool check_series_parallel_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    SeriesParallelAlgorithm a = SeriesParallelAlgorithm::QUEUE_REDUCTION;
    if (!algo.empty()) {
        if (algo == "minor_check") a = SeriesParallelAlgorithm::MINOR_CHECK;
        else if (algo == "queue_reduction") a = SeriesParallelAlgorithm::QUEUE_REDUCTION;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for series_parallel. Valid: 'minor_check', 'queue_reduction'");
    }
    return check_series_parallel(g, a).is_series_parallel;
}

// --- split ---
static bool check_split_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    SplitAlgorithm a = SplitAlgorithm::HAMMER_SIMEONE;
    if (!algo.empty()) {
        if (algo == "degree_sequence") a = SplitAlgorithm::DEGREE_SEQUENCE;
        else if (algo == "hammer_simeone") a = SplitAlgorithm::HAMMER_SIMEONE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for split. Valid: 'degree_sequence', 'hammer_simeone'");
    }
    return check_split(g, a).is_split;
}

// --- strongly_chordal ---
static bool check_strongly_chordal_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    StronglyChordalAlgorithm a = StronglyChordalAlgorithm::MCS_SEO;
    if (!algo.empty()) {
        if (algo == "strong_elimination") a = StronglyChordalAlgorithm::STRONG_ELIMINATION;
        else if (algo == "peo_matrix") a = StronglyChordalAlgorithm::PEO_MATRIX;
        else if (algo == "mcs_seo") a = StronglyChordalAlgorithm::MCS_SEO;
        else if (algo == "farber_seo") a = StronglyChordalAlgorithm::FARBER_SEO;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for strongly_chordal. Valid: 'strong_elimination', 'peo_matrix', 'mcs_seo', 'farber_seo'");
    }
    return check_strongly_chordal(g, a).is_strongly_chordal;
}

// --- three_leaf_power ---
static bool check_three_leaf_power_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    if (!algo.empty()) {
        throw std::invalid_argument("Unknown algorithm '" + algo + "' for three_leaf_power. No algorithm options available.");
    }
    return check_three_leaf_power(g).is_three_leaf_power;
}

// --- threshold ---
static bool check_threshold_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ThresholdAlgorithm a = ThresholdAlgorithm::DEGREE_SEQUENCE_FAST;
    if (!algo.empty()) {
        if (algo == "degree_sequence") a = ThresholdAlgorithm::DEGREE_SEQUENCE;
        else if (algo == "degree_sequence_fast") a = ThresholdAlgorithm::DEGREE_SEQUENCE_FAST;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for threshold. Valid: 'degree_sequence', 'degree_sequence_fast'");
    }
    return check_threshold(g, a).is_threshold;
}

// --- trapezoid ---
static bool check_trapezoid_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    TrapezoidAlgorithm a = TrapezoidAlgorithm::CHAIN_COVER;
    if (!algo.empty()) {
        if (algo == "chain_cover") a = TrapezoidAlgorithm::CHAIN_COVER;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for trapezoid. Valid: 'chain_cover'");
    }
    return check_trapezoid(g, a).is_trapezoid;
}

// --- trivially_perfect ---
static bool check_trivially_perfect_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    TriviallyPerfectAlgorithm a = TriviallyPerfectAlgorithm::DFS;
    if (!algo.empty()) {
        if (algo == "dfs") a = TriviallyPerfectAlgorithm::DFS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for trivially_perfect. Valid: 'dfs'");
    }
    return check_trivially_perfect(g, a).is_trivially_perfect;
}

// --- unit_interval ---
static bool check_unit_interval_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    UnitIntervalAlgorithm a = UnitIntervalAlgorithm::PROPER_INTERVAL;
    if (!algo.empty()) {
        if (algo == "proper_interval") a = UnitIntervalAlgorithm::PROPER_INTERVAL;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for unit_interval. Valid: 'proper_interval'");
    }
    return check_unit_interval(g, a).is_unit_interval;
}

// --- weakly_chordal ---
static bool check_weakly_chordal_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    WeaklyChordalAlgorithm a = WeaklyChordalAlgorithm::COMPLEMENT_BFS;
    if (!algo.empty()) {
        if (algo == "co_chordal_bipartite") a = WeaklyChordalAlgorithm::CO_CHORDAL_BIPARTITE;
        else if (algo == "complement_bfs") a = WeaklyChordalAlgorithm::COMPLEMENT_BFS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for weakly_chordal. Valid: 'co_chordal_bipartite', 'complement_bfs'");
    }
    return check_weakly_chordal(g, a).is_weakly_chordal;
}

// --- apex ---
static bool check_apex_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ApexAlgorithm a = ApexAlgorithm::VERTEX_DELETION;
    if (!algo.empty()) {
        if (algo == "vertex_deletion") a = ApexAlgorithm::VERTEX_DELETION;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for apex. Valid: 'vertex_deletion'");
    }
    return check_apex(g, a).is_apex;
}

// --- biconnected ---
static bool check_biconnected_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    BiconnectedAlgorithm a = BiconnectedAlgorithm::DFS;
    if (!algo.empty()) {
        if (algo == "dfs") a = BiconnectedAlgorithm::DFS;
        else if (algo == "block_cut_tree") a = BiconnectedAlgorithm::BLOCK_CUT_TREE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for biconnected. Valid: 'dfs', 'block_cut_tree'");
    }
    return check_biconnected(g, a).is_biconnected;
}

// --- bull_free ---
static bool check_bull_free_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    BullFreeAlgorithm a = BullFreeAlgorithm::TRIANGLE_SEARCH;
    if (!algo.empty()) {
        if (algo == "triangle_search") a = BullFreeAlgorithm::TRIANGLE_SEARCH;
        else if (algo == "brute") a = BullFreeAlgorithm::BRUTE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for bull_free. Valid: 'triangle_search', 'brute'");
    }
    return check_bull_free(g, a).is_bull_free;
}

// --- caterpillar ---
static bool check_caterpillar_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CaterpillarAlgorithm a = CaterpillarAlgorithm::LEAF_REMOVAL;
    if (!algo.empty()) {
        if (algo == "leaf_removal") a = CaterpillarAlgorithm::LEAF_REMOVAL;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for caterpillar. Valid: 'leaf_removal'");
    }
    return check_caterpillar(g, a).is_caterpillar;
}

// --- circle ---
static bool check_circle_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CircleAlgorithm a = CircleAlgorithm::NAJI_SYSTEM;
    if (!algo.empty()) {
        if (algo == "naji_system") a = CircleAlgorithm::NAJI_SYSTEM;
        else if (algo == "dow_backtracking") a = CircleAlgorithm::DOW_BACKTRACKING;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for circle. Valid: 'naji_system', 'dow_backtracking'");
    }
    return check_circle(g, a).is_circle;
}

// --- cluster ---
static bool check_cluster_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ClusterAlgorithm a = ClusterAlgorithm::COMPONENT_CLIQUE;
    if (!algo.empty()) {
        if (algo == "component_clique") a = ClusterAlgorithm::COMPONENT_CLIQUE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for cluster. Valid: 'component_clique'");
    }
    return check_cluster(g, a).is_cluster;
}

// --- cubic ---
static bool check_cubic_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CubicAlgorithm a = CubicAlgorithm::DEGREE_CHECK;
    if (!algo.empty()) {
        if (algo == "degree_check") a = CubicAlgorithm::DEGREE_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for cubic. Valid: 'degree_check'");
    }
    return check_cubic(g, a).is_cubic;
}

// --- cubic_planar ---
static bool check_cubic_planar_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CubicPlanarAlgorithm a = CubicPlanarAlgorithm::CUBIC_AND_PLANAR;
    if (!algo.empty()) {
        if (algo == "cubic_and_planar") a = CubicPlanarAlgorithm::CUBIC_AND_PLANAR;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for cubic_planar. Valid: 'cubic_and_planar'");
    }
    return check_cubic_planar(g, a).is_cubic_planar;
}

// --- digraph ---
static bool check_digraph_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    DigraphAlgorithm a = DigraphAlgorithm::VALIDITY_CHECK;
    if (!algo.empty()) {
        if (algo == "validity_check") a = DigraphAlgorithm::VALIDITY_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for digraph. Valid: 'validity_check'");
    }
    return check_digraph(n, edges, a).is_digraph;
}

// --- eulerian ---
static bool check_eulerian_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    EulerianAlgorithm a = EulerianAlgorithm::DEGREE_CHECK;
    if (!algo.empty()) {
        if (algo == "degree_check") a = EulerianAlgorithm::DEGREE_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for eulerian. Valid: 'degree_check'");
    }
    return check_eulerian(g, a).is_eulerian;
}

// --- even_hole_free ---
static bool check_even_hole_free_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    if (!algo.empty()) {
        throw std::invalid_argument("Unknown algorithm '" + algo + "' for even_hole_free. No algorithm options available.");
    }
    return check_even_hole_free(g).is_even_hole_free;
}

// --- five_leaf_power ---
static bool check_five_leaf_power_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    if (!algo.empty()) {
        throw std::invalid_argument("Unknown algorithm '" + algo + "' for five_leaf_power. No algorithm options available.");
    }
    return check_five_leaf_power(g).is_five_leaf_power;
}

// --- forest ---
static bool check_forest_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ForestAlgorithm a = ForestAlgorithm::BFS;
    if (!algo.empty()) {
        if (algo == "bfs") a = ForestAlgorithm::BFS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for forest. Valid: 'bfs'");
    }
    return check_forest(g, a).is_forest;
}

// --- four_leaf_power ---
static bool check_four_leaf_power_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    if (!algo.empty()) {
        throw std::invalid_argument("Unknown algorithm '" + algo + "' for four_leaf_power. No algorithm options available.");
    }
    return check_four_leaf_power(g).is_four_leaf_power;
}

// --- fullerene ---
static bool check_fullerene_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    FullereneAlgorithm a = FullereneAlgorithm::FACE_CHECK;
    if (!algo.empty()) {
        if (algo == "face_check") a = FullereneAlgorithm::FACE_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for fullerene. Valid: 'face_check'");
    }
    return check_fullerene(g, a).is_fullerene;
}

// --- gem_free ---
static bool check_gem_free_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    GemFreeAlgorithm a = GemFreeAlgorithm::NEIGHBOR_P4_SEARCH;
    if (!algo.empty()) {
        if (algo == "neighbor_p4_search") a = GemFreeAlgorithm::NEIGHBOR_P4_SEARCH;
        else if (algo == "brute") a = GemFreeAlgorithm::BRUTE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for gem_free. Valid: 'neighbor_p4_search', 'brute'");
    }
    return check_gem_free(g, a).is_gem_free;
}

// --- halin ---
static bool check_halin_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    HalinAlgorithm a = HalinAlgorithm::FACE_CHECK;
    if (!algo.empty()) {
        if (algo == "face_check") a = HalinAlgorithm::FACE_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for halin. Valid: 'face_check'");
    }
    return check_halin(g, a).is_halin;
}

// --- kregular ---
static bool check_kregular_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    KRegularAlgorithm a = KRegularAlgorithm::DEGREE_CHECK;
    if (!algo.empty()) {
        if (algo == "degree_check") a = KRegularAlgorithm::DEGREE_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for kregular. Valid: 'degree_check'");
    }
    return check_kregular(g, a).is_kregular;
}

// --- ktree ---
static bool check_ktree_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    KTreeAlgorithm a = KTreeAlgorithm::SIMPLICIAL_REMOVAL;
    if (!algo.empty()) {
        if (algo == "simplicial_removal") a = KTreeAlgorithm::SIMPLICIAL_REMOVAL;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for ktree. Valid: 'simplicial_removal'");
    }
    return check_ktree(g, a).is_ktree;
}

// --- laman ---
static bool check_laman_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    LamanAlgorithm a = LamanAlgorithm::PEBBLE_GAME;
    if (!algo.empty()) {
        if (algo == "pebble_game") a = LamanAlgorithm::PEBBLE_GAME;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for laman. Valid: 'pebble_game'");
    }
    return check_laman(g, a).is_laman;
}

// --- maximal_outer_planar ---
static bool check_maximal_outer_planar_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    MaximalOuterPlanarAlgorithm a = MaximalOuterPlanarAlgorithm::OUTER_PLANAR_EDGE_COUNT;
    if (!algo.empty()) {
        if (algo == "outer_planar_edge_count") a = MaximalOuterPlanarAlgorithm::OUTER_PLANAR_EDGE_COUNT;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for maximal_outer_planar. Valid: 'outer_planar_edge_count'");
    }
    return check_maximal_outer_planar(g, a).is_maximal_outer_planar;
}

// --- maximal_planar ---
static bool check_maximal_planar_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    MaximalPlanarAlgorithm a = MaximalPlanarAlgorithm::PLANAR_EDGE_COUNT;
    if (!algo.empty()) {
        if (algo == "planar_edge_count") a = MaximalPlanarAlgorithm::PLANAR_EDGE_COUNT;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for maximal_planar. Valid: 'planar_edge_count'");
    }
    return check_maximal_planar(g, a).is_maximal_planar;
}

// --- meyniel ---
static bool check_meyniel_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    MeynielAlgorithm a = MeynielAlgorithm::DIRECT_CHECK;
    if (!algo.empty()) {
        if (algo == "direct_check") a = MeynielAlgorithm::DIRECT_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for meyniel. Valid: 'direct_check'");
    }
    return check_meyniel(g, a).is_meyniel;
}

// --- odd_hole_free ---
static bool check_odd_hole_free_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    if (!algo.empty()) {
        throw std::invalid_argument("Unknown algorithm '" + algo + "' for odd_hole_free. No algorithm options available.");
    }
    return check_odd_hole_free(g).is_odd_hole_free;
}

// --- p5_free ---
static bool check_p5_free_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    P5FreeAlgorithm a = P5FreeAlgorithm::PATH_SEARCH;
    if (!algo.empty()) {
        if (algo == "path_search") a = P5FreeAlgorithm::PATH_SEARCH;
        else if (algo == "brute") a = P5FreeAlgorithm::BRUTE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for p5_free. Valid: 'path_search', 'brute'");
    }
    return check_p5_free(g, a).is_p5_free;
}

// --- parity ---
static bool check_parity_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ParityAlgorithm a = ParityAlgorithm::DIRECT_CHECK;
    if (!algo.empty()) {
        if (algo == "direct_check") a = ParityAlgorithm::DIRECT_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for parity. Valid: 'direct_check'");
    }
    return check_parity(g, a).is_parity;
}

// --- polyhedral ---
static bool check_polyhedral_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    PolyhedralAlgorithm a = PolyhedralAlgorithm::STEINITZ;
    if (!algo.empty()) {
        if (algo == "steinitz") a = PolyhedralAlgorithm::STEINITZ;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for polyhedral. Valid: 'steinitz'");
    }
    return check_polyhedral(g, a).is_polyhedral;
}

// --- poset ---
static bool check_poset_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    PosetAlgorithm a = PosetAlgorithm::DAG_AND_REDUCTION;
    if (!algo.empty()) {
        if (algo == "dag_and_reduction") a = PosetAlgorithm::DAG_AND_REDUCTION;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for poset. Valid: 'dag_and_reduction'");
    }
    return check_poset(n, edges, a).is_poset;
}

// --- proper_chordal ---
static bool check_proper_chordal_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    if (!algo.empty()) {
        throw std::invalid_argument("Unknown algorithm '" + algo + "' for proper_chordal. No algorithm options available.");
    }
    return check_proper_chordal(g).is_proper_chordal;
}

// --- proper_circular_arc ---
static bool check_proper_circular_arc_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    if (!algo.empty()) {
        throw std::invalid_argument("Unknown algorithm '" + algo + "' for proper_circular_arc. No algorithm options available.");
    }
    return check_proper_circular_arc(g).is_proper_circular_arc;
}

// --- self_complementary ---
static bool check_self_complementary_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    SelfComplementaryAlgorithm a = SelfComplementaryAlgorithm::ISOMORPHISM_CHECK;
    if (!algo.empty()) {
        if (algo == "isomorphism_check") a = SelfComplementaryAlgorithm::ISOMORPHISM_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for self_complementary. Valid: 'isomorphism_check'");
    }
    return check_self_complementary(g, a).is_self_complementary;
}

// --- simple_quadrangulation ---
static bool check_simple_quadrangulation_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    SimpleQuadrangulationAlgorithm a = SimpleQuadrangulationAlgorithm::COMBINED;
    if (!algo.empty()) {
        if (algo == "combined") a = SimpleQuadrangulationAlgorithm::COMBINED;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for simple_quadrangulation. Valid: 'combined'");
    }
    return check_simple_quadrangulation(g, a).is_simple_quadrangulation;
}

// --- snark ---
static bool check_snark_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    SnarkAlgorithm a = SnarkAlgorithm::COMBINED;
    if (!algo.empty()) {
        if (algo == "combined") a = SnarkAlgorithm::COMBINED;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for snark. Valid: 'combined'");
    }
    return check_snark(g, a).is_snark;
}

// --- strongly_regular ---
static bool check_strongly_regular_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    StronglyRegularAlgorithm a = StronglyRegularAlgorithm::PARAMETER_CHECK;
    if (!algo.empty()) {
        if (algo == "parameter_check") a = StronglyRegularAlgorithm::PARAMETER_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for strongly_regular. Valid: 'parameter_check'");
    }
    return check_strongly_regular(g, a).is_strongly_regular;
}

// --- tournament ---
static bool check_tournament_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    TournamentAlgorithm a = TournamentAlgorithm::ARC_CHECK;
    if (!algo.empty()) {
        if (algo == "arc_check") a = TournamentAlgorithm::ARC_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for tournament. Valid: 'arc_check'");
    }
    return check_tournament(n, edges, a).is_tournament;
}

// --- tree ---
static bool check_tree_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    TreeAlgorithm a = TreeAlgorithm::BFS;
    if (!algo.empty()) {
        if (algo == "bfs") a = TreeAlgorithm::BFS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for tree. Valid: 'bfs'");
    }
    return check_tree(g, a).is_tree;
}

// --- triangle_free ---
static bool check_triangle_free_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    TriangleFreeAlgorithm a = TriangleFreeAlgorithm::EDGE_PAIR;
    if (!algo.empty()) {
        if (algo == "edge_pair") a = TriangleFreeAlgorithm::EDGE_PAIR;
        else if (algo == "brute") a = TriangleFreeAlgorithm::BRUTE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for triangle_free. Valid: 'edge_pair', 'brute'");
    }
    return check_triangle_free(g, a).is_triangle_free;
}

// --- triconnected ---
static bool check_triconnected_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    TriconnectedAlgorithm a = TriconnectedAlgorithm::NAIVE;
    if (!algo.empty()) {
        if (algo == "naive") a = TriconnectedAlgorithm::NAIVE;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for triconnected. Valid: 'naive'");
    }
    return check_triconnected(g, a).is_triconnected;
}

// --- unicyclic ---
static bool check_unicyclic_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    UnicyclicAlgorithm a = UnicyclicAlgorithm::BFS;
    if (!algo.empty()) {
        if (algo == "bfs") a = UnicyclicAlgorithm::BFS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for unicyclic. Valid: 'bfs'");
    }
    return check_unicyclic(g, a).is_unicyclic;
}

// ============================================================
// Enumeration functions
// ============================================================

// Helper to convert EnumeratedGraph vector to Python-friendly format
template<typename ResultT>
static EnumResultPy convert_enum_result(const ResultT& result) {
    EnumResultPy out;
    out.reserve(result.graphs.size());
    for (size_t i = 0; i < result.graphs.size(); ++i) {
        out.push_back(std::make_pair(result.graphs[i].n, result.graphs[i].edges));
    }
    return out;
}

static EnumResultPy enumerate_biconnected_unlabeled_py(int n) {
    return convert_enum_result(enumerate_biconnected_unlabeled_graphs(n));
}

static EnumResultPy enumerate_biconvex_bipartite_py(int n) {
    return convert_enum_result(enumerate_biconvex_bipartite_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_bipartite_py(int n) {
    return convert_enum_result(enumerate_bipartite_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_bipartite_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_bipartite_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_bipartite_permutation_py(int n) {
    return convert_enum_result(enumerate_bipartite_permutation_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_bipartite_permutation_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_bipartite_permutation_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_block_py(int n) {
    return convert_enum_result(enumerate_block_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_cactus_py(int n) {
    return convert_enum_result(enumerate_cactus_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_cactus_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_cactus_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_chain_py(int n) {
    return convert_enum_result(enumerate_chain_unlabeled_graphs(n));
}

static EnumResultPy enumerate_chordal_py(int n) {
    return convert_enum_result(enumerate_chordal_labeled_graphs_reverse_search(n));
}

// Unlike the other enumerators, this one takes a host graph rather than a
// vertex count: it enumerates the chordal subgraphs contained in that graph.
static EnumResultPy enumerate_chordal_subgraphs_py(
    int n, const std::vector<std::pair<int, int>>& edges) {
    return convert_enum_result(enumerate_chordal_subgraphs(make_graph(n, edges)));
}

static EnumResultPy enumerate_chordal_bipartite_py(int n) {
    return convert_enum_result(enumerate_chordal_bipartite_labeled_graphs_reverse_search(n));
}

// Takes a host graph rather than a vertex count, and unlike the subgraph
// enumerators it returns vertex sets: an induced subgraph is determined by
// its vertex set, so no edge list is carried across the boundary.
static std::vector<std::vector<int>>
enumerate_chordal_bipartite_induced_subgraphs_py(
    int n, const std::vector<std::pair<int, int>>& edges) {
    return enumerate_chordal_bipartite_induced_subgraphs(make_graph(n, edges))
        .vertex_sets;
}

static EnumResultPy enumerate_chordal_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_chordal_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_circle_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_circle_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_claw_free_py(int n) {
    return convert_enum_result(enumerate_claw_free_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_co_comparability_py(int n) {
    return convert_enum_result(enumerate_co_comparability_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_co_comparability_unlabeled_py(int n) {
    return convert_enum_result(enumerate_co_comparability_unlabeled_graphs(n));
}

static EnumResultPy enumerate_cluster_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_cluster_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_cochain_py(int n) {
    return convert_enum_result(enumerate_cochain_unlabeled_graphs(n));
}

static EnumResultPy enumerate_cograph_py(int n) {
    return convert_enum_result(enumerate_cograph_labeled_graphs_cotree(n));
}

static EnumResultPy enumerate_cograph_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_cograph_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_comparability_py(int n) {
    return convert_enum_result(enumerate_comparability_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_comparability_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_comparability_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_convex_bipartite_py(int n) {
    return convert_enum_result(enumerate_convex_bipartite_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_diamond_free_py(int n) {
    return convert_enum_result(enumerate_diamond_free_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_distance_hereditary_py(int n) {
    return convert_enum_result(enumerate_distance_hereditary_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_distance_hereditary_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_distance_hereditary_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_interval_py(int n) {
    return convert_enum_result(enumerate_interval_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_interval_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_interval_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_line_graph_py(int n) {
    return convert_enum_result(enumerate_line_graphs_reverse_search(n));
}

static EnumResultPy enumerate_maximal_outer_planar_unlabeled_py(int n) {
    return convert_enum_result(enumerate_maximal_outer_planar_unlabeled_graphs(n));
}

static EnumResultPy enumerate_maximal_planar_unlabeled_py(int n) {
    return convert_enum_result(enumerate_maximal_planar_unlabeled_graphs(n));
}

static EnumResultPy enumerate_outer_planar_py(int n) {
    return convert_enum_result(enumerate_outer_planar_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_outer_planar_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_outer_planar_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_permutation_py(int n) {
    return convert_enum_result(enumerate_permutation_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_permutation_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_permutation_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_planar_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_planar_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_planar_py(int n) {
    return convert_enum_result(enumerate_planar_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_polyhedral_unlabeled_py(int n) {
    return convert_enum_result(enumerate_polyhedral_unlabeled_graphs(n));
}

static EnumResultPy enumerate_proper_interval_py(int n) {
    return convert_enum_result(enumerate_proper_interval_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_proper_interval_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_proper_interval_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_ptolemaic_py(int n) {
    return convert_enum_result(enumerate_ptolemaic_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_ptolemaic_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_ptolemaic_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_self_complementary_unlabeled_py(int n) {
    return convert_enum_result(enumerate_self_complementary_unlabeled_graphs(n));
}

static EnumResultPy enumerate_series_parallel_py(int n) {
    return convert_enum_result(enumerate_series_parallel_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_series_parallel_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_series_parallel_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_split_py(int n) {
    return convert_enum_result(enumerate_split_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_split_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_split_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_three_leaf_power_py(int n) {
    return convert_enum_result(enumerate_three_leaf_power_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_three_leaf_power_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_three_leaf_power_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_threshold_py(int n) {
    return convert_enum_result(enumerate_threshold_unlabeled_graphs(n));
}

static EnumResultPy enumerate_cubic_planar_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_cubic_planar_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_cubic_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_cubic_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_eulerian_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_eulerian_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_triangle_free_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_triangle_free_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_trivially_perfect_py(int n) {
    return convert_enum_result(enumerate_trivially_perfect_labeled_graphs_uvd(n));
}

static EnumResultPy enumerate_trivially_perfect_unlabeled_py(int n, bool connected_only) {
    return convert_enum_result(enumerate_trivially_perfect_unlabeled_graphs(n, connected_only));
}

static EnumResultPy enumerate_apollonian_unlabeled_py(int n) {
    return convert_enum_result(enumerate_apollonian_unlabeled_graphs(n));
}

static EnumResultPy enumerate_at_free_py(int n) {
    return convert_enum_result(enumerate_at_free_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_circular_arc_py(int n) {
    return convert_enum_result(enumerate_circular_arc_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_co_chordal_py(int n) {
    return convert_enum_result(enumerate_co_chordal_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_co_chordal_unlabeled_py(int n) {
    return convert_enum_result(enumerate_co_chordal_unlabeled_graphs(n));
}

static EnumResultPy enumerate_co_interval_py(int n) {
    return convert_enum_result(enumerate_co_interval_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_co_interval_unlabeled_py(int n) {
    return convert_enum_result(enumerate_co_interval_unlabeled_graphs(n));
}

static EnumResultPy enumerate_perfect_py(int n) {
    return convert_enum_result(enumerate_perfect_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_strongly_chordal_py(int n) {
    return convert_enum_result(enumerate_strongly_chordal_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_trapezoid_py(int n) {
    return convert_enum_result(enumerate_trapezoid_labeled_graphs_reverse_search(n));
}

static EnumResultPy enumerate_weakly_chordal_py(int n) {
    return convert_enum_result(enumerate_weakly_chordal_labeled_graphs_reverse_search(n));
}

// ============================================================
// Module definition
// ============================================================


// ===== Decompositions =====
//
// These return plain Python data rather than wrapped C++ objects: lists,
// tuples and dicts of ints. Vertex-indexed vectors keep the library's
// 1-indexed convention, so they have n + 1 entries with index 0 unused.

static std::vector<std::vector<int>> connected_components_py(
    int n, const std::vector<std::pair<int, int>>& edges) {
    return connected_components(make_graph(n, edges)).vertices;
}

static std::vector<std::vector<int>> co_components_py(
    int n, const std::vector<std::pair<int, int>>& edges) {
    return co_components(make_graph(n, edges)).vertices;
}

static py::dict twin_quotient_py(int n, const std::vector<std::pair<int, int>>& edges,
                                 const std::string& kind) {
    Graph g = make_graph(n, edges);
    TwinKind k = TwinKind::BOTH;
    if (!kind.empty()) {
        if (kind == "true") k = TwinKind::TRUE_TWINS;
        else if (kind == "false") k = TwinKind::FALSE_TWINS;
        else if (kind == "both") k = TwinKind::BOTH;
        else throw std::invalid_argument("Unknown twin kind '" + kind +
                                         "'. Valid: 'true', 'false', 'both'");
    }
    TwinQuotientResult q = contract_twins(g, k);
    std::vector<std::pair<int, int>> quotient_edges;
    for (int u = 1; u <= q.quotient.n; ++u) {
        for (size_t i = 0; i < q.quotient.adj[u].size(); ++i) {
            int v = q.quotient.adj[u][i];
            if (u < v) quotient_edges.push_back(std::make_pair(u, v));
        }
    }
    std::vector<std::vector<int>> members(q.members.begin() + 1, q.members.end());
    py::dict out;
    out["members"] = members;
    out["block_of"] = q.block_of;
    out["quotient_edges"] = quotient_edges;
    return out;
}

static py::dict block_cut_tree_py(int n, const std::vector<std::pair<int, int>>& edges) {
    BlockCutTreeResult r = compute_block_cut_tree(make_graph(n, edges));
    std::vector<int> cut_vertices;
    for (int v = 1; v <= n; ++v) {
        if (r.is_cut[v]) cut_vertices.push_back(v);
    }
    std::vector<std::pair<int, int>> incidences;
    for (size_t i = 0; i < r.blocks.size(); ++i) {
        for (size_t j = 0; j < r.blocks[i].size(); ++j) {
            int v = r.blocks[i][j];
            if (r.is_cut[v]) incidences.push_back(std::make_pair((int)i, v));
        }
    }
    py::dict out;
    out["blocks"] = r.blocks;
    out["block_edges"] = r.block_edges;
    out["cut_vertices"] = cut_vertices;
    out["bridges"] = r.bridges;
    out["tree_edges"] = incidences;
    return out;
}

static const char* md_kind_name(MDNodeKind kind) {
    switch (kind) {
        case MDNodeKind::LEAF: return "leaf";
        case MDNodeKind::SERIES: return "series";
        case MDNodeKind::PARALLEL: return "parallel";
        default: return "prime";
    }
}

static py::dict md_tree_dict(const MDTree& t) {
    py::list nodes;
    for (size_t i = 0; i < t.nodes.size(); ++i) {
        py::dict node;
        node["kind"] = std::string(md_kind_name(t.nodes[i].kind));
        node["vertex"] = t.nodes[i].vertex;
        node["parent"] = t.nodes[i].parent;
        node["children"] = t.nodes[i].children;
        node["vertices"] = t.nodes[i].vertices;
        nodes.append(node);
    }
    py::dict out;
    out["root"] = t.root;
    out["nodes"] = nodes;
    return out;
}

static py::dict modular_decomposition_py(int n, const std::vector<std::pair<int, int>>& edges) {
    return md_tree_dict(modular_decomposition(make_graph(n, edges)));
}

static py::dict cotree_py(int n, const std::vector<std::pair<int, int>>& edges,
                          const std::string& algo) {
    Graph g = make_graph(n, edges);
    CographAlgorithm a = CographAlgorithm::PARTITION_REFINEMENT;
    if (!algo.empty()) {
        if (algo == "cotree") a = CographAlgorithm::COTREE;
        else if (algo == "partition_refinement") a = CographAlgorithm::PARTITION_REFINEMENT;
        else if (algo == "modular") a = CographAlgorithm::MODULAR;
        else throw std::invalid_argument("Unknown algorithm '" + algo +
                                         "' for cotree. Valid: 'cotree', 'partition_refinement', 'modular'");
    }
    CotreeResult r = build_cotree(g, a);
    py::dict out = md_tree_dict(r.cotree);
    out["is_cograph"] = r.is_cograph;
    return out;
}

static py::dict transitive_orientation_py(int n, const std::vector<std::pair<int, int>>& edges,
                                          const std::string& algo) {
    Graph g = make_graph(n, edges);
    TransitiveOrientationAlgorithm a = TransitiveOrientationAlgorithm::FORCING;
    if (!algo.empty()) {
        if (algo == "forcing") a = TransitiveOrientationAlgorithm::FORCING;
        else if (algo == "backtracking") a = TransitiveOrientationAlgorithm::BACKTRACKING;
        else throw std::invalid_argument("Unknown algorithm '" + algo +
                                         "' for transitive_orientation. Valid: 'forcing', 'backtracking'");
    }
    TransitiveOrientationResult r = transitive_orientation(g, a);
    py::dict out;
    out["is_comparability"] = r.is_comparability;
    out["orientation"] = r.orientation;
    return out;
}

static py::dict permutation_realizer_py(int n, const std::vector<std::pair<int, int>>& edges) {
    PermutationRealizerResult r = build_permutation_realizer(make_graph(n, edges));
    py::dict out;
    out["is_permutation"] = r.is_permutation;
    out["pos1"] = r.pos1;
    out["pos2"] = r.pos2;
    out["pi"] = r.pi;
    return out;
}

static py::dict clique_tree_py(int n, const std::vector<std::pair<int, int>>& edges) {
    Graph g = make_graph(n, edges);
    ChordalResult ch = check_chordal(g);
    py::dict out;
    out["is_chordal"] = ch.is_chordal;
    if (!ch.is_chordal) {
        out["cliques"] = std::vector<std::vector<int>>();
        out["tree_edges"] = std::vector<std::pair<int, int>>();
        return out;
    }
    CliqueTreeResult ct = build_clique_tree(g, ch);
    std::vector<std::pair<int, int>> tree_edges;
    for (size_t i = 0; i < ct.tree.size(); ++i) {
        for (size_t j = 0; j < ct.tree[i].size(); ++j) {
            int t = ct.tree[i][j];
            if ((int)i < t) tree_edges.push_back(std::make_pair((int)i, t));
        }
    }
    out["cliques"] = ct.mc.cliques;
    out["tree_edges"] = tree_edges;
    return out;
}

static py::dict tree_decomposition_py(int n, const std::vector<std::pair<int, int>>& edges) {
    TreeDecompositionResult r = tree_decomposition_chordal(make_graph(n, edges));
    std::vector<std::pair<int, int>> tree_edges;
    for (size_t i = 0; i < r.tree.size(); ++i) {
        for (size_t j = 0; j < r.tree[i].size(); ++j) {
            int t = r.tree[i][j];
            if ((int)i < t) tree_edges.push_back(std::make_pair((int)i, t));
        }
    }
    py::dict out;
    out["success"] = r.success;
    out["bags"] = r.bags;
    out["tree_edges"] = tree_edges;
    out["width"] = r.width;
    return out;
}

static py::dict split_decomposition_py(int n, const std::vector<std::pair<int, int>>& edges) {
    SplitDecompositionResult r = split_decomposition(make_graph(n, edges));
    py::list bags;
    for (size_t i = 0; i < r.bags.size(); ++i) {
        std::vector<std::pair<int, int>> skel;
        for (int u = 1; u <= r.bags[i].skeleton.n; ++u) {
            for (size_t j = 0; j < r.bags[i].skeleton.adj[u].size(); ++j) {
                int v = r.bags[i].skeleton.adj[u][j];
                if (u < v) skel.push_back(std::make_pair(u, v));
            }
        }
        py::dict bag;
        bag["kind"] = std::string(r.bags[i].kind == SplitNodeKind::CLIQUE
                                      ? "clique"
                                      : (r.bags[i].kind == SplitNodeKind::STAR ? "star" : "prime"));
        bag["label"] = r.bags[i].label;
        bag["edges"] = skel;
        bag["center"] = r.bags[i].center;
        bags.append(bag);
    }
    py::list tree_edges;
    for (size_t i = 0; i < r.tree_edges.size(); ++i) {
        py::tuple e = py::make_tuple(r.tree_edges[i].bag_u, r.tree_edges[i].marker_u,
                                     r.tree_edges[i].bag_v, r.tree_edges[i].marker_v);
        tree_edges.append(e);
    }
    py::dict out;
    out["success"] = r.success;
    out["totally_decomposable"] = r.totally_decomposable;
    out["bags"] = bags;
    out["tree_edges"] = tree_edges;
    return out;
}

static py::dict spqr_tree_py(int n, const std::vector<std::pair<int, int>>& edges) {
    SPQRTreeResult r = compute_spqr_tree(make_graph(n, edges));
    py::list nodes;
    for (size_t i = 0; i < r.nodes.size(); ++i) {
        py::dict node;
        node["kind"] = std::string(r.nodes[i].kind == SPQRNodeKind::S
                                       ? "S"
                                       : (r.nodes[i].kind == SPQRNodeKind::P ? "P" : "R"));
        node["vertices"] = r.nodes[i].vertices;
        node["edges"] = r.nodes[i].edges;
        node["edge_orig"] = r.nodes[i].edge_orig;
        nodes.append(node);
    }
    py::list tree_edges;
    for (size_t i = 0; i < r.tree_edges.size(); ++i) {
        tree_edges.append(py::make_tuple(r.tree_edges[i].node_u, r.tree_edges[i].edge_u,
                                         r.tree_edges[i].node_v, r.tree_edges[i].edge_v));
    }
    py::dict out;
    out["success"] = r.success;
    out["nodes"] = nodes;
    out["tree_edges"] = tree_edges;
    return out;
}

static py::dict planar_embedding_py(int n, const std::vector<std::pair<int, int>>& edges,
                                    const std::string& algo) {
    Graph g = make_graph(n, edges);
    PlanarEmbeddingAlgorithm a = PlanarEmbeddingAlgorithm::DMP_GENERAL;
    if (!algo.empty()) {
        if (algo == "dmp_general") a = PlanarEmbeddingAlgorithm::DMP_GENERAL;
        else if (algo == "tutte_3connected") a = PlanarEmbeddingAlgorithm::TUTTE_3CONNECTED;
        else throw std::invalid_argument("Unknown algorithm '" + algo +
                                         "' for planar_embedding. Valid: 'dmp_general', 'tutte_3connected'");
    }
    PlanarEmbeddingResult r = compute_planar_embedding(g, a);
    py::dict out;
    out["success"] = r.success;
    out["rotation"] = r.rotation;
    out["faces"] = r.faces;
    return out;
}

static py::dict strong_elimination_ordering_py(int n,
                                               const std::vector<std::pair<int, int>>& edges) {
    StrongEliminationResult r = compute_strong_elimination_ordering(make_graph(n, edges));
    py::dict out;
    out["success"] = r.success;
    out["order"] = r.order;
    out["number"] = r.number;
    return out;
}

static py::dict indifference_tree_layout_py(int n,
                                            const std::vector<std::pair<int, int>>& edges) {
    TreeLayoutResult r = find_indifference_tree_layout(make_graph(n, edges));
    py::dict out;
    out["success"] = r.success;
    out["parent"] = r.parent;
    return out;
}

static py::dict consecutive_ones_py(int num_columns,
                                    const std::vector<std::vector<int>>& rows) {
    ConsecutiveOnesResult r = consecutive_ones(num_columns, rows);
    py::dict out;
    out["success"] = r.success;
    out["column_order"] = r.column_order;
    return out;
}

PYBIND11_MODULE(_core, m) {
    m.doc() = "C++ graph recognition bindings";

    // Decompositions
    m.def("_connected_components", &connected_components_py, py::arg("n"), py::arg("edges"));
    m.def("_co_components", &co_components_py, py::arg("n"), py::arg("edges"));
    m.def("_twin_quotient", &twin_quotient_py, py::arg("n"), py::arg("edges"), py::arg("kind") = "");
    m.def("_block_cut_tree", &block_cut_tree_py, py::arg("n"), py::arg("edges"));
    m.def("_modular_decomposition", &modular_decomposition_py, py::arg("n"), py::arg("edges"));
    m.def("_cotree", &cotree_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_transitive_orientation", &transitive_orientation_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_permutation_realizer", &permutation_realizer_py, py::arg("n"), py::arg("edges"));
    m.def("_clique_tree", &clique_tree_py, py::arg("n"), py::arg("edges"));
    m.def("_tree_decomposition", &tree_decomposition_py, py::arg("n"), py::arg("edges"));
    m.def("_split_decomposition", &split_decomposition_py, py::arg("n"), py::arg("edges"));
    m.def("_spqr_tree", &spqr_tree_py, py::arg("n"), py::arg("edges"));
    m.def("_planar_embedding", &planar_embedding_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_strong_elimination_ordering", &strong_elimination_ordering_py, py::arg("n"), py::arg("edges"));
    m.def("_indifference_tree_layout", &indifference_tree_layout_py, py::arg("n"), py::arg("edges"));
    m.def("_consecutive_ones", &consecutive_ones_py, py::arg("num_columns"), py::arg("rows"));

    // NO-side certificates
    m.def("_obstruction", &obstruction_py, py::arg("type_name"), py::arg("n"),
          py::arg("edges"));
    m.def("_certified_types", &certified_types_py);

    // Recognition functions
    m.def("_check_apollonian", &check_apollonian_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_at_free", &check_at_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_biconvex_bipartite", &check_biconvex_bipartite_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_bipartite", &check_bipartite_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_bipartite_permutation", &check_bipartite_permutation_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_block", &check_block_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_cactus", &check_cactus_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_chain", &check_chain_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_chordal", &check_chordal_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_chordal_bipartite", &check_chordal_bipartite_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_circular_arc", &check_circular_arc_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_claw_free", &check_claw_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_co_chordal", &check_co_chordal_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_co_comparability", &check_co_comparability_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_co_interval", &check_co_interval_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_cochain", &check_cochain_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_cograph", &check_cograph_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_comparability", &check_comparability_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_convex_bipartite", &check_convex_bipartite_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_diamond_free", &check_diamond_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_distance_hereditary", &check_distance_hereditary_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_interval", &check_interval_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_line_graph", &check_line_graph_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_outer_planar", &check_outer_planar_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_perfect", &check_perfect_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_permutation", &check_permutation_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_planar", &check_planar_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_proper_interval", &check_proper_interval_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_ptolemaic", &check_ptolemaic_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_quasi_threshold", &check_quasi_threshold_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_series_parallel", &check_series_parallel_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_split", &check_split_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_strongly_chordal", &check_strongly_chordal_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_three_leaf_power", &check_three_leaf_power_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_threshold", &check_threshold_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_trapezoid", &check_trapezoid_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_trivially_perfect", &check_trivially_perfect_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_unit_interval", &check_unit_interval_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_weakly_chordal", &check_weakly_chordal_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_apex", &check_apex_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_biconnected", &check_biconnected_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_bull_free", &check_bull_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_caterpillar", &check_caterpillar_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_circle", &check_circle_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_cluster", &check_cluster_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_cubic", &check_cubic_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_cubic_planar", &check_cubic_planar_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_digraph", &check_digraph_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_eulerian", &check_eulerian_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_even_hole_free", &check_even_hole_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_five_leaf_power", &check_five_leaf_power_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_forest", &check_forest_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_four_leaf_power", &check_four_leaf_power_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_fullerene", &check_fullerene_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_gem_free", &check_gem_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_halin", &check_halin_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_kregular", &check_kregular_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_ktree", &check_ktree_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_laman", &check_laman_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_maximal_outer_planar", &check_maximal_outer_planar_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_maximal_planar", &check_maximal_planar_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_meyniel", &check_meyniel_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_odd_hole_free", &check_odd_hole_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_p5_free", &check_p5_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_parity", &check_parity_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_polyhedral", &check_polyhedral_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_poset", &check_poset_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_proper_chordal", &check_proper_chordal_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_proper_circular_arc", &check_proper_circular_arc_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_self_complementary", &check_self_complementary_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_simple_quadrangulation", &check_simple_quadrangulation_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_snark", &check_snark_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_strongly_regular", &check_strongly_regular_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_tournament", &check_tournament_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_tree", &check_tree_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_triangle_free", &check_triangle_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_triconnected", &check_triconnected_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_unicyclic", &check_unicyclic_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");

    // Enumeration functions
    m.def("_enumerate_biconnected_unlabeled", &enumerate_biconnected_unlabeled_py, py::arg("n"));
    m.def("_enumerate_biconvex_bipartite_labeled", &enumerate_biconvex_bipartite_py, py::arg("n"));
    m.def("_enumerate_bipartite_labeled", &enumerate_bipartite_py, py::arg("n"));
    m.def("_enumerate_bipartite_unlabeled", &enumerate_bipartite_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_bipartite_permutation_labeled", &enumerate_bipartite_permutation_py, py::arg("n"));
    m.def("_enumerate_bipartite_permutation_unlabeled", &enumerate_bipartite_permutation_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_block_labeled", &enumerate_block_py, py::arg("n"));
    m.def("_enumerate_cactus_labeled", &enumerate_cactus_py, py::arg("n"));
    m.def("_enumerate_cactus_unlabeled", &enumerate_cactus_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_chain_unlabeled", &enumerate_chain_py, py::arg("n"));
    m.def("_enumerate_chordal_labeled", &enumerate_chordal_py, py::arg("n"));
    m.def("_enumerate_chordal_bipartite_labeled", &enumerate_chordal_bipartite_py, py::arg("n"));
    m.def("_enumerate_chordal_bipartite_induced_subgraphs",
          &enumerate_chordal_bipartite_induced_subgraphs_py, py::arg("n"),
          py::arg("edges"));
    m.def("_enumerate_chordal_subgraphs", &enumerate_chordal_subgraphs_py,
          py::arg("n"), py::arg("edges"));
    m.def("_enumerate_chordal_unlabeled", &enumerate_chordal_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_circle_unlabeled", &enumerate_circle_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_claw_free_labeled", &enumerate_claw_free_py, py::arg("n"));
    m.def("_enumerate_co_comparability_labeled", &enumerate_co_comparability_py, py::arg("n"));
    m.def("_enumerate_co_comparability_unlabeled",
          &enumerate_co_comparability_unlabeled_py, py::arg("n"));
    m.def("_enumerate_cluster_unlabeled", &enumerate_cluster_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_cochain_unlabeled", &enumerate_cochain_py, py::arg("n"));
    m.def("_enumerate_cograph_labeled", &enumerate_cograph_py, py::arg("n"));
    m.def("_enumerate_cograph_unlabeled", &enumerate_cograph_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_comparability_labeled", &enumerate_comparability_py, py::arg("n"));
    m.def("_enumerate_comparability_unlabeled",
          &enumerate_comparability_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_convex_bipartite_labeled", &enumerate_convex_bipartite_py, py::arg("n"));
    m.def("_enumerate_diamond_free_labeled", &enumerate_diamond_free_py, py::arg("n"));
    m.def("_enumerate_distance_hereditary_labeled", &enumerate_distance_hereditary_py, py::arg("n"));
    m.def("_enumerate_distance_hereditary_unlabeled", &enumerate_distance_hereditary_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_interval_labeled", &enumerate_interval_py, py::arg("n"));
    m.def("_enumerate_interval_unlabeled", &enumerate_interval_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_line_graph_labeled", &enumerate_line_graph_py, py::arg("n"));
    m.def("_enumerate_maximal_outer_planar_unlabeled", &enumerate_maximal_outer_planar_unlabeled_py, py::arg("n"));
    m.def("_enumerate_maximal_planar_unlabeled", &enumerate_maximal_planar_unlabeled_py, py::arg("n"));
    m.def("_enumerate_outer_planar_labeled", &enumerate_outer_planar_py, py::arg("n"));
    m.def("_enumerate_outer_planar_unlabeled", &enumerate_outer_planar_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_permutation_labeled", &enumerate_permutation_py, py::arg("n"));
    m.def("_enumerate_permutation_unlabeled", &enumerate_permutation_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_planar_labeled", &enumerate_planar_py, py::arg("n"));
    m.def("_enumerate_planar_unlabeled", &enumerate_planar_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_polyhedral_unlabeled", &enumerate_polyhedral_unlabeled_py, py::arg("n"));
    m.def("_enumerate_proper_interval_labeled", &enumerate_proper_interval_py, py::arg("n"));
    m.def("_enumerate_proper_interval_unlabeled", &enumerate_proper_interval_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_ptolemaic_labeled", &enumerate_ptolemaic_py, py::arg("n"));
    m.def("_enumerate_ptolemaic_unlabeled", &enumerate_ptolemaic_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_self_complementary_unlabeled", &enumerate_self_complementary_unlabeled_py, py::arg("n"));
    m.def("_enumerate_series_parallel_labeled", &enumerate_series_parallel_py, py::arg("n"));
    m.def("_enumerate_series_parallel_unlabeled", &enumerate_series_parallel_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_split_labeled", &enumerate_split_py, py::arg("n"));
    m.def("_enumerate_split_unlabeled", &enumerate_split_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_three_leaf_power_labeled", &enumerate_three_leaf_power_py, py::arg("n"));
    m.def("_enumerate_three_leaf_power_unlabeled", &enumerate_three_leaf_power_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_threshold_unlabeled", &enumerate_threshold_py, py::arg("n"));
    m.def("_enumerate_cubic_planar_unlabeled", &enumerate_cubic_planar_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_cubic_unlabeled", &enumerate_cubic_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_eulerian_unlabeled", &enumerate_eulerian_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_triangle_free_unlabeled", &enumerate_triangle_free_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_trivially_perfect_labeled", &enumerate_trivially_perfect_py, py::arg("n"));
    m.def("_enumerate_trivially_perfect_unlabeled", &enumerate_trivially_perfect_unlabeled_py,
          py::arg("n"), py::arg("connected_only") = false);
    m.def("_enumerate_apollonian_unlabeled", &enumerate_apollonian_unlabeled_py, py::arg("n"));
    m.def("_enumerate_at_free_labeled", &enumerate_at_free_py, py::arg("n"));
    m.def("_enumerate_circular_arc_labeled", &enumerate_circular_arc_py, py::arg("n"));
    m.def("_enumerate_co_chordal_labeled", &enumerate_co_chordal_py, py::arg("n"));
    m.def("_enumerate_co_chordal_unlabeled", &enumerate_co_chordal_unlabeled_py, py::arg("n"));
    m.def("_enumerate_co_interval_labeled", &enumerate_co_interval_py, py::arg("n"));
    m.def("_enumerate_co_interval_unlabeled", &enumerate_co_interval_unlabeled_py, py::arg("n"));
    m.def("_enumerate_perfect_labeled", &enumerate_perfect_py, py::arg("n"));
    m.def("_enumerate_strongly_chordal_labeled", &enumerate_strongly_chordal_py, py::arg("n"));
    m.def("_enumerate_trapezoid_labeled", &enumerate_trapezoid_py, py::arg("n"));
    m.def("_enumerate_weakly_chordal_labeled", &enumerate_weakly_chordal_py, py::arg("n"));
}
