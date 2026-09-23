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


# review_all_1.md 65 / 67: which containers and integer types an edge may use.
class TestEdgeContainers:
    def test_set_edge_is_rejected(self):
        with pytest.raises(TypeError, match="sequence"):
            is_interval(3, [{1, 2}, {2, 3}])

    def test_frozenset_edge_is_rejected(self):
        with pytest.raises(TypeError, match="sequence"):
            is_interval(3, [frozenset({1, 2})])

    def test_dict_edge_is_rejected(self):
        with pytest.raises(TypeError, match="sequence"):
            is_interval(3, [{1: 2, 3: 4}])

    def test_directed_set_arc_is_rejected(self):
        from graph_recognition import is_poset
        with pytest.raises(TypeError, match="sequence"):
            is_poset(2, [{1, 2}])

    def test_list_edges_are_accepted(self):
        assert is_interval(3, [[1, 2], [2, 3]]) is True

    def test_numpy_integers_are_accepted(self):
        np = pytest.importorskip("numpy")
        assert is_interval(np.int64(3), [(1, 2), (2, 3)]) is True
        assert is_interval(3, [(np.int32(1), np.int64(2)), (2, 3)]) is True
        c4 = np.array([[1, 2], [2, 3], [3, 4], [4, 1]])
        assert is_interval(4, c4) is False
        ok, cert = recognize_interval(np.int64(4), c4)
        assert ok is False and cert is not None

    def test_numpy_bool_is_rejected(self):
        np = pytest.importorskip("numpy")
        with pytest.raises(TypeError, match="integer"):
            is_interval(np.bool_(True), [])

    def test_numpy_float_is_rejected(self):
        np = pytest.importorskip("numpy")
        with pytest.raises(TypeError, match="integer"):
            is_interval(np.float64(3.0), [])
        with pytest.raises(TypeError, match="integer"):
            is_interval(3, [(np.float64(1.0), 2)])

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

    def test_numpy_n(self):
        np = pytest.importorskip("numpy")
        from graph_recognition import enumerate_chordal_labeled_graphs
        assert len(enumerate_chordal_labeled_graphs(np.int64(3))) == 8


class TestConsecutiveOnesValidation:
    def test_numpy_columns(self):
        np = pytest.importorskip("numpy")
        from graph_recognition import consecutive_ones
        r = consecutive_ones(np.int64(3), [np.array([1, 2]), [np.int64(2), 3]])
        assert r["success"] is True

    def test_bool_column_is_rejected(self):
        from graph_recognition import consecutive_ones
        with pytest.raises(TypeError, match="non-integer"):
            consecutive_ones(3, [[True, 2]])
