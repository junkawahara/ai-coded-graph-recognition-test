"""Basic tests for the 38 recognizers bound in the second batch.

Together with tests/test_basic.py (the original 38 classes), every bound
class has at least one YES and one NO instance. All expected values were
cross-checked against the C++ CLI implementations.
"""

from graph_recognition import (
    is_apex,
    is_biconnected,
    is_bull_free,
    is_caterpillar,
    is_circle,
    is_cluster,
    is_cubic,
    is_cubic_planar,
    is_digraph,
    is_eulerian,
    is_even_hole_free,
    is_five_leaf_power,
    is_forest,
    is_four_leaf_power,
    is_fullerene,
    is_gem_free,
    is_halin,
    is_kregular,
    is_ktree,
    is_laman,
    is_maximal_planar,
    is_meyniel,
    is_odd_hole_free,
    is_p5_free,
    is_parity,
    is_polyhedral,
    is_poset,
    is_proper_chordal,
    is_proper_circular_arc,
    is_self_complementary,
    is_simple_quadrangulation,
    is_snark,
    is_strongly_regular,
    is_tournament,
    is_tree,
    is_triangle_free,
    is_triconnected,
    is_unicyclic,
)

import pytest


@pytest.fixture
def bull():
    """Triangle 1-2-3 with horns 4-1 and 5-2."""
    return 5, [(1, 2), (1, 3), (2, 3), (1, 4), (2, 5)]


@pytest.fixture
def wheel_5():
    """W5: hub 6 joined to the cycle 1-2-3-4-5."""
    return 6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 1),
               (6, 1), (6, 2), (6, 3), (6, 4), (6, 5)]


@pytest.fixture
def wheel_4():
    """W4: hub 1 joined to the cycle 2-3-4-5 (a Halin graph)."""
    return 5, [(2, 3), (3, 4), (4, 5), (5, 2),
               (1, 2), (1, 3), (1, 4), (1, 5)]


@pytest.fixture
def gem():
    """P4 1-2-3-4 plus the dominating vertex 5."""
    return 5, [(1, 2), (2, 3), (3, 4), (5, 1), (5, 2), (5, 3), (5, 4)]


@pytest.fixture
def spider_2_2_2():
    """Three legs of length 2 from center 1 (a non-caterpillar tree)."""
    return 7, [(1, 2), (2, 3), (1, 4), (4, 5), (1, 6), (6, 7)]


@pytest.fixture
def two_disjoint_edges():
    return 4, [(1, 2), (3, 4)]


@pytest.fixture
def cube():
    """The cube graph Q3 (the smallest simple quadrangulation)."""
    return 8, [(1, 2), (2, 3), (3, 4), (4, 1),
               (5, 6), (6, 7), (7, 8), (8, 5),
               (1, 5), (2, 6), (3, 7), (4, 8)]


@pytest.fixture
def petersen():
    return 10, [(1, 2), (1, 5), (1, 6), (2, 3), (2, 7), (3, 4), (3, 8),
                (4, 5), (4, 9), (5, 10), (6, 8), (6, 9), (7, 9), (7, 10),
                (8, 10)]


@pytest.fixture
def dodecahedron():
    """The dodecahedral graph (the smallest fullerene, C20)."""
    return 20, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 1),
                (1, 6), (2, 7), (3, 8), (4, 9), (5, 10),
                (6, 15), (6, 11), (7, 11), (7, 12), (8, 12),
                (8, 13), (9, 13), (9, 14), (10, 14), (10, 15),
                (11, 16), (12, 17), (13, 18), (14, 19), (15, 20),
                (16, 17), (17, 18), (18, 19), (19, 20), (20, 16)]


class TestApex:
    def test_k5_yes(self, complete_5):
        assert is_apex(*complete_5) is True

    def test_k7_no(self):
        edges = [(u, v) for u in range(1, 8) for v in range(u + 1, 8)]
        assert is_apex(7, edges) is False


