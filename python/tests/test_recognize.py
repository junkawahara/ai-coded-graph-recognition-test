"""Cross-checks that every binding is wired to the right C++ checker.

Three layers, none of which can pass with a miswired binding:

1. The _core module must export exactly one _check_* per GRAPH_TYPES entry
   and one _enumerate_* per _ENUM_TYPES entry (catches typos in m.def and
   registry drift in either direction).
2. Every recognizer must reproduce the expected YES/NO answers of the C++
   test fixtures under tests/<type>/ (catches a binding calling the wrong
   checker: the fixture answers differ between classes). Runs only from a
   repository checkout; skipped in an sdist install.
3. recognize_<type> must return (bool, None) agreeing with is_<type>, and
   unknown algorithm names must be rejected (API contract; note that
   is_/recognize_ share the underlying checker by construction, so this
   alone would not catch miswiring -- layers 1 and 2 do).
"""

import os

import pytest

import graph_recognition as gr
from graph_recognition import ALGORITHMS, GRAPH_TYPES
from graph_recognition import _core

# These recognizers interpret the edge list as directed arcs.
DIRECTED_TYPES = {"digraph", "poset", "tournament"}

# Small undirected instance: P4. Directed instance: the covering chain
# 1 -> 2 -> 3 (a valid digraph / Hasse diagram, not a tournament -- the
# boolean value does not matter here, only that the call works).
UNDIRECTED_INPUT = (4, [(1, 2), (2, 3), (3, 4)])
DIRECTED_INPUT = (3, [(1, 2), (2, 3)])

REPO_TESTS_DIR = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), os.pardir, os.pardir, "tests"
)


def _input_for(type_name):
    return DIRECTED_INPUT if type_name in DIRECTED_TYPES else UNDIRECTED_INPUT


def test_core_bindings_match_registry():
    core_checks = sorted(
        name[len("_check_"):] for name in dir(_core) if name.startswith("_check_")
    )
    assert core_checks == sorted(GRAPH_TYPES)

    core_enums = sorted(
        name[len("_enumerate_"):]
        for name in dir(_core)
        if name.startswith("_enumerate_")
    )
    assert core_enums == sorted(gr._ENUM_TYPES)


def _fixture_cases(type_name):
    d = os.path.join(REPO_TESTS_DIR, type_name)
    if not os.path.isdir(d):
        return []
    stems = sorted(
        f[:-3] for f in os.listdir(d)
        if f.endswith(".in") and os.path.isfile(os.path.join(d, f[:-3] + ".exp"))
    )
    return [(os.path.join(d, s + ".in"), os.path.join(d, s + ".exp")) for s in stems]


@pytest.mark.parametrize("type_name", GRAPH_TYPES)
def test_fixture_answers_match_cpp_expectations(type_name):
    """is_<type> reproduces the C++ fixture answers for its own class."""
    cases = _fixture_cases(type_name)
    if not cases:
        pytest.skip("no repository fixtures for {}".format(type_name))

    is_fn = getattr(gr, "is_{}".format(type_name))
    for in_path, exp_path in cases:
        with open(in_path) as f:
            header = f.readline().split()
            n, m = int(header[0]), int(header[1])
            edges = []
            for _ in range(m):
                u, v = f.readline().split()
                edges.append((int(u), int(v)))
        with open(exp_path) as f:
            expected = f.readline().split()[0]
        assert expected in ("YES", "NO"), in_path

        got = is_fn(n, edges)
        assert got is (expected == "YES"), "{} disagrees with {}".format(
            "is_" + type_name, in_path
        )


def test_digraph_invalid_arcs_answer_no_like_the_cli():
    """Self-loops / range violations are the NO conditions the directed
    recognizers exist to detect; validation must not pre-empt them."""
    assert gr.is_digraph(3, [(1, 1)]) is False          # self-loop
    assert gr.is_digraph(3, [(1, 4)]) is False          # out of range
    assert gr.is_digraph(3, [(1, 2), (1, 2)]) is False  # duplicate arc
    assert gr.is_digraph(3, [(1, 2), (2, 1)]) is True   # anti-parallel is fine
    assert gr.is_tournament(2, [(1, 1)]) is False
    assert gr.is_poset(2, [(1, 1)]) is False


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


def test_enumeration_cap_rejects_large_n():
    with pytest.raises(ValueError):
        gr.enumerate_chordal_graphs(gr.ENUM_MAX_N + 1)
    # The cap itself still works.
    assert len(gr.enumerate_chordal_graphs(3)) == 8
