"""Recursion-depth guards of the recursive recognizers.

line_graph, circular_arc (and proper_circular_arc, which shares its endpoint
search), meyniel and even_hole_free recurse once per edge / maximal clique /
vertex and used to overflow the default stack on large sparse inputs, which
killed the interpreter with SIGSEGV. They now raise RuntimeError at a
documented limit (the stack-overflow exception to the complexity policy).
"""

import pytest

from graph_recognition import (
    is_circular_arc,
    is_even_hole_free,
    is_line_graph,
    is_meyniel,
    is_proper_circular_arc,
    recognize_meyniel,
)


def path(n):
    return n, [(i, i + 1) for i in range(1, n)]


def star(n):
    return n, [(1, i) for i in range(2, n + 1)]


@pytest.mark.parametrize(
    "fn, n, keyword",
    [
        (is_line_graph, 6001, "recursion limit"),  # m = 6000 edges
        (is_circular_arc, 7001, "maximal cliques"),  # 7000 cliques
        (is_proper_circular_arc, 7000, "recursion limit"),
        (is_meyniel, 17000, "meyniel"),
        (is_even_hole_free, 32000, "even_hole_free"),
    ],
)
def test_limit_raises_runtime_error(fn, n, keyword):
    with pytest.raises(RuntimeError, match=keyword):
        fn(*path(n))


def test_recognize_raises_too():
    with pytest.raises(RuntimeError, match="recursion limit"):
        recognize_meyniel(*path(17000))


def test_just_below_the_limit_answers():
    assert is_line_graph(*path(6000)) is True  # 5999 edges
    assert is_circular_arc(*path(7000)) is True  # 6999 cliques
    assert is_meyniel(*star(16999)) is True
    assert is_even_hole_free(*star(31999)) is True


def test_docstrings_mention_the_condition():
    for fn in (is_line_graph, is_circular_arc, is_proper_circular_arc, is_meyniel, is_even_hole_free):
        assert "RuntimeError" in fn.__doc__, fn.__name__
