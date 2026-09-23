"""Independent-set triangle-free enumeration and the retained legacy algorithm."""

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
def test_algorithms_enumerate_the_same_representatives_in_the_same_order(n, connected_only):
    # Preserve connected_only as the second positional argument.
    default = gr.enumerate_triangle_free_unlabeled_graphs(n, connected_only)
    dedicated = gr.enumerate_triangle_free_unlabeled_graphs(
        n, connected_only, algorithm="independent_sets"
    )
    legacy = gr.enumerate_triangle_free_unlabeled_graphs(
        n, connected_only=connected_only, algorithm="canonical_augmentation"
    )
    counts = [1, 1, 1, 3, 6, 19] if connected_only else [1, 2, 3, 7, 14, 38]
    assert default == dedicated == legacy
    assert len(dedicated) == counts[n - 1]
    assert len({canonical_edges(size, edges) for size, edges in dedicated}) == len(dedicated)
    for size, edges in dedicated:
        assert size == n
        assert gr.is_triangle_free(size, edges)
        assert edges == sorted(edges)
        assert all(1 <= u < v <= n for u, v in edges)
        if connected_only:
            reached = {1}
            for _ in range(n):
                for u, v in edges:
                    if u in reached or v in reached:
                        reached.update((u, v))
            assert len(reached) == n


def test_algorithm_selection_and_validation():
    enumerate_fn = gr.enumerate_triangle_free_unlabeled_graphs
    for algorithm in [None, "independent_sets", "canonical_augmentation"]:
        for n in [-1, 0, gr.UNLABELED_ENUM_MAX_N + 1]:
            with pytest.raises(ValueError):
                enumerate_fn(n, algorithm=algorithm)
        for n in [True, 2.5, "3"]:
            with pytest.raises(TypeError):
                enumerate_fn(n, algorithm=algorithm)
    for algorithm in ["unknown", "reverse_search"]:
        with pytest.raises(ValueError, match="Unknown algorithm"):
            enumerate_fn(3, algorithm=algorithm)
    with pytest.raises(TypeError):
        enumerate_fn(3, algorithm=42)


def test_legacy_algorithm_preserves_output_order():
    assert gr.enumerate_triangle_free_unlabeled_graphs(3, algorithm="canonical_augmentation") == [
        (3, []), (3, [(1, 3)]), (3, [(1, 3), (2, 3)]),
    ]
