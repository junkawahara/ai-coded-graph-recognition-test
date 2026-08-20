"""Tests for algorithm selection."""

from graph_recognition import (
    is_biconvex_bipartite,
    is_block,
    is_chain,
    is_chordal,
    is_chordal_bipartite,
    is_circular_arc,
    is_claw_free,
    is_cochain,
    is_cograph,
    is_convex_bipartite,
    is_diamond_free,
    is_distance_hereditary,
    is_interval,
    is_line_graph,
    is_outer_planar,
    is_permutation,
    is_planar,
    is_proper_interval,
    is_ptolemaic,
    is_series_parallel,
    is_split,
    is_strongly_chordal,
    is_threshold,
    is_weakly_chordal,
)


def _assert_algorithms_agree(func, n, edges, algorithms):
    """All algorithms should agree on the result for a given graph."""
    results = {algo: func(n, edges, algorithm=algo) for algo in algorithms}
    values = list(results.values())
    assert all(v == values[0] for v in values), \
        "Algorithms disagree: {}".format(results)


class TestIntervalAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_interval, *path_4, ["backtracking", "at_free"])

    def test_cycle_agree(self, cycle_4):
        _assert_algorithms_agree(is_interval, *cycle_4, ["backtracking", "at_free"])


class TestBlockAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_block, *path_4, ["dfs", "chordal_diamond_free"])

    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_block, *complete_4, ["dfs", "chordal_diamond_free"])


class TestChordalAlgorithms:
    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_chordal, *complete_4,
                                 ["mcs_peo", "bucket_mcs_peo", "lexbfs_peo"])

    def test_cycle_agree(self, cycle_4):
        _assert_algorithms_agree(is_chordal, *cycle_4,
                                 ["mcs_peo", "bucket_mcs_peo", "lexbfs_peo"])


class TestPermutationAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_permutation, *path_4, ["backtracking", "class_based"])


class TestCographAlgorithms:
    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_cograph, *complete_4, ["cotree", "partition_refinement"])

    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_cograph, *path_4, ["cotree", "partition_refinement"])


class TestSplitAlgorithms:
    def test_star_agree(self, star_4):
        _assert_algorithms_agree(is_split, *star_4, ["degree_sequence", "hammer_simeone"])


class TestThresholdAlgorithms:
    def test_star_agree(self, star_4):
        _assert_algorithms_agree(is_threshold, *star_4, ["degree_sequence", "degree_sequence_fast"])


class TestChainAlgorithms:
    def test_complete_bipartite_agree(self, complete_bipartite_2_3):
        _assert_algorithms_agree(is_chain, *complete_bipartite_2_3,
                                 ["neighborhood_inclusion", "degree_sort"])


class TestCochainAlgorithms:
    def test_complete_agree(self, complete_3):
        _assert_algorithms_agree(is_cochain, *complete_3, ["complement", "direct"])


class TestDistanceHereditaryAlgorithms:
    def test_tree_agree(self, path_4):
        _assert_algorithms_agree(is_distance_hereditary, *path_4,
                                 ["hashmap_twins", "sorted_twins", "hash_twins"])


class TestProperIntervalAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_proper_interval, *path_4,
                                 ["triple_loop_claw_check", "fast_claw_check"])


class TestStronglyChordalAlgorithms:
    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_strongly_chordal, *complete_4,
                                 ["strong_elimination", "peo_matrix", "mcs_seo"])


class TestChordalBipartiteAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_chordal_bipartite, *path_4,
                                 ["cycle_check", "bisimplicial", "fast_bisimplicial"])


class TestCircularArcAlgorithms:
    def test_cycle_agree(self, cycle_5):
        _assert_algorithms_agree(is_circular_arc, *cycle_5,
                                 ["mcconnell", "backtracking"])

    def test_non_helly_agree(self, triangle_3_pendants):
        """Non-Helly circular-arc: triangle with 3 pendants.
        This graph IS circular-arc but NOT Helly circular-arc."""
        _assert_algorithms_agree(is_circular_arc, *triangle_3_pendants,
                                 ["mcconnell", "backtracking"])


class TestWeaklyChordalAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_weakly_chordal, *path_4,
                                 ["co_chordal_bipartite", "complement_bfs"])


class TestSeriesParallelAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_series_parallel, *path_4,
                                 ["minor_check", "queue_reduction"])


class TestPlanarAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_planar, *path_4,
                                 ["left_right", "minor_check"])

    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_planar, *complete_4,
                                 ["left_right", "minor_check"])

    def test_k5_agree(self):
        edges = [(u, v) for u in range(1, 6) for v in range(u + 1, 6)]
        _assert_algorithms_agree(is_planar, 5, edges,
                                 ["left_right", "minor_check"])


class TestOuterPlanarAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_outer_planar, *path_4,
                                 ["minor_check", "augmented_planarity"])

    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_outer_planar, *complete_4,
                                 ["minor_check", "augmented_planarity"])


class TestPtolemaicAlgorithms:
    def test_tree_agree(self, path_4):
        _assert_algorithms_agree(is_ptolemaic, *path_4,
                                 ["dh_hashmap", "dh_sorted"])


class TestBiconvexBipartiteAlgorithms:
    def test_complete_bipartite_agree(self, complete_bipartite_2_3):
        _assert_algorithms_agree(is_biconvex_bipartite, *complete_bipartite_2_3,
                                 ["brute_force", "c1p"])


class TestConvexBipartiteAlgorithms:
    def test_complete_bipartite_agree(self, complete_bipartite_2_3):
        _assert_algorithms_agree(is_convex_bipartite, *complete_bipartite_2_3,
                                 ["brute_force", "c1p"])


class TestClawFreeAlgorithms:
    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_claw_free, *complete_4,
                                 ["triple_loop", "edge_count"])

    def test_star_agree(self, star_4):
        _assert_algorithms_agree(is_claw_free, *star_4,
                                 ["triple_loop", "edge_count"])


class TestDiamondFreeAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_diamond_free, *path_4,
                                 ["brute", "edge_pair"])

    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_diamond_free, *complete_4,
                                 ["brute", "edge_pair"])


class TestLineGraphAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_line_graph, *path_4,
                                 ["brute", "krausz"])

    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_line_graph, *complete_4,
                                 ["brute", "krausz"])
