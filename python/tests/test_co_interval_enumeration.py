"""Co-interval enumeration through the complement and legacy algorithms."""

from itertools import combinations

import pytest

import graph_recognition as gr


@pytest.mark.parametrize("n, count", [(1, 1), (2, 2), (3, 8), (4, 61), (5, 822), (6, 17914)])
def test_algorithms_enumerate_the_same_labeled_graphs(n, count):
    default = gr.enumerate_co_interval_labeled_graphs(n)
    dedicated = gr.enumerate_co_interval_labeled_graphs(n, algorithm="kiyomi_kijima_uno")
    legacy = gr.enumerate_co_interval_labeled_graphs(n, algorithm="reverse_search")
    assert default == dedicated
    assert len(dedicated) == len(legacy) == count
    dedicated_set = {(size, tuple(edges)) for size, edges in dedicated}
    legacy_set = {(size, tuple(edges)) for size, edges in legacy}
    assert len(dedicated_set) == count
    assert dedicated_set == legacy_set
    for size, edges in dedicated:
        assert size == n
        assert edges == sorted(edges)
        assert all(1 <= u < v <= n for u, v in edges)
    assert dedicated[0] == (n, [])
    assert (n, tuple(combinations(range(1, n + 1), 2))) in dedicated_set
    if n >= 3:
        assert dedicated != legacy


@pytest.mark.parametrize("algorithm", [None, "kiyomi_kijima_uno", "reverse_search"])
@pytest.mark.parametrize("n", [-1, 0, gr.ENUM_MAX_N + 1])
def test_algorithms_preserve_vertex_count_validation(algorithm, n):
    with pytest.raises(ValueError):
        gr.enumerate_co_interval_labeled_graphs(n, algorithm=algorithm)


def test_unknown_algorithm_is_rejected():
    with pytest.raises(ValueError, match="Unknown algorithm"):
        gr.enumerate_co_interval_labeled_graphs(3, algorithm="unknown")


def test_algorithm_must_be_a_string():
    with pytest.raises(TypeError):
        gr.enumerate_co_interval_labeled_graphs(3, algorithm=42)
