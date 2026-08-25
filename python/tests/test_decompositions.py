"""Tests for the decomposition functions."""

import pytest

import graph_recognition as gr
from graph_recognition import DECOMPOSITIONS


P4 = (4, [(1, 2), (2, 3), (3, 4)])
C4 = (4, [(1, 2), (2, 3), (3, 4), (4, 1)])
C5 = (5, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 1)])
K4 = (4, [(1, 2), (1, 3), (1, 4), (2, 3), (2, 4), (3, 4)])
STAR4 = (5, [(1, 2), (1, 3), (1, 4), (1, 5)])


def test_all_decompositions_are_exported():
    for name in DECOMPOSITIONS:
        assert hasattr(gr, name), name
        assert name in gr.__all__


class TestComponents:
    def test_components_split(self):
        assert gr.connected_components(5, [(1, 2), (3, 4)]) == [[1, 2], [3, 4], [5]]

    def test_co_components_match_the_complement(self):
        # The complement of P4 is P4 again, so it stays connected.
        assert len(gr.co_components(*P4)) == 1
        # The complement of a star has the centre isolated.
        assert len(gr.co_components(*STAR4)) == 2


class TestTwinQuotient:
    def test_star_leaves_are_false_twins(self):
        r = gr.twin_quotient(*STAR4, kind="false")
        assert sorted(len(m) for m in r["members"]) == [1, 4]
        assert r["quotient_edges"] == [(1, 2)]

    def test_true_twins_of_a_complete_graph_collapse(self):
        r = gr.twin_quotient(*K4, kind="true")
        assert r["members"] == [[1, 2, 3, 4]]

    def test_unknown_kind_rejected(self):
        with pytest.raises(ValueError):
            gr.twin_quotient(*P4, kind="nonsense")


class TestBlockCutTree:
    def test_path_blocks_are_its_edges(self):
        r = gr.block_cut_tree(*P4)
        assert len(r["blocks"]) == 3
        assert r["cut_vertices"] == [2, 3]
        assert sorted(r["bridges"]) == [(1, 2), (2, 3), (3, 4)]

    def test_cycle_is_one_block(self):
        r = gr.block_cut_tree(*C4)
        assert len(r["blocks"]) == 1
        assert r["cut_vertices"] == []
        assert r["bridges"] == []


class TestModularDecomposition:
    def test_p4_is_prime(self):
        t = gr.modular_decomposition(*P4)
        assert t["nodes"][t["root"]]["kind"] == "prime"

    def test_complete_graph_is_one_series_node(self):
        t = gr.modular_decomposition(*K4)
        root = t["nodes"][t["root"]]
        assert root["kind"] == "series"
        assert root["vertices"] == [1, 2, 3, 4]

    def test_empty_graph_has_no_tree(self):
        t = gr.modular_decomposition(0, [])
        assert t["root"] == -1
        assert t["nodes"] == []


class TestCotree:
    def test_p4_has_no_cotree(self):
        t = gr.cotree(*P4)
        assert t["is_cograph"] is False
        assert t["nodes"] == []

    @pytest.mark.parametrize("algo", ["cotree", "partition_refinement", "modular"])
    def test_algorithms_agree(self, algo):
        assert gr.cotree(*K4, algorithm=algo)["is_cograph"] is True
        assert gr.cotree(*P4, algorithm=algo)["is_cograph"] is False

    def test_unknown_algorithm_rejected(self):
        with pytest.raises(ValueError):
            gr.cotree(*P4, algorithm="nope")


class TestOrientationAndRealizer:
    def test_path_is_a_comparability_graph(self):
        r = gr.transitive_orientation(*P4)
        assert r["is_comparability"] is True
        assert len(r["orientation"]) == 3
        # Every edge is oriented exactly once.
        undirected = {tuple(sorted(e)) for e in r["orientation"]}
        assert undirected == {(1, 2), (2, 3), (3, 4)}

    def test_odd_cycle_is_not(self):
        assert gr.transitive_orientation(*C5)["is_comparability"] is False

    def test_realizer_describes_the_graph(self):
        r = gr.permutation_realizer(*P4)
        assert r["is_permutation"] is True
        n, edges = P4
        edge_set = {tuple(sorted(e)) for e in edges}
        for u in range(1, n + 1):
            for v in range(u + 1, n + 1):
                crossing = (r["pos1"][u] < r["pos1"][v]) != (r["pos2"][u] < r["pos2"][v])
                assert crossing == ((u, v) in edge_set)

    def test_realizer_fails_on_a_non_permutation_graph(self):
        assert gr.permutation_realizer(*C5)["is_permutation"] is False


