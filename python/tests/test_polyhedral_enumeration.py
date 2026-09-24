"""Canonical edge deletions and retained planar vertex additions."""

from itertools import permutations

import pytest

import graph_recognition as gr


def canonical_edges(n, edges):
    return min(
        tuple(sorted(tuple(sorted((order[u - 1], order[v - 1]))) for u, v in edges))
        for order in permutations(range(1, n + 1))
    )


@pytest.mark.parametrize("n", range(1, 8))
def test_algorithms_enumerate_the_same_classes(n):
    default = gr.enumerate_polyhedral_unlabeled_graphs(n)
    dedicated = gr.enumerate_polyhedral_unlabeled_graphs(n, algorithm="plantri")
    legacy = gr.enumerate_polyhedral_unlabeled_graphs(n, algorithm="canonical_augmentation")
    assert default == dedicated
    assert len(dedicated) == len(legacy) == [0, 0, 0, 1, 2, 7, 34][n - 1]
    actual = {canonical_edges(size, edges) for size, edges in dedicated}
    expected = {canonical_edges(size, edges) for size, edges in legacy}
    assert len(actual) == len(dedicated)
    assert actual == expected
    assert all(gr.is_polyhedral(size, edges) for size, edges in dedicated)


def test_algorithm_validation_and_small_graphs():
    enumerate_fn = gr.enumerate_polyhedral_unlabeled_graphs
    for algorithm in [None, "", "plantri", "canonical_augmentation"]:
        for n in [-1, 0, gr.UNLABELED_ENUM_MAX_N + 1]:
            with pytest.raises(ValueError):
                enumerate_fn(n, algorithm=algorithm)
        for n in [True, 2.5, "3"]:
            with pytest.raises(TypeError):
                enumerate_fn(n, algorithm=algorithm)
        for n in range(1, 4):
            assert enumerate_fn(n, algorithm=algorithm) == []
    for algorithm in ["unknown", "reverse_search"]:
        with pytest.raises(ValueError, match="Unknown algorithm"):
            enumerate_fn(3, algorithm=algorithm)
    with pytest.raises(TypeError):
        enumerate_fn(3, algorithm=42)
    with pytest.raises(TypeError):
        enumerate_fn(3, connected_only=True)


def test_maximum_python_size_and_docstring():
    assert len(gr.enumerate_polyhedral_unlabeled_graphs(gr.UNLABELED_ENUM_MAX_N)) == 257
    assert "plantri" in gr.enumerate_polyhedral_unlabeled_graphs.__doc__
    assert "canonical_augmentation" in gr.enumerate_polyhedral_unlabeled_graphs.__doc__
