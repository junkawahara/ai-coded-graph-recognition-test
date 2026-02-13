"""Tests for NetworkX integration."""

import pytest

nx = pytest.importorskip("networkx")

from graph_recognition import (
    is_bipartite,
    is_chordal,
    is_interval,
    is_planar,
    recognize_interval,
)


class TestNetworkXBasic:
    def test_path_interval(self):
        G = nx.path_graph(5)
        assert is_interval(G) is True

    def test_cycle_4_not_interval(self):
        G = nx.cycle_graph(4)
        assert is_interval(G) is False

    def test_cycle_4_bipartite(self):
        G = nx.cycle_graph(4)
        assert is_bipartite(G) is True

    def test_complete_chordal(self):
        G = nx.complete_graph(5)
        assert is_chordal(G) is True

    def test_complete_5_not_planar(self):
        G = nx.complete_graph(5)
        assert is_planar(G) is False

    def test_empty_graph(self):
        G = nx.empty_graph(5)
        assert is_interval(G) is True

    def test_single_vertex(self):
        G = nx.Graph()
        G.add_node(0)
        assert is_chordal(G) is True


class TestNetworkXNodeTypes:
    def test_string_nodes(self):
        G = nx.Graph()
        G.add_edges_from([("a", "b"), ("b", "c")])
        assert is_interval(G) is True

    def test_mixed_integer_nodes(self):
        """NetworkX with 0-indexed nodes gets renumbered to 1-indexed."""
        G = nx.Graph()
        G.add_edges_from([(0, 1), (1, 2), (2, 3)])
        assert is_interval(G) is True

    def test_non_contiguous_nodes(self):
        G = nx.Graph()
        G.add_edges_from([(10, 20), (20, 30)])
        assert is_interval(G) is True


class TestNetworkXRecognize:
    def test_recognize_path(self):
        G = nx.path_graph(4)
        result, cert = recognize_interval(G)
        assert result is True
        assert cert is None

    def test_recognize_cycle(self):
        G = nx.cycle_graph(4)
        result, cert = recognize_interval(G)
        assert result is False
        assert cert is None
