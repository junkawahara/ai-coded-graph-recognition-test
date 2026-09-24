"""Canonical ear additions, legacy selection, and optional edge bounds."""

from itertools import permutations

import pytest

import graph_recognition as gr


def canonical_edges(n, edges):
    return min(
        tuple(sorted(tuple(sorted((order[u - 1], order[v - 1]))) for u, v in edges))
        for order in permutations(range(1, n + 1))
    )


@pytest.mark.parametrize("labeled", [False, True])
@pytest.mark.parametrize("n", range(1, 7))
def test_algorithms_agree(n, labeled):
    enumerate_fn = (gr.enumerate_biconnected_labeled_graphs if labeled
                    else gr.enumerate_biconnected_unlabeled_graphs)
    legacy_name = "reverse_search" if labeled else "canonical_augmentation"
    default = enumerate_fn(n)
    dedicated = enumerate_fn(n, algorithm="ear_augmentation")
    legacy = enumerate_fn(n, algorithm=legacy_name)
    counts = [0, 0, 1, 10, 238, 11368] if labeled else [0, 0, 1, 3, 10, 56]
    assert default == dedicated
    assert len(dedicated) == len(legacy) == counts[n - 1]
    key = (lambda size, edges: tuple(edges)) if labeled else canonical_edges
    assert {(size, key(size, edges)) for size, edges in dedicated} == {
        (size, key(size, edges)) for size, edges in legacy
    }


@pytest.mark.parametrize("labeled", [False, True])
def test_edge_bounds_and_validation(labeled):
    enumerate_fn = (gr.enumerate_biconnected_labeled_graphs if labeled
                    else gr.enumerate_biconnected_unlabeled_graphs)
    legacy = "reverse_search" if labeled else "canonical_augmentation"
    max_n = gr.ENUM_MAX_N if labeled else gr.UNLABELED_ENUM_MAX_N
    for algo in [None, "ear_augmentation", legacy]:
        full = enumerate_fn(5, algorithm=algo)
        for limit in range(12):
            assert enumerate_fn(5, algorithm=algo, max_edges=limit) == [
                graph for graph in full if len(graph[1]) <= limit
            ]
        assert enumerate_fn(5, algorithm=algo, max_edges=10**100) == full
        for n in [-1, 0, max_n + 1]:
            with pytest.raises(ValueError):
                enumerate_fn(n, algorithm=algo)
        for n in [True, 2.5, "3"]:
            with pytest.raises(TypeError):
                enumerate_fn(n, algorithm=algo)
        with pytest.raises(ValueError, match="nonnegative"):
            enumerate_fn(3, algorithm=algo, max_edges=-1)
        for limit in [True, 1.5, "3"]:
            with pytest.raises(TypeError):
                enumerate_fn(3, algorithm=algo, max_edges=limit)
    with pytest.raises(ValueError, match="Unknown algorithm"):
        enumerate_fn(3, algorithm="unknown")
    with pytest.raises(TypeError):
        enumerate_fn(3, algorithm=42)
    assert "ear_augmentation" in enumerate_fn.__doc__
    assert legacy in enumerate_fn.__doc__
    assert "max_edges" in enumerate_fn.__doc__


def test_unlabeled_limit_and_legacy_order():
    assert len(gr.enumerate_biconnected_unlabeled_graphs(8)) == 7123
    assert gr.enumerate_biconnected_unlabeled_graphs(4, "canonical_augmentation") == [
        (4, [(1, 3), (1, 4), (2, 3), (2, 4)]),
        (4, [(1, 3), (1, 4), (2, 3), (2, 4), (3, 4)]),
        (4, [(1, 2), (1, 3), (1, 4), (2, 3), (2, 4), (3, 4)]),
    ]
