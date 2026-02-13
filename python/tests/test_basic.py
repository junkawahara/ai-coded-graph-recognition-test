"""Basic tests for all 31 graph classes: at least one YES and one NO instance each."""

from graph_recognition import (
    is_at_free,
    is_bipartite,
    is_bipartite_permutation,
    is_block,
    is_cactus,
    is_chain,
    is_chordal,
    is_chordal_bipartite,
    is_circular_arc,
    is_co_chordal,
    is_co_comparability,
    is_co_interval,
    is_cochain,
    is_cograph,
    is_comparability,
    is_distance_hereditary,
    is_interval,
    is_outer_planar,
    is_permutation,
    is_planar,
    is_proper_interval,
    is_ptolemaic,
    is_quasi_threshold,
    is_series_parallel,
    is_split,
    is_strongly_chordal,
    is_threshold,
    is_trivially_perfect,
    is_unit_interval,
    is_weakly_chordal,
)


class TestInterval:
    def test_path_yes(self, path_4):
        assert is_interval(*path_4) is True

    def test_cycle_4_no(self, cycle_4):
        assert is_interval(*cycle_4) is False

    def test_complete_yes(self, complete_4):
        assert is_interval(*complete_4) is True

    def test_empty_yes(self, empty_3):
        assert is_interval(*empty_3) is True


class TestChordal:
    def test_complete_yes(self, complete_4):
        assert is_chordal(*complete_4) is True

    def test_cycle_4_no(self, cycle_4):
        assert is_chordal(*cycle_4) is False

    def test_path_yes(self, path_4):
        assert is_chordal(*path_4) is True


class TestBipartite:
    def test_path_yes(self, path_4):
        assert is_bipartite(*path_4) is True

    def test_cycle_4_yes(self, cycle_4):
        assert is_bipartite(*cycle_4) is True

    def test_k3_no(self, complete_3):
        assert is_bipartite(*complete_3) is False


class TestCograph:
    def test_complete_yes(self, complete_4):
        assert is_cograph(*complete_4) is True

    def test_p4_no(self, path_4):
        assert is_cograph(*path_4) is False

    def test_empty_yes(self, empty_3):
        assert is_cograph(*empty_3) is True


class TestSplit:
    def test_star_yes(self, star_4):
        assert is_split(*star_4) is True

    def test_cycle_5_no(self, cycle_5):
        assert is_split(*cycle_5) is False

    def test_complete_yes(self, complete_4):
        assert is_split(*complete_4) is True


class TestThreshold:
    def test_star_yes(self, star_4):
        assert is_threshold(*star_4) is True

    def test_p4_no(self, path_4):
        assert is_threshold(*path_4) is False

    def test_complete_yes(self, complete_3):
        assert is_threshold(*complete_3) is True


class TestComparability:
    def test_complete_yes(self, complete_4):
        assert is_comparability(*complete_4) is True

    def test_bipartite_yes(self, cycle_4):
        assert is_comparability(*cycle_4) is True

    def test_c5_no(self, cycle_5):
        assert is_comparability(*cycle_5) is False


class TestPermutation:
    def test_path_yes(self, path_4):
        assert is_permutation(*path_4) is True

    def test_complete_yes(self, complete_3):
        assert is_permutation(*complete_3) is True


class TestProperInterval:
    def test_path_yes(self, path_4):
        assert is_proper_interval(*path_4) is True

    def test_claw_no(self, claw):
        assert is_proper_interval(*claw) is False


class TestUnitInterval:
    def test_path_yes(self, path_4):
        assert is_unit_interval(*path_4) is True

    def test_claw_no(self, claw):
        assert is_unit_interval(*claw) is False


class TestBlock:
    def test_complete_yes(self, complete_4):
        assert is_block(*complete_4) is True

    def test_cycle_4_no(self, cycle_4):
        assert is_block(*cycle_4) is False

    def test_tree_yes(self, path_4):
        assert is_block(*path_4) is True


class TestCactus:
    def test_tree_yes(self, path_4):
        assert is_cactus(*path_4) is True

    def test_cycle_yes(self, cycle_5):
        assert is_cactus(*cycle_5) is True

    def test_k4_no(self, complete_4):
        assert is_cactus(*complete_4) is False


class TestDistanceHereditary:
    def test_tree_yes(self, path_4):
        assert is_distance_hereditary(*path_4) is True

    def test_cograph_yes(self, complete_3):
        assert is_distance_hereditary(*complete_3) is True

    def test_cycle_5_no(self, cycle_5):
        assert is_distance_hereditary(*cycle_5) is False


class TestPtolemaic:
    def test_complete_yes(self, complete_4):
        assert is_ptolemaic(*complete_4) is True

    def test_cycle_4_no(self, cycle_4):
        assert is_ptolemaic(*cycle_4) is False

    def test_tree_yes(self, path_4):
        assert is_ptolemaic(*path_4) is True


