"""Parametrized coverage of every recognize_* / is_* pair.

Previously only recognize_interval was ever executed; a broken binding for
any other type would have gone unnoticed. Each bound type is now run on a
small instance: recognize_<type> must return (bool, None) and agree with
is_<type>, and an unknown algorithm name must be rejected.
"""

import pytest

import graph_recognition as gr
from graph_recognition import ALGORITHMS, GRAPH_TYPES

# These recognizers interpret the edge list as directed arcs.
DIRECTED_TYPES = {"digraph", "poset", "tournament"}

# Small undirected instance: P4. Directed instance: the covering chain
# 1 -> 2 -> 3 (a valid digraph / Hasse diagram, not a tournament -- the
# boolean value does not matter here, only that the call works).
UNDIRECTED_INPUT = (4, [(1, 2), (2, 3), (3, 4)])
DIRECTED_INPUT = (3, [(1, 2), (2, 3)])


def _input_for(type_name):
    return DIRECTED_INPUT if type_name in DIRECTED_TYPES else UNDIRECTED_INPUT


@pytest.mark.parametrize("type_name", GRAPH_TYPES)
def test_recognize_matches_is(type_name):
    n, edges = _input_for(type_name)
    is_fn = getattr(gr, "is_{}".format(type_name))
    rec_fn = getattr(gr, "recognize_{}".format(type_name))

    expected = is_fn(n, edges)
    assert isinstance(expected, bool)

    result = rec_fn(n, edges)
    assert isinstance(result, tuple) and len(result) == 2
    assert result[0] is expected
    assert result[1] is None


@pytest.mark.parametrize("type_name", GRAPH_TYPES)
def test_explicit_algorithms_agree(type_name):
    n, edges = _input_for(type_name)
    is_fn = getattr(gr, "is_{}".format(type_name))
    default = is_fn(n, edges)
    for algo in ALGORITHMS[type_name]:
        assert is_fn(n, edges, algorithm=algo) is default, \
            "algorithm {} disagrees for {}".format(algo, type_name)


@pytest.mark.parametrize("type_name", GRAPH_TYPES)
def test_unknown_algorithm_rejected(type_name):
    n, edges = _input_for(type_name)
    is_fn = getattr(gr, "is_{}".format(type_name))
    with pytest.raises(ValueError):
        is_fn(n, edges, algorithm="definitely_not_an_algorithm")
