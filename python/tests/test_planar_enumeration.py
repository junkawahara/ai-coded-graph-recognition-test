"""Embedded edge deletion, abstract isomorph rejection and legacy options."""

from itertools import permutations

import pytest

import graph_recognition as gr


def canonical_edges(n, edges):
    return min(
        tuple(sorted(tuple(sorted((order[u - 1], order[v - 1]))) for u, v in edges))
        for order in permutations(range(1, n + 1))
    )


@pytest.mark.parametrize("n", range(1, 7))
@pytest.mark.parametrize("connected", [False, True])
def test_unlabeled_classes_and_positional_connected_flag(n, connected):
    enum = gr.enumerate_planar_unlabeled_graphs
    default = enum(n, connected)
    dedicated = enum(n, connected_only=connected, algorithm="plantri")
    legacy = enum(n, connected, algorithm="canonical_augmentation")
    assert default == dedicated
    counts = [1, 1, 2, 6, 20, 99] if connected else [1, 2, 4, 11, 33, 142]
    assert len(dedicated) == len(legacy) == counts[n - 1]
    actual = {canonical_edges(size, edges) for size, edges in dedicated}
    assert len(actual) == len(dedicated)
    assert actual == {canonical_edges(size, edges) for size, edges in legacy}
    assert all(gr.is_planar(size, edges) for size, edges in dedicated)


@pytest.mark.parametrize("n", range(1, 7))
def test_all_labelings_match_legacy(n):
    enum = gr.enumerate_planar_labeled_graphs
    dedicated = enum(n, algorithm="plantri")
    legacy = enum(n, algorithm="reverse_search")
    assert enum(n) == dedicated
    actual = {tuple(edges) for _, edges in dedicated}
    assert len(actual) == len(dedicated)
    assert actual == {tuple(edges) for _, edges in legacy}


@pytest.mark.parametrize("labeled", [False, True])
def test_validation_empty_algorithm_and_documentation(labeled):
    enum = gr.enumerate_planar_labeled_graphs if labeled else gr.enumerate_planar_unlabeled_graphs
    legacy = "reverse_search" if labeled else "canonical_augmentation"
    max_n = gr.ENUM_MAX_N if labeled else gr.UNLABELED_ENUM_MAX_N
    for algorithm in [None, "", "plantri", legacy]:
        assert enum(1, algorithm=algorithm) == [(1, [])]
        for n in [-1, 0, max_n + 1]:
            with pytest.raises(ValueError):
                enum(n, algorithm=algorithm)
        for n in [True, 2.5, "3"]:
            with pytest.raises(TypeError):
                enum(n, algorithm=algorithm)
    for algorithm in ["unknown", "canonical_augmentation" if labeled else "reverse_search"]:
        with pytest.raises(ValueError, match="Unknown algorithm"):
            enum(3, algorithm=algorithm)
    with pytest.raises(TypeError):
        enum(3, algorithm=42)
    if labeled:
        with pytest.raises(TypeError):
            enum(3, connected_only=True)
    assert "plantri" in enum.__doc__
    assert legacy in enum.__doc__


def test_maximum_unlabeled_python_size():
    assert len(gr.enumerate_planar_unlabeled_graphs(gr.UNLABELED_ENUM_MAX_N)) == 6966
    assert len(gr.enumerate_planar_unlabeled_graphs(gr.UNLABELED_ENUM_MAX_N, True)) == 5974
