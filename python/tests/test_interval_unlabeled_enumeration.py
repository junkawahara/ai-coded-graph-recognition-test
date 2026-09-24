"""MPQ-tree enumeration and the retained canonical vertex additions."""

from itertools import permutations

import pytest

import graph_recognition as gr


def canonical_edges(n, edges):
    return min(
        tuple(sorted(tuple(sorted((order[u - 1], order[v - 1]))) for u, v in edges))
        for order in permutations(range(1, n + 1))
    )


@pytest.mark.parametrize("connected_only", [False, True])
@pytest.mark.parametrize("n", range(1, 7))
def test_algorithms_enumerate_the_same_classes(n, connected_only):
    default = gr.enumerate_interval_unlabeled_graphs(n, connected_only)
    dedicated = gr.enumerate_interval_unlabeled_graphs(n, connected_only, algorithm="mpq_tree")
    legacy = gr.enumerate_interval_unlabeled_graphs(
        n, connected_only=connected_only, algorithm="canonical_augmentation"
    )
    counts = [1, 1, 2, 5, 15, 56] if connected_only else [1, 2, 4, 10, 27, 92]
    assert default == dedicated
    assert len(dedicated) == len(legacy) == counts[n - 1]
    actual = {(size, canonical_edges(size, edges)) for size, edges in dedicated}
    expected = {(size, canonical_edges(size, edges)) for size, edges in legacy}
    assert len(actual) == len(expected) == counts[n - 1]
    assert actual == expected


def test_algorithm_validation_and_legacy_order():
    enumerate_fn = gr.enumerate_interval_unlabeled_graphs
    for algorithm in [None, "mpq_tree", "canonical_augmentation"]:
        for n in [-1, 0, gr.UNLABELED_ENUM_MAX_N + 1]:
            with pytest.raises(ValueError):
                enumerate_fn(n, algorithm=algorithm)
        for n in [True, 2.5, "3"]:
            with pytest.raises(TypeError):
                enumerate_fn(n, algorithm=algorithm)
    with pytest.raises(ValueError, match="Unknown algorithm"):
        enumerate_fn(3, algorithm="unknown")
    with pytest.raises(TypeError):
        enumerate_fn(3, algorithm=42)
    assert enumerate_fn(3, algorithm="canonical_augmentation") == [
        (3, []), (3, [(1, 3)]), (3, [(1, 3), (2, 3)]),
        (3, [(1, 2), (1, 3), (2, 3)]),
    ]


def test_maximum_python_size_and_docstring():
    assert len(gr.enumerate_interval_unlabeled_graphs(8)) == 1807
    assert len(gr.enumerate_interval_unlabeled_graphs(8, True)) == 1328
    assert "mpq_tree" in gr.enumerate_interval_unlabeled_graphs.__doc__
    assert "canonical_augmentation" in gr.enumerate_interval_unlabeled_graphs.__doc__
