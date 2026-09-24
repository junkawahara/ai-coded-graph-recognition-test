"""BlockGraphGen, component composition, label expansion and legacy selection."""

from itertools import permutations

import pytest

import graph_recognition as gr


@pytest.mark.parametrize("n, count", [(1, 1), (2, 2), (3, 8), (4, 55), (5, 562), (6, 7739)])
def test_labeled_algorithms_match(n, count):
    default = gr.enumerate_block_labeled_graphs(n)
    dedicated = gr.enumerate_block_labeled_graphs(n, algorithm="block_graph_gen")
    legacy = gr.enumerate_block_labeled_graphs(n, algorithm="reverse_search")
    assert default == dedicated
    actual = {(size, tuple(edges)) for size, edges in dedicated}
    assert len(actual) == len(dedicated) == len(legacy) == count
    assert actual == {(size, tuple(edges)) for size, edges in legacy}
    for size, edges in dedicated:
        assert size == n
        assert edges == sorted(edges)
        assert all(1 <= u < v <= n for u, v in edges)


@pytest.mark.parametrize("connected_only", [False, True])
@pytest.mark.parametrize("n", range(1, 9))
def test_unlabeled_counts_and_validity(n, connected_only):
    result = gr.enumerate_block_unlabeled_graphs(n, connected_only)
    assert result == gr.enumerate_block_unlabeled_graphs(
        n, connected_only=connected_only, algorithm="block_graph_gen"
    )
    counts = [1, 1, 2, 4, 9, 22, 59, 165] if connected_only else [1, 2, 4, 9, 20, 50, 128, 351]
    assert len(result) == counts[n - 1]
    for size, edges in result:
        assert size == n
        assert gr.is_block(size, edges)


def test_unlabeled_classes_match_legacy_labelings():
    def canonical(edges):
        return min(
            tuple(sorted(tuple(sorted((p[u - 1], p[v - 1]))) for u, v in edges))
            for p in permutations(range(1, 6))
        )

    result = gr.enumerate_block_unlabeled_graphs(5)
    actual = {canonical(edges) for _, edges in result}
    expected = {
        canonical(edges)
        for _, edges in gr.enumerate_block_labeled_graphs(5, algorithm="reverse_search")
    }
    assert len(actual) == len(result)
    assert actual == expected


def test_validation_legacy_order_and_documentation():
    for enumerate_fn, maximum, algorithms in [
        (gr.enumerate_block_labeled_graphs, gr.ENUM_MAX_N, [None, "block_graph_gen", "reverse_search"]),
        (gr.enumerate_block_unlabeled_graphs, gr.UNLABELED_ENUM_MAX_N, [None, "block_graph_gen"]),
    ]:
        for algorithm in algorithms:
            for n in [-1, 0, maximum + 1]:
                with pytest.raises(ValueError):
                    enumerate_fn(n, algorithm=algorithm)
            for n in [True, 2.5, "3"]:
                with pytest.raises(TypeError):
                    enumerate_fn(n, algorithm=algorithm)
        with pytest.raises(ValueError, match="Unknown algorithm"):
            enumerate_fn(3, algorithm="unknown")
        with pytest.raises(TypeError):
            enumerate_fn(3, algorithm=42)
        assert "block_graph_gen" in enumerate_fn.__doc__
    assert gr.enumerate_block_labeled_graphs(2, algorithm="reverse_search") == [
        (2, []), (2, [(1, 2)])
    ]
    assert "reverse_search" in gr.enumerate_block_labeled_graphs.__doc__
