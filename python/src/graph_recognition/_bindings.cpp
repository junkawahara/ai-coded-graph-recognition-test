#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "graph.h"
#include "at_free.h"
#include "bipartite.h"
#include "bipartite_permutation.h"
#include "block.h"
#include "cactus.h"
#include "chain.h"
#include "chordal.h"
#include "chordal_bipartite.h"
#include "chordal_enum.h"
#include "circular_arc.h"
#include "co_chordal.h"
#include "co_comparability.h"
#include "co_interval.h"
#include "cochain.h"
#include "cograph.h"
#include "comparability.h"
#include "distance_hereditary.h"
#include "interval.h"
#include "outer_planar.h"
#include "permutation.h"
#include "planar.h"
#include "proper_interval.h"
#include "ptolemaic.h"
#include "quasi_threshold.h"
#include "series_parallel.h"
#include "split.h"
#include "strongly_chordal.h"
#include "threshold.h"
#include "trivially_perfect.h"
#include "unit_interval.h"
#include "weakly_chordal.h"

namespace py = pybind11;
using namespace graph_recognition;

static Graph make_graph(int n, const std::vector<std::pair<int, int>>& edges) {
    return Graph(n, edges);
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
    BipartitePermutationAlgorithm a = BipartitePermutationAlgorithm::CHAIN_BOTH_SIDES;
    if (!algo.empty()) {
        if (algo == "chain_both_sides") a = BipartitePermutationAlgorithm::CHAIN_BOTH_SIDES;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for bipartite_permutation. Valid: 'chain_both_sides'");
    }
    return check_bipartite_permutation(g, a).is_bipartite_permutation;
}

// --- block ---
static bool check_block_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    BlockAlgorithm a = BlockAlgorithm::DFS;
    if (!algo.empty()) {
        if (algo == "dfs") a = BlockAlgorithm::DFS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for block. Valid: 'dfs'");
    }
    return check_block(g, a).is_block;
}

// --- cactus ---
static bool check_cactus_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    CactusAlgorithm a = CactusAlgorithm::DFS;
    if (!algo.empty()) {
        if (algo == "dfs") a = CactusAlgorithm::DFS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for cactus. Valid: 'dfs'");
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
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for chordal. Valid: 'mcs_peo', 'bucket_mcs_peo'");
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
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for cograph. Valid: 'cotree', 'partition_refinement'");
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

// --- distance_hereditary ---
static bool check_distance_hereditary_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    DistanceHereditaryAlgorithm a = DistanceHereditaryAlgorithm::HASH_TWINS;
    if (!algo.empty()) {
        if (algo == "hashmap_twins") a = DistanceHereditaryAlgorithm::HASHMAP_TWINS;
        else if (algo == "sorted_twins") a = DistanceHereditaryAlgorithm::SORTED_TWINS;
        else if (algo == "hash_twins") a = DistanceHereditaryAlgorithm::HASH_TWINS;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for distance_hereditary. Valid: 'hashmap_twins', 'sorted_twins', 'hash_twins'");
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
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for interval. Valid: 'backtracking', 'at_free'");
    }
    return check_interval(g, a).is_interval;
}

// --- outer_planar ---
static bool check_outer_planar_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    OuterPlanarAlgorithm a = OuterPlanarAlgorithm::MINOR_CHECK;
    if (!algo.empty()) {
        if (algo == "minor_check") a = OuterPlanarAlgorithm::MINOR_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for outer_planar. Valid: 'minor_check'");
    }
    return check_outer_planar(g, a).is_outer_planar;
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
    PlanarAlgorithm a = PlanarAlgorithm::MINOR_CHECK;
    if (!algo.empty()) {
        if (algo == "minor_check") a = PlanarAlgorithm::MINOR_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for planar. Valid: 'minor_check'");
    }
    return check_planar(g, a).is_planar;
}

// --- proper_interval ---
static bool check_proper_interval_py(int n, const std::vector<std::pair<int, int>>& edges, const std::string& algo) {
    Graph g = make_graph(n, edges);
    ProperIntervalAlgorithm a = ProperIntervalAlgorithm::FAST_CLAW_CHECK;
    if (!algo.empty()) {
        if (algo == "pq_tree") a = ProperIntervalAlgorithm::PQ_TREE;
        else if (algo == "fast_claw_check") a = ProperIntervalAlgorithm::FAST_CLAW_CHECK;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for proper_interval. Valid: 'pq_tree', 'fast_claw_check'");
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
    StronglyChordalAlgorithm a = StronglyChordalAlgorithm::PEO_MATRIX;
    if (!algo.empty()) {
        if (algo == "strong_elimination") a = StronglyChordalAlgorithm::STRONG_ELIMINATION;
        else if (algo == "peo_matrix") a = StronglyChordalAlgorithm::PEO_MATRIX;
        else throw std::invalid_argument("Unknown algorithm '" + algo + "' for strongly_chordal. Valid: 'strong_elimination', 'peo_matrix'");
    }
    return check_strongly_chordal(g, a).is_strongly_chordal;
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

// --- chordal_enum (special: enumeration, not recognition) ---
static std::vector<std::pair<int, std::vector<std::pair<int, int>>>>
enumerate_chordal_py(int n) {
    ChordalEnumerationResult result = enumerate_chordal_graphs_reverse_search(n);
    std::vector<std::pair<int, std::vector<std::pair<int, int>>>> out;
    out.reserve(result.graphs.size());
    for (size_t i = 0; i < result.graphs.size(); ++i) {
        out.push_back(std::make_pair(result.graphs[i].n, result.graphs[i].edges));
    }
    return out;
}

PYBIND11_MODULE(_core, m) {
    m.doc() = "C++ graph recognition bindings";

    m.def("_check_at_free", &check_at_free_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_bipartite", &check_bipartite_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_bipartite_permutation", &check_bipartite_permutation_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_block", &check_block_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_cactus", &check_cactus_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_chain", &check_chain_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_chordal", &check_chordal_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_chordal_bipartite", &check_chordal_bipartite_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_circular_arc", &check_circular_arc_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_co_chordal", &check_co_chordal_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_co_comparability", &check_co_comparability_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_co_interval", &check_co_interval_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_cochain", &check_cochain_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_cograph", &check_cograph_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_comparability", &check_comparability_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_distance_hereditary", &check_distance_hereditary_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_interval", &check_interval_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_outer_planar", &check_outer_planar_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_permutation", &check_permutation_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_planar", &check_planar_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_proper_interval", &check_proper_interval_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_ptolemaic", &check_ptolemaic_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_quasi_threshold", &check_quasi_threshold_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_series_parallel", &check_series_parallel_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_split", &check_split_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_strongly_chordal", &check_strongly_chordal_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_threshold", &check_threshold_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_trivially_perfect", &check_trivially_perfect_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_unit_interval", &check_unit_interval_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");
    m.def("_check_weakly_chordal", &check_weakly_chordal_py, py::arg("n"), py::arg("edges"), py::arg("algo") = "");

    m.def("_enumerate_chordal", &enumerate_chordal_py, py::arg("n"));
}