class TestTriviallyPerfect:
    def test_star_yes(self, star_4):
        assert is_trivially_perfect(*star_4) is True

    def test_p4_no(self, path_4):
        assert is_trivially_perfect(*path_4) is False

    def test_complete_yes(self, complete_3):
        assert is_trivially_perfect(*complete_3) is True


class TestQuasiThreshold:
    def test_star_yes(self, star_4):
        assert is_quasi_threshold(*star_4) is True

    def test_p4_no(self, path_4):
        assert is_quasi_threshold(*path_4) is False


class TestATFree:
    def test_path_yes(self, path_5):
        assert is_at_free(*path_5) is True

    def test_complete_yes(self, complete_4):
        assert is_at_free(*complete_4) is True


class TestPlanar:
    def test_k4_yes(self, complete_4):
        assert is_planar(*complete_4) is True

    def test_k5_no(self, complete_5):
        assert is_planar(*complete_5) is False

    def test_path_yes(self, path_4):
        assert is_planar(*path_4) is True


class TestOuterPlanar:
    def test_path_yes(self, path_4):
        assert is_outer_planar(*path_4) is True

    def test_k4_no(self, complete_4):
        assert is_outer_planar(*complete_4) is False

    def test_cycle_yes(self, cycle_5):
        assert is_outer_planar(*cycle_5) is True


class TestSeriesParallel:
    def test_path_yes(self, path_4):
        assert is_series_parallel(*path_4) is True

    def test_k4_no(self, complete_4):
        assert is_series_parallel(*complete_4) is False


class TestChain:
    def test_complete_bipartite_yes(self, complete_bipartite_2_3):
        assert is_chain(*complete_bipartite_2_3) is True

    def test_cycle_6_no(self, cycle_6):
        assert is_chain(*cycle_6) is False


class TestCochain:
    def test_complete_yes(self, complete_3):
        assert is_cochain(*complete_3) is True

    def test_path_yes(self):
        assert is_cochain(3, [(1, 2), (2, 3)]) is True

    def test_empty_3_no(self, empty_3):
        # Complement of empty_3 is K3, which is not bipartite, so not chain
        assert is_cochain(*empty_3) is False


class TestChordalBipartite:
    def test_path_yes(self, path_4):
        assert is_chordal_bipartite(*path_4) is True

    def test_cycle_6_no(self, cycle_6):
        assert is_chordal_bipartite(*cycle_6) is False


class TestCircularArc:
    def test_cycle_yes(self, cycle_5):
        assert is_circular_arc(*cycle_5) is True

    def test_complete_yes(self, complete_4):
        assert is_circular_arc(*complete_4) is True


class TestCoChordal:
    def test_complete_yes(self, complete_4):
        assert is_co_chordal(*complete_4) is True

    def test_empty_yes(self, empty_3):
        assert is_co_chordal(*empty_3) is True


class TestCoComparability:
    def test_complete_yes(self, complete_4):
        assert is_co_comparability(*complete_4) is True

    def test_path_yes(self, path_4):
        assert is_co_comparability(*path_4) is True


class TestCoInterval:
    def test_complete_yes(self, complete_4):
        assert is_co_interval(*complete_4) is True

    def test_empty_yes(self, empty_3):
        assert is_co_interval(*empty_3) is True


class TestBipartitePermutation:
    def test_path_yes(self, path_4):
        assert is_bipartite_permutation(*path_4) is True

    def test_k3_no(self, complete_3):
        assert is_bipartite_permutation(*complete_3) is False


class TestStronglyChordal:
    def test_complete_yes(self, complete_4):
        assert is_strongly_chordal(*complete_4) is True

    def test_cycle_4_no(self, cycle_4):
        assert is_strongly_chordal(*cycle_4) is False


class TestWeaklyChordal:
    def test_complete_yes(self, complete_4):
        assert is_weakly_chordal(*complete_4) is True

    def test_cycle_5_no(self, cycle_5):
        assert is_weakly_chordal(*cycle_5) is False

    def test_path_yes(self, path_4):
        assert is_weakly_chordal(*path_4) is True


class TestEnumerateChordal:
    def test_n1(self):
        from graph_recognition import enumerate_chordal_graphs
        result = enumerate_chordal_graphs(1)
        assert len(result) == 1  # single vertex

    def test_n2(self):
        from graph_recognition import enumerate_chordal_graphs
        result = enumerate_chordal_graphs(2)
        assert len(result) == 2  # empty + edge

    def test_n3(self):
        from graph_recognition import enumerate_chordal_graphs
        result = enumerate_chordal_graphs(3)
        assert len(result) == 8  # 2^3 = 8 (all graphs on 3 vertices are chordal)