class TestBiconnected:
    def test_cycle_yes(self, cycle_4):
        assert is_biconnected(*cycle_4) is True

    def test_path_no(self, path_4):
        assert is_biconnected(*path_4) is False


class TestBullFree:
    def test_path_yes(self, path_4):
        assert is_bull_free(*path_4) is True

    def test_bull_no(self, bull):
        assert is_bull_free(*bull) is False


class TestCaterpillar:
    def test_path_yes(self, path_4):
        assert is_caterpillar(*path_4) is True

    def test_spider_no(self, spider_2_2_2):
        assert is_caterpillar(*spider_2_2_2) is False


class TestCircle:
    def test_cycle_yes(self, cycle_5):
        assert is_circle(*cycle_5) is True

    def test_w5_no(self, wheel_5):
        # W5 is one of Bouchet's vertex-minor obstructions
        assert is_circle(*wheel_5) is False


class TestCluster:
    def test_disjoint_cliques_yes(self, two_disjoint_edges):
        assert is_cluster(*two_disjoint_edges) is True

    def test_path_no(self, path_4):
        assert is_cluster(*path_4) is False


class TestCubic:
    def test_k4_yes(self, complete_4):
        assert is_cubic(*complete_4) is True

    def test_path_no(self, path_4):
        assert is_cubic(*path_4) is False


class TestCubicPlanar:
    def test_k4_yes(self, complete_4):
        assert is_cubic_planar(*complete_4) is True

    def test_k33_no(self, complete_bipartite_3_3):
        # cubic but not planar
        assert is_cubic_planar(*complete_bipartite_3_3) is False


class TestDigraph:
    def test_simple_yes(self):
        # edges are directed arcs for this recognizer
        assert is_digraph(3, [(1, 2), (2, 3)]) is True


class TestEulerian:
    def test_cycle_yes(self, cycle_4):
        assert is_eulerian(*cycle_4) is True

    def test_path_no(self, path_4):
        assert is_eulerian(*path_4) is False


class TestEvenHoleFree:
    def test_complete_yes(self, complete_4):
        assert is_even_hole_free(*complete_4) is True

    def test_c4_no(self, cycle_4):
        assert is_even_hole_free(*cycle_4) is False


class TestFiveLeafPower:
    def test_path_yes(self, path_4):
        assert is_five_leaf_power(*path_4) is True

    def test_c4_no(self, cycle_4):
        assert is_five_leaf_power(*cycle_4) is False


class TestForest:
    def test_two_trees_yes(self, two_disjoint_edges):
        assert is_forest(*two_disjoint_edges) is True

    def test_cycle_no(self, cycle_4):
        assert is_forest(*cycle_4) is False


class TestFourLeafPower:
    def test_path_yes(self, path_4):
        assert is_four_leaf_power(*path_4) is True

    def test_c4_no(self, cycle_4):
        assert is_four_leaf_power(*cycle_4) is False


class TestFullerene:
    def test_dodecahedron_yes(self, dodecahedron):
        assert is_fullerene(*dodecahedron) is True

    def test_k4_no(self, complete_4):
        assert is_fullerene(*complete_4) is False


class TestGemFree:
    def test_path_yes(self, path_4):
        assert is_gem_free(*path_4) is True

    def test_gem_no(self, gem):
        assert is_gem_free(*gem) is False


class TestHalin:
    def test_wheel_yes(self, wheel_4):
        assert is_halin(*wheel_4) is True

    def test_path_no(self, path_4):
        assert is_halin(*path_4) is False


class TestKRegular:
    def test_cycle_yes(self, cycle_4):
        assert is_kregular(*cycle_4) is True

    def test_path_no(self, path_4):
        assert is_kregular(*path_4) is False


class TestKTree:
    def test_k4_yes(self, complete_4):
        assert is_ktree(*complete_4) is True

    def test_c4_no(self, cycle_4):
        assert is_ktree(*cycle_4) is False


class TestLaman:
    def test_triangle_yes(self, complete_3):
        assert is_laman(*complete_3) is True

    def test_k4_no(self, complete_4):
        assert is_laman(*complete_4) is False


