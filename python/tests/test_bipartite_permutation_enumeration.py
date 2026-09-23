"""Canonical line-string bipartite permutation enumeration and the retained legacy algorithms."""

from itertools import permutations

import pytest

import graph_recognition as gr


def canonical_edges(n, edges):
    return min(
        tuple(sorted(tuple(sorted((order[u - 1], order[v - 1]))) for u, v in edges))
        for order in permutations(range(1, n + 1))
    )


@pytest.mark.parametrize("n, count", [(1, 1), (2, 2), (3, 7), (4, 41), (5, 376), (6, 5117)])
def test_labeled_algorithms_enumerate_the_same_graphs(n, count):
    default = gr.enumerate_bipartite_permutation_labeled_graphs(n)
    dedicated = gr.enumerate_bipartite_permutation_labeled_graphs(n, algorithm="string_representation")
    legacy = gr.enumerate_bipartite_permutation_labeled_graphs(n, algorithm="reverse_search")
    assert default == dedicated
    assert len(dedicated) == len(legacy) == count
    actual = {(size, tuple(edges)) for size, edges in dedicated}
    expected = {(size, tuple(edges)) for size, edges in legacy}
    assert len(actual) == count
    assert actual == expected
    assert (n, ()) in actual
    for size, edges in dedicated:
        assert size == n
        assert edges == sorted(edges)
        assert all(1 <= u < v <= n for u, v in edges)


@pytest.mark.parametrize("connected_only", [False, True])
@pytest.mark.parametrize("n", range(1, 7))
def test_unlabeled_algorithms_enumerate_the_same_isomorphism_classes(n, connected_only):
    # Keep connected_only as the second positional argument for compatibility.
    default = gr.enumerate_bipartite_permutation_unlabeled_graphs(n, connected_only)
    dedicated = gr.enumerate_bipartite_permutation_unlabeled_graphs(
        n, connected_only, algorithm="string_representation"
    )
    legacy = gr.enumerate_bipartite_permutation_unlabeled_graphs(
        n, connected_only=connected_only, algorithm="canonical_augmentation"
    )
    counts = [1, 1, 1, 3, 5, 16] if connected_only else [1, 2, 3, 7, 13, 34]
    assert default == dedicated
    assert len(dedicated) == len(legacy) == counts[n - 1]
    actual = {(size, canonical_edges(size, edges)) for size, edges in dedicated}
    expected = {(size, canonical_edges(size, edges)) for size, edges in legacy}
    assert len(actual) == len(expected) == counts[n - 1]
    assert actual == expected


@pytest.mark.parametrize("enumerate_fn, legacy, max_n", [
    (gr.enumerate_bipartite_permutation_labeled_graphs, "reverse_search", gr.ENUM_MAX_N),
    (gr.enumerate_bipartite_permutation_unlabeled_graphs, "canonical_augmentation", gr.UNLABELED_ENUM_MAX_N),
])
def test_algorithm_selection_and_validation(enumerate_fn, legacy, max_n):
    for algorithm in [None, "string_representation", legacy]:
        for n in [-1, 0, max_n + 1]:
            with pytest.raises(ValueError):
                enumerate_fn(n, algorithm=algorithm)
        for n in [True, 2.5, "3"]:
            with pytest.raises(TypeError):
                enumerate_fn(n, algorithm=algorithm)
    with pytest.raises(ValueError, match="Unknown algorithm"):
        enumerate_fn(3, algorithm="unknown")
    with pytest.raises(TypeError):
        enumerate_fn(3, algorithm=42)


def test_legacy_algorithms_preserve_output_order():
    assert gr.enumerate_bipartite_permutation_labeled_graphs(3, algorithm="reverse_search") == [
        (3, []), (3, [(1, 3)]), (3, [(2, 3)]), (3, [(1, 3), (2, 3)]),
        (3, [(1, 2)]), (3, [(1, 2), (1, 3)]), (3, [(1, 2), (2, 3)]),
    ]
    assert gr.enumerate_bipartite_permutation_unlabeled_graphs(3, algorithm="canonical_augmentation") == [
        (3, []), (3, [(1, 3)]), (3, [(1, 3), (2, 3)]),
    ]
