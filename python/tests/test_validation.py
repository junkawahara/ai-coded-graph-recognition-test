"""Tests for input validation."""

import pytest
from graph_recognition import is_interval, recognize_interval


class TestTypeErrors:
    def test_non_integer_n(self):
        with pytest.raises(TypeError, match="integer"):
            is_interval(3.5, [])

    def test_string_n(self):
        with pytest.raises(TypeError, match="integer"):
            is_interval("3", [])

    def test_non_integer_edge_vertex(self):
        with pytest.raises(TypeError, match="integers"):
            is_interval(3, [(1.0, 2)])

    def test_networkx_graph_with_edges(self):
        """Passing edges along with a NetworkX graph should raise TypeError."""
        pytest.importorskip("networkx")
        import networkx as nx
        G = nx.path_graph(3)
        with pytest.raises(TypeError, match="NetworkX"):
            is_interval(G, [(1, 2)])


class TestValueErrors:
    def test_negative_n(self):
        with pytest.raises(ValueError, match="non-negative"):
            is_interval(-1, [])

    def test_vertex_too_large(self):
        with pytest.raises(ValueError, match="outside range"):
            is_interval(3, [(1, 5)])

    def test_vertex_zero(self):
        with pytest.raises(ValueError, match="outside range"):
            is_interval(3, [(0, 1)])

    def test_vertex_negative(self):
        with pytest.raises(ValueError, match="outside range"):
            is_interval(3, [(-1, 2)])

    def test_bad_edge_format(self):
        with pytest.raises(ValueError, match="pair"):
            is_interval(3, [(1, 2, 3)])

    def test_bad_algorithm(self):
        with pytest.raises(ValueError, match="Unknown algorithm"):
            is_interval(3, [(1, 2)], algorithm="nonexistent")


class TestRecognizeValidation:
    def test_negative_n(self):
        with pytest.raises(ValueError):
            recognize_interval(-1, [])

    def test_bad_algorithm(self):
        with pytest.raises(ValueError, match="Unknown algorithm"):
            recognize_interval(3, [(1, 2)], algorithm="bad")


class TestEnumerateValidation:
    def test_zero_n(self):
        from graph_recognition import enumerate_chordal_graphs
        with pytest.raises(ValueError, match="positive"):
            enumerate_chordal_graphs(0)

    def test_negative_n(self):
        from graph_recognition import enumerate_chordal_graphs
        with pytest.raises(ValueError, match="positive"):
            enumerate_chordal_graphs(-1)

    def test_non_integer_n(self):
        from graph_recognition import enumerate_chordal_graphs
        with pytest.raises(ValueError):
            enumerate_chordal_graphs(2.5)
