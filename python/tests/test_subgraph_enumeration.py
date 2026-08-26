"""Tests for the subgraph enumerators.

Unlike enumerate_<type>_graphs(n), these take a host graph and enumerate the
subgraphs of it that belong to the class. Expected counts were cross-checked
against the C++ CLI (bin/chordal_subgraph_enum) and, for the small hosts,
against a brute force over all 2^m edge subsets.
"""

import itertools

import pytest

import graph_recognition as gr


def canon(edges):
    """Isomorphism-free key: the normalized, sorted edge set."""
    return frozenset((min(u, v), max(u, v)) for u, v in edges)


def brute_force(n, edges):
    """Every chordal edge subset of the host, found by testing all 2^m."""
    found = set()
    for size in range(len(edges) + 1):
        for subset in itertools.combinations(edges, size):
            if gr.is_chordal(n, list(subset)):
                found.add(canon(subset))
    return found


class TestChordalSubgraphEnumeration:
    def test_path_4_yields_every_edge_subset(self, path_4):
        # Every subgraph of a forest is chordal, so all 2^3 subsets appear.
        n, edges = path_4
        assert len(gr.enumerate_chordal_subgraphs(n, edges)) == 8

    def test_cycle_4_excludes_only_the_cycle_itself(self, cycle_4):
        n, edges = cycle_4
        result = gr.enumerate_chordal_subgraphs(n, edges)
        assert len(result) == 15
        assert canon(edges) not in {canon(sub) for _, sub in result}

    def test_complete_4_matches_labeled_chordal_enumeration(self, complete_4):
        # A complete host makes this the fixed-n labeled chordal enumeration.
        n, edges = complete_4
        assert {canon(sub) for _, sub in gr.enumerate_chordal_subgraphs(n, edges)} == {
            canon(sub) for _, sub in gr.enumerate_chordal_graphs(4)
        }

    def test_edgeless_host_yields_only_the_empty_subgraph(self, empty_3):
        n, edges = empty_3
        assert gr.enumerate_chordal_subgraphs(n, edges) == [(3, [])]

    @pytest.mark.parametrize(
        "fixture_name,expected",
        [
            ("path_4", 8),
            ("cycle_4", 15),
            ("cycle_5", 31),
            ("cycle_6", 63),
            ("complete_3", 8),
            ("complete_4", 61),
            ("complete_5", 822),
            ("star_4", 8),
            # K_{2,3} has three C4s; by inclusion-exclusion 12 - 3 + 1 = 10
            # of its 64 edge subsets contain one, leaving 54.
            ("complete_bipartite_2_3", 54),
            ("complete_bipartite_3_3", 328),
        ],
    )
    def test_counts_and_validity(self, request, fixture_name, expected):
        n, edges = request.getfixturevalue(fixture_name)
        result = gr.enumerate_chordal_subgraphs(n, edges)
        assert len(result) == expected

        host = canon(edges)
        seen = set()
        for vertices, sub in result:
            assert vertices == n
            key = canon(sub)
            assert len(key) == len(sub), "duplicate edge within a subgraph"
            assert key <= host, "edge outside the host graph"
            assert key not in seen, "duplicate subgraph in enumeration"
            seen.add(key)
            assert gr.is_chordal(n, sub) is True

    @pytest.mark.parametrize(
        "fixture_name",
        ["path_4", "cycle_4", "cycle_5", "cycle_6", "complete_4",
         "complete_bipartite_2_3", "empty_3", "triangle_3_pendants"],
    )
    def test_matches_brute_force(self, request, fixture_name):
        n, edges = request.getfixturevalue(fixture_name)
        result = gr.enumerate_chordal_subgraphs(n, edges)
        assert {canon(sub) for _, sub in result} == brute_force(n, edges)

    def test_networkx_host(self):
        nx = pytest.importorskip("networkx")
        assert len(gr.enumerate_chordal_subgraphs(nx.cycle_graph(4))) == 15

    def test_networkx_host_rejects_edges_argument(self):
        nx = pytest.importorskip("networkx")
        with pytest.raises(TypeError, match="must not be given"):
            gr.enumerate_chordal_subgraphs(nx.cycle_graph(4), [(1, 2)])

    def test_edges_required_for_int_host(self):
        with pytest.raises(TypeError, match="edges is required"):
            gr.enumerate_chordal_subgraphs(4)

    def test_self_loop_rejected(self):
        with pytest.raises(ValueError, match="Self-loop"):
            gr.enumerate_chordal_subgraphs(4, [(1, 1)])

    def test_out_of_range_edge_rejected(self):
        with pytest.raises(ValueError, match="outside range"):
            gr.enumerate_chordal_subgraphs(4, [(1, 9)])

    def test_too_many_edges_rejected(self):
        # K7 has 21 edges; the guard is on m because a host with m edges can
        # have up to 2^m chordal subgraphs.
        edges = list(itertools.combinations(range(1, 8), 2))
        with pytest.raises(ValueError, match="exceeds the supported maximum"):
            gr.enumerate_chordal_subgraphs(7, edges)

    def test_duplicate_edges_do_not_count_towards_the_guard(self):
        # The guard counts distinct edges, matching what the C++ side sees.
        edges = list(itertools.combinations(range(1, 7), 2))
        assert len(edges) == 15
        result = gr.enumerate_chordal_subgraphs(6, edges + edges)
        assert len(result) == len(gr.enumerate_chordal_graphs(6))

    def test_exported(self):
        assert "enumerate_chordal_subgraphs" in gr.__all__
