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

    def test_n_beyond_cpp_limit(self):
        # n = INT_MAX would overflow the core's n + 1 array size; anything
        # larger cannot even be converted to a C int. Both are ValueErrors
        # here, not a pybind TypeError or a core exception.
        from graph_recognition._validation import MAX_VERTICES
        assert MAX_VERTICES == 2**31 - 2
        with pytest.raises(ValueError, match="at most"):
            is_interval(2**31 - 1, [])
        with pytest.raises(ValueError, match="at most"):
            is_interval(2**31, [])
        with pytest.raises(ValueError, match="at most"):
            is_interval(2**63, [])

    def test_directed_n_beyond_cpp_limit(self):
        # The directed recognizers (poset) do not go through Graph, but
        # their own n + 1 sizing has the same limit.
        from graph_recognition import is_poset
        with pytest.raises(ValueError, match="at most"):
            is_poset(2**31 - 1, [])

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
        from graph_recognition import enumerate_chordal_labeled_graphs
        with pytest.raises(ValueError, match="positive"):
            enumerate_chordal_labeled_graphs(0)

    def test_negative_n(self):
        from graph_recognition import enumerate_chordal_labeled_graphs
        with pytest.raises(ValueError, match="positive"):
            enumerate_chordal_labeled_graphs(-1)

    def test_non_integer_n(self):
        from graph_recognition import enumerate_chordal_labeled_graphs
        with pytest.raises(TypeError, match="integer"):
            enumerate_chordal_labeled_graphs(2.5)

    def test_bool_n(self):
        from graph_recognition import enumerate_chordal_labeled_graphs
        with pytest.raises(TypeError, match="integer"):
            enumerate_chordal_labeled_graphs(True)
