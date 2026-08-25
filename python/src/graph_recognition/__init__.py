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

The decomposition functions return the structure behind such an answer::

    from graph_recognition import modular_decomposition, split_decomposition

    modular_decomposition(4, [(1, 2), (2, 3), (3, 4)])["nodes"][0]["kind"]
    # 'prime' -- P4 has no non-trivial module
"""

__version__ = "0.1.0"

from graph_recognition._types import ALGORITHMS, DISPLAY_NAMES, GRAPH_TYPES
from graph_recognition._validation import validate_graph_input
from graph_recognition._networkx import (
    _is_networkx_graph,
    from_networkx,
    from_networkx_directed,
)
from graph_recognition._decompositions import (
    DECOMPOSITIONS,
    block_cut_tree,
    clique_tree,
    co_components,
    connected_components,
    consecutive_ones,
    cotree,
    indifference_tree_layout,
    modular_decomposition,
    permutation_realizer,
    planar_embedding,
    split_decomposition,
    spqr_tree,
    strong_elimination_ordering,
    transitive_orientation,
    tree_decomposition,
    twin_quotient,
)
import graph_recognition._core as _core


# These recognizers interpret the edge list as directed arcs. Their C++
# checkers detect self-loops, out-of-range endpoints, and duplicate arcs
# themselves (answering NO), so input validation must not pre-empt them.
DIRECTED_TYPES = frozenset(["digraph", "poset", "tournament"])


def _normalize_input(type_name, n_or_graph, edges):
    """Shared (n, edges) extraction and validation for is_*/recognize_*."""
    directed = type_name in DIRECTED_TYPES
    if _is_networkx_graph(n_or_graph):
        if edges is not None:
            raise TypeError(
                "When passing a NetworkX graph, 'edges' must not be given"
            )
        if directed:
            n, edges_list = from_networkx_directed(n_or_graph)
        else:
            n, edges_list = from_networkx(n_or_graph)
    else:
        n = n_or_graph
        if edges is None:
            raise TypeError(
                "edges is required when n_or_graph is an integer"
            )
        edges_list = list(edges)
    validate_graph_input(n, edges_list, directed=directed)
    return n, edges_list


def _make_is_function(type_name, check_fn, display_name, algorithms):
    """Factory for is_<type> functions."""

    def is_type(n_or_graph, edges=None, **kwargs):
        algorithm = kwargs.pop("algorithm", None)
        if kwargs:
            raise TypeError(
                "unexpected keyword arguments: {}".format(sorted(kwargs))
            )
        n, edges_list = _normalize_input(type_name, n_or_graph, edges)
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
        algorithm = kwargs.pop("algorithm", None)
        if kwargs:
            raise TypeError(
                "unexpected keyword arguments: {}".format(sorted(kwargs))
            )
        n, edges_list = _normalize_input(type_name, n_or_graph, edges)
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


# ============================================================
# Enumeration functions
# ============================================================

# All enumeration types with their display names
_ENUM_TYPES = [
    "at_free",
    "biconvex_bipartite",
    "bipartite",
    "bipartite_permutation",
    "block",
    "cactus",
    "chain",
    "chordal",
    "chordal_bipartite",
    "circular_arc",
    "claw_free",
    "co_chordal",
    "co_comparability",
    "co_interval",
    "cochain",
    "cograph",
    "comparability",
    "convex_bipartite",
    "diamond_free",
    "distance_hereditary",
    "interval",
    "line_graph",
    "outer_planar",
    "perfect",
    "permutation",
    "planar",
    "proper_interval",
    "ptolemaic",
    "series_parallel",
    "split",
    "strongly_chordal",
    "three_leaf_power",
    "threshold",
    "trapezoid",
    "trivially_perfect",
    "weakly_chordal",
]


# chain / cochain / threshold enumerate one representative per isomorphism
# class (that is what the underlying C++ enumerators produce); every other
# enumerator emits labeled graphs.
_NON_ISOMORPHIC_ENUM_TYPES = frozenset(["chain", "cochain", "threshold"])


_ENUM_ALGORITHMS = {
    "chain": "staircase matrix construction",
    "cochain": "complement of chain graph enumeration",
    "cograph": "recursive cotree construction",
    "threshold": "binary string construction",
    "trivially_perfect": "universal vertex decomposition",
}

# Labeled graph-class counts explode super-exponentially (labeled chordal
# graphs alone: n = 7 -> 617675, n = 8 -> ~3.1e7, n = 9 -> ~2.2e9), and
# unlike the streaming CLI these functions materialize every graph as
# Python tuples: n = 8 for the larger classes is an OOM kill, n = 9 runs
# essentially forever. Refuse instead of silently starting either.
ENUM_MAX_N = 6


def _make_enumerate_function(type_name, enum_fn):
    """Factory for enumerate_<type>_graphs functions."""

    display = DISPLAY_NAMES.get(type_name, type_name)
    algo_desc = _ENUM_ALGORITHMS.get(type_name, "reverse search")
    kind = ("non-isomorphic" if type_name in _NON_ISOMORPHIC_ENUM_TYPES
            else "labeled")

    def enumerate_type(n):
        if isinstance(n, bool) or not isinstance(n, int):
            raise TypeError(
                "n must be an integer, got {}".format(type(n).__name__)
            )
        if n < 1:
            raise ValueError("n must be a positive integer, got {}".format(n))
        if n > ENUM_MAX_N:
            raise ValueError(
                "n = {} exceeds the supported maximum {} for enumeration: "
                "the number of labeled graphs explodes super-exponentially "
                "and the result would not fit in memory (use the streaming "
                "C++ API for larger n)".format(n, ENUM_MAX_N)
            )
        return enum_fn(n)

    enumerate_type.__name__ = "enumerate_{}_graphs".format(type_name)
    enumerate_type.__qualname__ = "enumerate_{}_graphs".format(type_name)
    enumerate_type.__doc__ = (
        "Enumerate all {kind} {name} graphs on n vertices by {algo}.\n"
        "\n"
        "Args:\n"
        "    n: Number of vertices (positive integer, at most {maxn}).\n"
        "\n"
        "Returns:\n"
        "    List of (n, edges) tuples where edges is a list of (u, v) pairs.\n"
        "\n"
        "Raises:\n"
        "    ValueError: If n exceeds {maxn} (the materialized result would\n"
        "        not fit in memory).\n"
    ).format(kind=kind, name=display, algo=algo_desc, maxn=ENUM_MAX_N)

    return enumerate_type


for _type_name in _ENUM_TYPES:
    _enum_fn = getattr(_core, "_enumerate_{}".format(_type_name))
    globals()["enumerate_{}_graphs".format(_type_name)] = _make_enumerate_function(
        _type_name, _enum_fn
    )


__all__ = (
    ["__version__"]
    + ["is_{}".format(t) for t in GRAPH_TYPES]
    + ["recognize_{}".format(t) for t in GRAPH_TYPES]
    + ["enumerate_{}_graphs".format(t) for t in _ENUM_TYPES]
    + list(DECOMPOSITIONS)
)