class TestCliqueAndTreeDecomposition:
    def test_path_has_width_one(self):
        r = gr.tree_decomposition(*P4)
        assert r["success"] is True
        assert r["width"] == 1
        assert len(r["bags"]) == 3

    def test_complete_graph_is_one_bag(self):
        r = gr.tree_decomposition(*K4)
        assert r["width"] == 3
        assert len(r["bags"]) == 1

    def test_non_chordal_is_rejected(self):
        assert gr.tree_decomposition(*C4)["success"] is False
        assert gr.clique_tree(*C4)["is_chordal"] is False

    def test_clique_tree_of_a_path(self):
        r = gr.clique_tree(*P4)
        assert r["is_chordal"] is True
        assert len(r["cliques"]) == 3
        assert len(r["tree_edges"]) == 2


class TestSplitDecomposition:
    def test_path_is_totally_decomposable(self):
        r = gr.split_decomposition(*P4)
        assert r["success"] is True
        assert r["totally_decomposable"] is True
        assert all(bag["kind"] in ("clique", "star") for bag in r["bags"])

    def test_five_cycle_is_one_prime_bag(self):
        r = gr.split_decomposition(*C5)
        assert r["success"] is True
        assert r["totally_decomposable"] is False
        assert [bag["kind"] for bag in r["bags"]] == ["prime"]

    def test_disconnected_input_is_rejected(self):
        assert gr.split_decomposition(4, [(1, 2), (3, 4)])["success"] is False


class TestSPQRTree:
    def test_cycle_is_one_polygon(self):
        r = gr.spqr_tree(*C5)
        assert r["success"] is True
        assert [node["kind"] for node in r["nodes"]] == ["S"]

    def test_complete_graph_is_rigid(self):
        r = gr.spqr_tree(*K4)
        assert [node["kind"] for node in r["nodes"]] == ["R"]

    def test_non_biconnected_input_is_rejected(self):
        assert gr.spqr_tree(*P4)["success"] is False


class TestPlanarEmbedding:
    def test_tree_embeds_with_one_face(self):
        r = gr.planar_embedding(*P4)
        assert r["success"] is True
        assert len(r["faces"]) == 1

    def test_cycle_has_two_faces(self):
        r = gr.planar_embedding(*C4)
        assert len(r["faces"]) == 2

    def test_k5_is_rejected(self):
        k5 = (5, [(u, v) for u in range(1, 6) for v in range(u + 1, 6)])
        assert gr.planar_embedding(*k5)["success"] is False

    def test_unknown_algorithm_rejected(self):
        with pytest.raises(ValueError):
            gr.planar_embedding(*P4, algorithm="nope")


class TestOrderings:
    def test_strong_elimination_ordering_of_a_path(self):
        r = gr.strong_elimination_ordering(*P4)
        assert r["success"] is True
        assert sorted(r["order"][1:]) == [1, 2, 3, 4]

    def test_three_sun_has_none(self):
        sun = (6, [(1, 2), (2, 3), (3, 1), (4, 1), (4, 2), (5, 2), (5, 3), (6, 3), (6, 1)])
        assert gr.strong_elimination_ordering(*sun)["success"] is False

    def test_indifference_tree_layout_of_a_path(self):
        r = gr.indifference_tree_layout(*P4)
        assert r["success"] is True
        assert len(r["parent"]) == 5


class TestConsecutiveOnes:
    def test_accepts_a_c1p_matrix(self):
        r = gr.consecutive_ones(5, [[1, 2], [2, 3, 4], [4, 5]])
        assert r["success"] is True

    def test_rejects_the_smallest_non_c1p_matrix(self):
        assert gr.consecutive_ones(4, [[1, 2], [1, 3], [1, 4]])["success"] is False

    def test_type_errors(self):
        with pytest.raises(TypeError):
            gr.consecutive_ones("4", [[1, 2]])
        with pytest.raises(ValueError):
            gr.consecutive_ones(-1, [])
        with pytest.raises(TypeError):
            gr.consecutive_ones(3, [[1, "2"]])


class TestInputHandling:
    @pytest.mark.parametrize("name", [n for n in DECOMPOSITIONS if n != "consecutive_ones"])
    def test_invalid_edges_rejected(self, name):
        fn = getattr(gr, name)
        with pytest.raises(ValueError):
            fn(3, [(1, 5)])
        with pytest.raises(ValueError):
            fn(3, [(1, 1)])
        with pytest.raises(TypeError):
            fn(3)

    def test_networkx_input(self):
        nx = pytest.importorskip("networkx")
        g = nx.path_graph(4)
        assert gr.connected_components(g) == [[1, 2, 3, 4]]
        with pytest.raises(TypeError):
            gr.connected_components(g, [(1, 2)])
