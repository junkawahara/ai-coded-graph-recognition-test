"""Graph class recognition algorithms.

A Python wrapper for the C++ graph recognition library, providing
functions to test whether a graph belongs to various graph classes.

Example::

    from graph_recognition import is_interval, is_chordal

    # Using (n, edges) format (1-indexed)
    is_interval(4, [(1, 2), (2, 3), (3, 4)])  # True

    # Using NetworkX (requires: pip install graph-recognition[networkx])
    import networkx as nx
    G = nx.path_graph(5)
    is_interval(G)  # True
"""

__version__ = "0.1.0"

from graph_recognition._core import _enumerate_chordal
from graph_recognition._types import ALGORITHMS, DISPLAY_NAMES, GRAPH_TYPES
from graph_recognition._validation import validate_graph_input
from graph_recognition._networkx import _is_networkx_graph, from_networkx
import graph_recognition._core as _core


def _make_is_function(type_name, check_fn, display_name, algorithms):
    """Factory for is_<type> functions."""

    def is_type(n_or_graph, edges=None, **kwargs):
        algorithm = kwargs.get("algorithm", None)
        if _is_networkx_graph(n_or_graph):
            if edges is not None:
                raise TypeError(
                    "When passing a NetworkX Graph, 'edges' must not be given"
                )
            n, edges_list = from_networkx(n_or_graph)
        else:
            n = n_or_graph
            edges_list = list(edges) if edges is not None else []
        validate_graph_input(n, edges_list)
        algo_str = algorithm if algorithm is not None else ""
        return check_fn(n, edges_list, algo_str)

    is_type.__name__ = "is_{}".format(type_name)
    is_type.__qualname__ = "is_{}".format(type_name)

    algo_doc = ", ".join("'{}'".format(a) for a in algorithms)
    is_type.__doc__ = (
        "Check if a graph is a {name} graph.\n"
        "\n"
        "Args:\n"
        "    n_or_graph: Number of vertices (int, 1-indexed) or a\n"
        "        networkx.Graph.\n"
        "    edges: List of (u, v) tuples (1-indexed). Required when\n"
        "        n_or_graph is an int.\n"
        "    algorithm: Algorithm name (str). Available: {algos}.\n"
        "        None for default.\n"
        "\n"
        "Returns:\n"
        "    bool: True if the graph belongs to the class.\n"
    ).format(name=display_name, algos=algo_doc)

    return is_type


def _make_recognize_function(type_name, check_fn, display_name, algorithms):
    """Factory for recognize_<type> functions."""

    def recognize_type(n_or_graph, edges=None, **kwargs):
        algorithm = kwargs.get("algorithm", None)
        if _is_networkx_graph(n_or_graph):
            if edges is not None:
                raise TypeError(
                    "When passing a NetworkX Graph, 'edges' must not be given"
                )
            n, edges_list = from_networkx(n_or_graph)
        else:
            n = n_or_graph
            edges_list = list(edges) if edges is not None else []
        validate_graph_input(n, edges_list)
        algo_str = algorithm if algorithm is not None else ""
        result = check_fn(n, edges_list, algo_str)
        return (result, None)

    recognize_type.__name__ = "recognize_{}".format(type_name)
    recognize_type.__qualname__ = "recognize_{}".format(type_name)

    algo_doc = ", ".join("'{}'".format(a) for a in algorithms)
    recognize_type.__doc__ = (
        "Recognize whether a graph is a {name} graph.\n"
        "\n"
        "Args:\n"
        "    n_or_graph: Number of vertices (int, 1-indexed) or a\n"
        "        networkx.Graph.\n"
        "    edges: List of (u, v) tuples (1-indexed). Required when\n"
        "        n_or_graph is an int.\n"
        "    algorithm: Algorithm name (str). Available: {algos}.\n"
        "        None for default.\n"
        "\n"
        "Returns:\n"
        "    Tuple of (bool, dict or None). The bool indicates membership.\n"
        "    The dict contains a certificate (currently None, reserved\n"
        "    for future use).\n"
    ).format(name=display_name, algos=algo_doc)

    return recognize_type


# Generate all is_* and recognize_* functions
for _type_name in GRAPH_TYPES:
    _check_fn = getattr(_core, "_check_{}".format(_type_name))
    _display = DISPLAY_NAMES[_type_name]
    _algos = ALGORITHMS[_type_name]
    globals()["is_{}".format(_type_name)] = _make_is_function(
        _type_name, _check_fn, _display, _algos
    )
    globals()["recognize_{}".format(_type_name)] = _make_recognize_function(
        _type_name, _check_fn, _display, _algos
    )


def enumerate_chordal_graphs(n):
    """Enumerate all labeled chordal graphs on n vertices by reverse search.

    Args:
        n: Number of vertices (positive integer).

    Returns:
        List of (n, edges) tuples where edges is a list of (u, v) pairs.
    """
    if not isinstance(n, int) or n < 1:
        raise ValueError("n must be a positive integer, got {}".format(n))
    return _enumerate_chordal(n)


__all__ = (
    ["__version__", "enumerate_chordal_graphs"]
    + ["is_{}".format(t) for t in GRAPH_TYPES]
    + ["recognize_{}".format(t) for t in GRAPH_TYPES]
)