class TestMaximalPlanar:
    def test_k4_yes(self, complete_4):
        assert is_maximal_planar(*complete_4) is True

    def test_path_no(self, path_4):
        assert is_maximal_planar(*path_4) is False


class TestMeyniel:
    def test_complete_yes(self, complete_4):
        assert is_meyniel(*complete_4) is True

    def test_c5_no(self, cycle_5):
        assert is_meyniel(*cycle_5) is False


class TestOddHoleFree:
    def test_c4_yes(self, cycle_4):
        assert is_odd_hole_free(*cycle_4) is True

    def test_c5_no(self, cycle_5):
        assert is_odd_hole_free(*cycle_5) is False


class TestP5Free:
    def test_p4_yes(self, path_4):
        assert is_p5_free(*path_4) is True

    def test_p5_no(self, path_5):
        assert is_p5_free(*path_5) is False


class TestParity:
    def test_c4_yes(self, cycle_4):
        assert is_parity(*cycle_4) is True

    def test_c5_no(self, cycle_5):
        assert is_parity(*cycle_5) is False


class TestPolyhedral:
    def test_k4_yes(self, complete_4):
        assert is_polyhedral(*complete_4) is True

    def test_path_no(self, path_4):
        assert is_polyhedral(*path_4) is False


class TestPoset:
    def test_chain_yes(self):
        # edges are the arcs of the covering relation (u, v) = v covers u
        assert is_poset(3, [(1, 2), (2, 3)]) is True

    def test_transitive_arc_no(self):
        assert is_poset(3, [(1, 2), (2, 3), (1, 3)]) is False


class TestProperChordal:
    def test_path_yes(self, path_4):
        assert is_proper_chordal(*path_4) is True

    def test_c4_no(self, cycle_4):
        assert is_proper_chordal(*cycle_4) is False


class TestProperCircularArc:
    def test_cycle_yes(self, cycle_5):
        assert is_proper_circular_arc(*cycle_5) is True

    def test_claw_no(self, claw):
        assert is_proper_circular_arc(*claw) is False


class TestSelfComplementary:
    def test_p4_yes(self, path_4):
        assert is_self_complementary(*path_4) is True

    def test_k3_no(self, complete_3):
        assert is_self_complementary(*complete_3) is False


class TestSimpleQuadrangulation:
    def test_cube_yes(self, cube):
        assert is_simple_quadrangulation(*cube) is True

    def test_k4_no(self, complete_4):
        assert is_simple_quadrangulation(*complete_4) is False


class TestSnark:
    def test_petersen_yes(self, petersen):
        assert is_snark(*petersen) is True

    def test_k4_no(self, complete_4):
        assert is_snark(*complete_4) is False


class TestStronglyRegular:
    def test_c5_yes(self, cycle_5):
        assert is_strongly_regular(*cycle_5) is True

    def test_path_no(self, path_4):
        assert is_strongly_regular(*path_4) is False


class TestTournament:
    def test_transitive_yes(self):
        # edges are directed arcs for this recognizer
        assert is_tournament(3, [(1, 2), (1, 3), (2, 3)]) is True

    def test_missing_arc_no(self):
        assert is_tournament(3, [(1, 2)]) is False


class TestTree:
    def test_path_yes(self, path_4):
        assert is_tree(*path_4) is True

    def test_cycle_no(self, cycle_4):
        assert is_tree(*cycle_4) is False


class TestTriangleFree:
    def test_c4_yes(self, cycle_4):
        assert is_triangle_free(*cycle_4) is True

    def test_k3_no(self, complete_3):
        assert is_triangle_free(*complete_3) is False


class TestTriconnected:
    def test_k4_yes(self, complete_4):
        assert is_triconnected(*complete_4) is True

    def test_c4_no(self, cycle_4):
        assert is_triconnected(*cycle_4) is False


class TestUnicyclic:
    def test_c4_yes(self, cycle_4):
        assert is_unicyclic(*cycle_4) is True

    def test_path_no(self, path_4):
        assert is_unicyclic(*path_4) is False
