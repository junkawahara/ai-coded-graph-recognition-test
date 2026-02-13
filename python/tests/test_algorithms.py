"""Tests for algorithm selection."""

from graph_recognition import (
    is_chain,
    is_chordal,
    is_chordal_bipartite,
    is_circular_arc,
    is_cochain,
    is_cograph,
    is_distance_hereditary,
    is_interval,
    is_permutation,
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


class TestChordalAlgorithms:
    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_chordal, *complete_4, ["mcs_peo", "bucket_mcs_peo"])

    def test_cycle_agree(self, cycle_4):
        _assert_algorithms_agree(is_chordal, *cycle_4, ["mcs_peo", "bucket_mcs_peo"])


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
                                 ["pq_tree", "fast_claw_check"])


class TestStronglyChordalAlgorithms:
    def test_complete_agree(self, complete_4):
        _assert_algorithms_agree(is_strongly_chordal, *complete_4,
                                 ["strong_elimination", "peo_matrix"])


class TestChordalBipartiteAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_chordal_bipartite, *path_4,
                                 ["cycle_check", "bisimplicial", "fast_bisimplicial"])


class TestCircularArcAlgorithms:
    def test_cycle_agree(self, cycle_5):
        _assert_algorithms_agree(is_circular_arc, *cycle_5,
                                 ["mcconnell", "backtracking"])


class TestWeaklyChordalAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_weakly_chordal, *path_4,
                                 ["co_chordal_bipartite", "complement_bfs"])


class TestSeriesParallelAlgorithms:
    def test_path_agree(self, path_4):
        _assert_algorithms_agree(is_series_parallel, *path_4,
                                 ["minor_check", "queue_reduction"])


class TestPtolemaicAlgorithms:
    def test_tree_agree(self, path_4):
        _assert_algorithms_agree(is_ptolemaic, *path_4,
                                 ["dh_hashmap", "dh_sorted"])
