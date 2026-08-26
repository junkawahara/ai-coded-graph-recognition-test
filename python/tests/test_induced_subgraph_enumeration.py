"""Tests for the induced subgraph enumerators.

Unlike the spanning-subgraph enumerators, whose solutions are edge subsets of
the host, these enumerate the vertex subsets X for which the induced subgraph
G[X] belongs to the class, and so return vertex lists. Expected counts were
cross-checked against the C++ CLI
(bin/chordal_bipartite_induced_subgraph_enum) and against a brute force over
all 2^n vertex subsets.
"""

import itertools

import pytest

import graph_recognition as gr


def induced_edges(vertices, edges):
    """The host edges with both endpoints inside vertices."""
    keep = set(vertices)
    return [(u, v) for u, v in edges if u in keep and v in keep]


def relabel(vertices, edges):
    """The induced subgraph as a standalone 1-indexed (n, edges) pair."""
    label = {v: i + 1 for i, v in enumerate(sorted(vertices))}
    return len(label), [
        (label[u], label[v]) for u, v in induced_edges(vertices, edges)
    ]


def brute_force(n, edges):
    """Every chordal bipartite vertex subset, found by testing all 2^n."""
    found = set()
    for size in range(n + 1):
        for subset in itertools.combinations(range(1, n + 1), size):
            if gr.is_chordal_bipartite(*relabel(subset, edges)):
                found.add(subset)
    return found


class TestChordalBipartiteInducedSubgraphEnumeration:
    def test_path_4_yields_every_vertex_subset(self, path_4):
        # A path is chordal bipartite and the class is hereditary, so all
        # 2^4 subsets appear.
        n, edges = path_4
        assert len(gr.enumerate_chordal_bipartite_induced_subgraphs(n, edges)) == 16

    def test_cycle_6_excludes_only_the_cycle_itself(self, cycle_6):
        n, edges = cycle_6
        result = gr.enumerate_chordal_bipartite_induced_subgraphs(n, edges)
        assert len(result) == 63
        assert [1, 2, 3, 4, 5, 6] not in result

    def test_empty_set_comes_first_and_sets_are_sorted(self, cycle_5):
        n, edges = cycle_5
        result = gr.enumerate_chordal_bipartite_induced_subgraphs(n, edges)
        assert result[0] == []
        for vs in result:
            assert vs == sorted(vs)
            assert len(set(vs)) == len(vs)

    def test_edgeless_host_yields_every_vertex_subset(self, empty_3):
        n, edges = empty_3
        result = gr.enumerate_chordal_bipartite_induced_subgraphs(n, edges)
        assert len(result) == 8
        assert {tuple(vs) for vs in result} == {
            subset
            for size in range(4)
            for subset in itertools.combinations((1, 2, 3), size)
        }

    @pytest.mark.parametrize(
        "fixture_name,expected",
        [
            ("path_4", 16),
            ("cycle_4", 16),
            # C5 is an odd cycle, so only the full vertex set fails.
            ("cycle_5", 31),
            ("cycle_6", 63),
            # A triangle is not bipartite, so only its proper subsets pass.
            ("complete_3", 7),
            # In K4 every 3 vertices induce a triangle: 1 + 4 + 6 = 11.
            ("complete_4", 11),
            ("empty_3", 8),
            # A star is a tree, hence chordal bipartite: all 2^4 subsets.
            ("star_4", 16),
            ("complete_bipartite_2_3", 32),
            ("complete_bipartite_3_3", 64),
            # 64 minus the 8 subsets containing the whole triangle.
            ("triangle_3_pendants", 56),
        ],
    )
    def test_counts_and_validity(self, request, fixture_name, expected):
        n, edges = request.getfixturevalue(fixture_name)
        result = gr.enumerate_chordal_bipartite_induced_subgraphs(n, edges)
        assert len(result) == expected

        seen = set()
        for vs in result:
            key = tuple(vs)
            assert key not in seen, "duplicate vertex set in enumeration"
            seen.add(key)
            assert all(1 <= v <= n for v in vs), "vertex outside the host"
            assert gr.is_chordal_bipartite(*relabel(vs, edges)) is True

    @pytest.mark.parametrize(
        "fixture_name",
        ["path_4", "cycle_4", "cycle_5", "cycle_6", "complete_4",
         "complete_bipartite_2_3", "empty_3", "triangle_3_pendants"],
    )
    def test_matches_brute_force(self, request, fixture_name):
        n, edges = request.getfixturevalue(fixture_name)
        result = gr.enumerate_chordal_bipartite_induced_subgraphs(n, edges)
        assert {tuple(vs) for vs in result} == brute_force(n, edges)

    def test_networkx_host(self):
        nx = pytest.importorskip("networkx")
        # networkx.cycle_graph(6) is 0-indexed; from_networkx shifts it to
        # 1..6, which does not change the count.
        assert len(
            gr.enumerate_chordal_bipartite_induced_subgraphs(nx.cycle_graph(6))
        ) == 63

    def test_networkx_host_rejects_edges_argument(self):
        nx = pytest.importorskip("networkx")
        with pytest.raises(TypeError, match="must not be given"):
            gr.enumerate_chordal_bipartite_induced_subgraphs(
                nx.cycle_graph(4), [(1, 2)]
            )

    def test_edges_required_for_int_host(self):
        with pytest.raises(TypeError, match="edges is required"):
            gr.enumerate_chordal_bipartite_induced_subgraphs(4)

    def test_self_loop_rejected(self):
        with pytest.raises(ValueError, match="Self-loop"):
            gr.enumerate_chordal_bipartite_induced_subgraphs(4, [(1, 1)])

    def test_out_of_range_edge_rejected(self):
        with pytest.raises(ValueError, match="outside range"):
            gr.enumerate_chordal_bipartite_induced_subgraphs(4, [(1, 9)])

    def test_too_many_vertices_rejected(self):
        # The guard is on n, not m: an edgeless host is chordal bipartite and
        # already has 2^n induced subgraphs.
        with pytest.raises(ValueError, match="exceeds the supported maximum"):
            gr.enumerate_chordal_bipartite_induced_subgraphs(
                gr.INDUCED_ENUM_MAX_N + 1, []
            )

    def test_exported(self):
        assert "enumerate_chordal_bipartite_induced_subgraphs" in gr.__all__
