"""Dual triangulations, component multisets and the retained vertex search."""

from itertools import permutations

import pytest

import graph_recognition as gr


def canonical_edges(n, edges):
    return min(
        tuple(sorted(tuple(sorted((order[u - 1], order[v - 1]))) for u, v in edges))
        for order in permutations(range(1, n + 1))
    )


@pytest.mark.parametrize("n", range(1, 9))
@pytest.mark.parametrize("connected", [False, True])
def test_algorithms_enumerate_the_same_classes(n, connected):
    enumerate_fn = gr.enumerate_cubic_planar_unlabeled_graphs
    default = enumerate_fn(n, connected)
    dedicated = enumerate_fn(n, connected_only=connected, algorithm="plantri")
    legacy = enumerate_fn(n, connected, "canonical_augmentation")
    assert default == dedicated
    counts = [0, 0, 0, 1, 0, 1, 0, 3 if connected else 4]
    assert len(dedicated) == len(legacy) == counts[n - 1]
    actual = {canonical_edges(size, edges) for size, edges in dedicated}
    expected = {canonical_edges(size, edges) for size, edges in legacy}
    assert len(actual) == len(dedicated)
    assert actual == expected
    assert all(gr.is_cubic_planar(size, edges) for size, edges in dedicated)


def test_algorithm_validation_and_small_graphs():
    enumerate_fn = gr.enumerate_cubic_planar_unlabeled_graphs
    for algorithm in [None, "", "plantri", "canonical_augmentation"]:
        for n in [-1, 0, gr.UNLABELED_ENUM_MAX_N + 1]:
            with pytest.raises(ValueError):
                enumerate_fn(n, algorithm=algorithm)
        for n in [True, 2.5, "3"]:
            with pytest.raises(TypeError):
                enumerate_fn(n, algorithm=algorithm)
        for n in [1, 2, 3, 5, 7]:
            assert enumerate_fn(n, algorithm=algorithm) == []
        assert len(enumerate_fn(8, algorithm=algorithm)) == 4
    for algorithm in ["unknown", "reverse_search"]:
        with pytest.raises(ValueError, match="Unknown algorithm"):
            enumerate_fn(3, algorithm=algorithm)
    with pytest.raises(TypeError):
        enumerate_fn(3, algorithm=42)
    assert "plantri" in enumerate_fn.__doc__
    assert "canonical_augmentation" in enumerate_fn.__doc__
    assert "connected_only" in enumerate_fn.__doc__
