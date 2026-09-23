"""NO instances for classes that previously had only YES tests, plus
API-contract tests (networkx dispatch, kwargs, enumeration coverage)."""

import itertools

import pytest

import graph_recognition as gr


C5 = (5, [(1, 2), (2, 3), (3, 4), (4, 5), (1, 5)])
C6 = (6, [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (1, 6)])


class TestMissingNoCases:
    def test_permutation_c5_no(self):
        # C5 is not a comparability graph, hence not permutation
        assert gr.is_permutation(*C5) is False

    def test_at_free_c6_no(self):
        # alternate vertices of C6 form an asteroidal triple
        assert gr.is_at_free(*C6) is False

    def test_circular_arc_c4_plus_k1_no(self):
        # disconnected circular-arc requires every component interval;
        # C4 is not interval
        assert gr.is_circular_arc(5, [(1, 2), (2, 3), (3, 4), (1, 4)]) is False

    def test_co_chordal_c5_no(self):
        # complement of C5 is C5, which is not chordal
        assert gr.is_co_chordal(*C5) is False

    def test_co_comparability_c5_no(self):
        assert gr.is_co_comparability(*C5) is False

    def test_co_interval_2k2_no(self):
        # complement of 2K2 is C4, which is not interval
        assert gr.is_co_interval(4, [(1, 2), (3, 4)]) is False

    def test_trapezoid_c5_no(self):
        # trapezoid graphs are co-comparability
        assert gr.is_trapezoid(*C5) is False


@pytest.fixture
def nx():
    """networkx, or skip just the requesting test. An importorskip in a class
    body runs at collection time and skips the whole module, taking the
    unrelated tests below with it (review_all_1.md 36)."""
    return pytest.importorskip("networkx")


class TestNetworkXDispatch:
    def test_multigraph_rejected(self, nx):
        G = nx.MultiGraph([(1, 2), (1, 2)])
        with pytest.raises(TypeError, match="MultiGraph"):
            gr.is_interval(G)

    def test_multidigraph_rejected(self, nx):
        G = nx.MultiDiGraph([(1, 2)])
        with pytest.raises(TypeError, match="MultiGraph"):
            gr.is_interval(G)

    def test_digraph_rejected(self, nx):
        G = nx.DiGraph([(1, 2)])
        with pytest.raises(TypeError, match="DiGraph"):
            gr.is_interval(G)

    def test_graph_subclass_accepted(self, nx):
        class MyGraph(nx.Graph):
            pass

        G = MyGraph([(1, 2), (2, 3)])
        assert gr.is_interval(G) is True

    def test_self_loop_rejected(self, nx):
        G = nx.Graph()
        G.add_edge(1, 1)
        with pytest.raises(ValueError, match="[Ss]elf-loop"):
            gr.is_interval(G)


class TestKeywordArguments:
    def test_unknown_kwarg_rejected(self):
        with pytest.raises(TypeError, match="algorithim"):
            gr.is_interval(3, [(1, 2)], algorithim="at_free")

    def test_unknown_kwarg_rejected_recognize(self):
        with pytest.raises(TypeError, match="unexpected"):
            gr.recognize_interval(3, [(1, 2)], algo="at_free")

    def test_algorithm_kwarg_accepted(self):
        assert gr.is_interval(3, [(1, 2)], algorithm=None) is True


class TestEnumerationCoverage:
    """Smoke-test every enumeration function and cross-check the graphs
    against the corresponding recognizer where one exists."""

    # Classes with no 3-vertex member at all: self-complementary graphs
    # exist only for n = 0, 1 (mod 4); cubic (planar or not) graphs need
    # n even and >= 4; polyhedral graphs (3-connected planar) need n >= 4.
    EMPTY_AT_N3 = frozenset(
        [
            "cubic_planar_unlabeled",
            "cubic_unlabeled",
            "polyhedral_unlabeled",
            "self_complementary_unlabeled",
        ]
    )

    @pytest.mark.parametrize("type_name", gr._ENUM_TYPES)
    def test_enumerate_n3(self, type_name):
        enum_fn = getattr(gr, "enumerate_{}_graphs".format(type_name))
        result = enum_fn(3)
        assert isinstance(result, list)
        assert len(result) >= 1 or type_name in self.EMPTY_AT_N3
        seen = set()
        for n, edges in result:
            assert n == 3
            canon = frozenset(
                (min(u, v), max(u, v)) for u, v in edges
            )
            assert len(canon) == len(edges)
            assert canon not in seen, "duplicate graph in enumeration"
            seen.add(canon)
        # "bipartite_labeled" / "bipartite_unlabeled" -> is_bipartite.
        # Every enumerated class has a recognizer, so a missing one is a
        # registry error, not a reason to skip the membership check.
        base = type_name.rsplit("_", 1)[0]
        assert type_name.endswith(("_labeled", "_unlabeled")), type_name
        is_fn = getattr(gr, "is_{}".format(base))
        for n, edges in result:
            assert is_fn(n, edges) is True, (type_name, edges)
        if type_name in gr._NON_ISOMORPHIC_ENUM_TYPES:
            # Two representatives must not be isomorphic (n = 3: 6 relabelings).
            classes = set()
            for n, edges in result:
                canon = min(
                    tuple(sorted(
                        (min(p[u - 1], p[v - 1]), max(p[u - 1], p[v - 1]))
                        for u, v in edges
                    ))
                    for p in itertools.permutations(range(1, n + 1))
                )
                assert canon not in classes, "isomorphic duplicate in enumeration"
                classes.add(canon)

    def test_known_counts_n3(self):
        assert len(gr.enumerate_chordal_labeled_graphs(3)) == 8
        assert len(gr.enumerate_bipartite_labeled_graphs(3)) == 7
        assert len(gr.enumerate_circular_arc_labeled_graphs(3)) == 8
        # threshold enumeration is up to isomorphism: 2^(n-1) graphs
        assert len(gr.enumerate_threshold_unlabeled_graphs(3)) == 4
