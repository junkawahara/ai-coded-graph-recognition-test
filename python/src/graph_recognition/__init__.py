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


_CERTIFIED_TYPES = frozenset(_core._certified_types())


def _make_recognize_function(type_name, check_fn, display_name, algorithms):
    """Factory for recognize_<type> functions."""

    certified = type_name in _CERTIFIED_TYPES

    def recognize_type(n_or_graph, edges=None, **kwargs):
        algorithm = kwargs.pop("algorithm", None)
        if kwargs:
            raise TypeError(
                "unexpected keyword arguments: {}".format(sorted(kwargs))
            )
        n, edges_list = _normalize_input(type_name, n_or_graph, edges)
        algo_str = algorithm if algorithm is not None else ""
        result = check_fn(n, edges_list, algo_str)
        if result or not certified:
            return (result, None)
        return (result, _core._obstruction(type_name, n, edges_list))

    recognize_type.__name__ = "recognize_{}".format(type_name)
    recognize_type.__qualname__ = "recognize_{}".format(type_name)

    algo_doc = ", ".join("'{}'".format(a) for a in algorithms)
    if certified:
        cert_doc = (
            "Returns:\n"
            "    Tuple of (bool, dict or None). The bool indicates membership.\n"
            "    On a False answer the dict is a NO certificate: the concrete\n"
            "    structure that rules the graph out of the class, with keys\n"
            "    'kind' (e.g. 'hole', 'claw', 'asteroidal_triple'),\n"
            "    'in_complement' (True when the pattern is induced in the\n"
            "    complement), 'vertices' (1-indexed vertex list, ordered as the\n"
            "    kind prescribes) and 'vertex_sets' (branch sets, cycles or\n"
            "    paths; empty for most kinds). The dict is None on a True\n"
            "    answer.\n"
            "\n"
            "    Some classes need more work to produce the witness than to\n"
            "    decide membership, so a False answer can cost noticeably more\n"
            "    here than in is_{tname}().\n"
        ).format(tname=type_name)
    else:
        cert_doc = (
            "Returns:\n"
            "    Tuple of (bool, dict or None). The bool indicates membership.\n"
            "    The dict is always None: this class has no NO certificate yet.\n"
        )
    recognize_type.__doc__ = (
        "Recognize whether a graph is a {name} graph.\n"
        "\n"
        "Args:\n"
        "    n_or_graph: Number of vertices (int, 1-indexed) or a\n"
        "        networkx.Graph.\n"
        "    edges: List of (u, v) tuples (1-indexed). Required when\n"
        "        n_or_graph is an int.\n"
        "    algorithm: Algorithm name (str). Available: {algos}.\n"
        "        None for default. The certificate does not depend on it.\n"
        "\n"
        "{cert}"
    ).format(name=display_name, algos=algo_doc, cert=cert_doc)

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
    "at_free_labeled",
    "biconnected_unlabeled",
    "biconvex_bipartite_labeled",
    "bipartite_labeled",
    "bipartite_permutation_labeled",
    "bipartite_unlabeled",
    "block_labeled",
    "cactus_labeled",
    "chain_unlabeled",
    "chordal_labeled",
    "chordal_bipartite_labeled",
    "chordal_unlabeled",
    "circle_unlabeled",
    "circular_arc_labeled",
    "claw_free_labeled",
    "cluster_unlabeled",
    "co_chordal_labeled",
    "co_chordal_unlabeled",
    "co_comparability_labeled",
    "co_interval_labeled",
    "cochain_unlabeled",
    "cograph_labeled",
    "cograph_unlabeled",
    "comparability_labeled",
    "convex_bipartite_labeled",
    "diamond_free_labeled",
    "distance_hereditary_labeled",
    "distance_hereditary_unlabeled",
    "eulerian_unlabeled",
    "interval_labeled",
    "line_graph_labeled",
    "outer_planar_labeled",
    "perfect_labeled",
    "permutation_labeled",
    "permutation_unlabeled",
    "planar_labeled",
    "planar_unlabeled",
    "proper_interval_labeled",
    "proper_interval_unlabeled",
    "ptolemaic_labeled",
    "ptolemaic_unlabeled",
    "self_complementary_unlabeled",
    "series_parallel_labeled",
    "split_labeled",
    "split_unlabeled",
    "strongly_chordal_labeled",
    "three_leaf_power_labeled",
    "three_leaf_power_unlabeled",
    "threshold_unlabeled",
    "trapezoid_labeled",
    "triangle_free_unlabeled",
    "trivially_perfect_labeled",
    "trivially_perfect_unlabeled",
    "weakly_chordal_labeled",
]


# The *_unlabeled enumerators produce one representative per isomorphism
# class (that is what the underlying C++ enumerators produce); the
# *_labeled ones emit labeled graphs.
_NON_ISOMORPHIC_ENUM_TYPES = frozenset(
    [
        "biconnected_unlabeled",
        "bipartite_unlabeled",
        "chain_unlabeled",
        "chordal_unlabeled",
        "circle_unlabeled",
        "cluster_unlabeled",
        "co_chordal_unlabeled",
        "cochain_unlabeled",
        "cograph_unlabeled",
        "distance_hereditary_unlabeled",
        "eulerian_unlabeled",
        "permutation_unlabeled",
        "planar_unlabeled",
        "proper_interval_unlabeled",
        "ptolemaic_unlabeled",
        "self_complementary_unlabeled",
        "split_unlabeled",
        "three_leaf_power_unlabeled",
        "threshold_unlabeled",
        "triangle_free_unlabeled",
        "trivially_perfect_unlabeled",
    ]
)

# Enumerators whose C++ entry point takes a connected_only flag.
_CONNECTED_ONLY_ENUM_TYPES = frozenset(
    [
        "bipartite_unlabeled",
        "chordal_unlabeled",
        "circle_unlabeled",
        "cluster_unlabeled",
        "cograph_unlabeled",
        "distance_hereditary_unlabeled",
        "eulerian_unlabeled",
        "permutation_unlabeled",
        "planar_unlabeled",
        "proper_interval_unlabeled",
        "ptolemaic_unlabeled",
        "split_unlabeled",
        "three_leaf_power_unlabeled",
        "triangle_free_unlabeled",
        "trivially_perfect_unlabeled",
    ]
)


_ENUM_ALGORITHMS = {
    "biconnected_unlabeled": "McKay canonical augmentation (geng -C style connectivity constraints)",
    "bipartite_unlabeled": "McKay canonical augmentation (genbg style)",
    "chain_unlabeled": "staircase matrix construction",
    "chordal_unlabeled": "McKay canonical augmentation (recognizer-pruned)",
    "circle_unlabeled": "McKay canonical augmentation (canonical deletion)",
    "cluster_unlabeled": "one clique per part of each integer partition of n",
    "co_chordal_unlabeled": "complement of chordal graph enumeration",
    "cochain_unlabeled": "complement of chain graph enumeration",
    "cograph_labeled": "recursive cotree construction",
    "cograph_unlabeled": "recursive cotree construction via the complement duality",
    "distance_hereditary_unlabeled": "pendant/true-twin/false-twin extensions + isomorph rejection",
    "eulerian_unlabeled": "McKay canonical augmentation (forced even-degree last level)",
    "permutation_unlabeled": "McKay canonical augmentation (canonical deletion)",
    "planar_unlabeled": "McKay canonical augmentation (recognizer-pruned)",
    "proper_interval_unlabeled": "Saitoh et al. bracket-string representation",
    "ptolemaic_unlabeled": "pendant/true-twin/simplicial-false-twin extensions + isomorph rejection",
    "self_complementary_unlabeled": "one complementing permutation per cycle type + isomorph rejection",
    "split_unlabeled": "McKay canonical augmentation (recognizer-pruned)",
    "three_leaf_power_unlabeled": "true-twin/restricted-pendant extensions + isomorph rejection",
    "threshold_unlabeled": "binary string construction",
    "triangle_free_unlabeled": "McKay canonical augmentation (geng -t style)",
    "trivially_perfect_labeled": "universal vertex decomposition",
    "trivially_perfect_unlabeled": "the rooted-forest bijection (ancestor closure)",
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

    supports_connected_only = type_name in _CONNECTED_ONLY_ENUM_TYPES

    def _validate(n):
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

    if supports_connected_only:
        def enumerate_type(n, connected_only=False):
            _validate(n)
            return enum_fn(n, connected_only)
    else:
        def enumerate_type(n):
            _validate(n)
            return enum_fn(n)

    enumerate_type.__name__ = "enumerate_{}_graphs".format(type_name)
    enumerate_type.__qualname__ = "enumerate_{}_graphs".format(type_name)
    connected_only_arg = (
        "    connected_only: If True, emit only connected graphs.\n"
        if supports_connected_only else ""
    )
    enumerate_type.__doc__ = (
        "Enumerate all {kind} {name} graphs on n vertices by {algo}.\n"
        "\n"
        "Args:\n"
        "    n: Number of vertices (positive integer, at most {maxn}).\n"
        "{connected_only_arg}"
        "\n"
        "Returns:\n"
        "    List of (n, edges) tuples where edges is a list of (u, v) pairs.\n"
        "\n"
        "Raises:\n"
        "    ValueError: If n exceeds {maxn} (the materialized result would\n"
        "        not fit in memory).\n"
    ).format(kind=kind, name=display, algo=algo_desc, maxn=ENUM_MAX_N,
             connected_only_arg=connected_only_arg)

    return enumerate_type


for _type_name in _ENUM_TYPES:
    _enum_fn = getattr(_core, "_enumerate_{}".format(_type_name))
    globals()["enumerate_{}_graphs".format(_type_name)] = _make_enumerate_function(
        _type_name, _enum_fn
    )


# ============================================================
# Subgraph enumeration functions
# ============================================================

# Subgraph enumerators take a host graph instead of a vertex count: they
# enumerate the subgraphs of that graph belonging to the class.
_SUBGRAPH_ENUM_TYPES = ["chordal"]

_SUBGRAPH_ENUM_ALGORITHMS = {
    "chordal": "Kiyomi--Uno reverse search",
}

# The output size is driven by the edge count rather than the vertex count:
# every subgraph of a forest is chordal, so a host with m edges can reach 2^m
# subgraphs, each materialized here as Python tuples. The cap keeps that
# worst case in the same order as ENUM_MAX_N does for the fixed-n
# enumerators; use the streaming C++ API for larger hosts.
ENUM_MAX_M = 16


def _make_enumerate_subgraphs_function(type_name, enum_fn):
    """Factory for enumerate_<type>_subgraphs functions."""

    display = DISPLAY_NAMES.get(type_name, type_name)
    algo_desc = _SUBGRAPH_ENUM_ALGORITHMS.get(type_name, "reverse search")

    def enumerate_subgraphs(n_or_graph, edges=None):
        n, edges_list = _normalize_input(type_name, n_or_graph, edges)
        # Count distinct edges: duplicates describe the same simple graph and
        # do not enlarge the output, so they must not trip the guard.
        distinct = len({(min(u, v), max(u, v)) for u, v in edges_list})
        if distinct > ENUM_MAX_M:
            raise ValueError(
                "the host graph has {} edges, which exceeds the supported "
                "maximum {} for subgraph enumeration: a host with m edges "
                "can have up to 2^m {} subgraphs and the result would not "
                "fit in memory (use the streaming C++ API for larger "
                "hosts)".format(distinct, ENUM_MAX_M, display)
            )
        return enum_fn(n, edges_list)

    enumerate_subgraphs.__name__ = "enumerate_{}_subgraphs".format(type_name)
    enumerate_subgraphs.__qualname__ = enumerate_subgraphs.__name__
    enumerate_subgraphs.__doc__ = (
        "Enumerate every {name} subgraph of a graph by {algo}.\n"
        "\n"
        "A {name} subgraph is a spanning subgraph (V, E') with E' a subset\n"
        "of the host's edges such that (V, E') is {name}. The vertex set is\n"
        "fixed and isolated vertices are kept, so the subgraphs are in\n"
        "bijection with the {name} edge subsets of the host and the empty\n"
        "edge set is always among them.\n"
        "\n"
        "Args:\n"
        "    n_or_graph: Number of vertices (int, 1-indexed) or a\n"
        "        networkx.Graph.\n"
        "    edges: List of (u, v) tuples (1-indexed). Required when\n"
        "        n_or_graph is an int.\n"
        "\n"
        "Returns:\n"
        "    List of (n, edges) tuples where edges is a list of (u, v)\n"
        "    pairs.\n"
        "\n"
        "Raises:\n"
        "    ValueError: If the host has more than {maxm} distinct edges\n"
        "        (the materialized result would not fit in memory).\n"
    ).format(name=display, algo=algo_desc, maxm=ENUM_MAX_M)

    return enumerate_subgraphs


for _type_name in _SUBGRAPH_ENUM_TYPES:
    _enum_fn = getattr(_core, "_enumerate_{}_subgraphs".format(_type_name))
    globals()["enumerate_{}_subgraphs".format(_type_name)] = (
        _make_enumerate_subgraphs_function(_type_name, _enum_fn)
    )


# ============================================================
# Induced subgraph enumeration functions
# ============================================================

# Induced subgraph enumerators also take a host graph, but their solutions
# are vertex subsets X with G[X] in the class, so they return vertex lists
# rather than edge lists.
_INDUCED_SUBGRAPH_ENUM_TYPES = ["chordal_bipartite"]

_INDUCED_SUBGRAPH_ENUM_ALGORITHMS = {
    "chordal_bipartite": "Kurita--Wasa--Arimura--Uno ECB reverse search",
}

# The guard is on the vertex count, not the edge count: these classes are
# hereditary, so a host that is itself in the class has all 2^n vertex
# subsets as solutions no matter how sparse it is. The cap keeps that worst
# case in the same order as ENUM_MAX_M does for the spanning-subgraph
# enumerators; use the streaming C++ API for larger hosts.
INDUCED_ENUM_MAX_N = 16


def _make_enumerate_induced_subgraphs_function(type_name, enum_fn):
    """Factory for enumerate_<type>_induced_subgraphs functions."""

    display = DISPLAY_NAMES.get(type_name, type_name)
    algo_desc = _INDUCED_SUBGRAPH_ENUM_ALGORITHMS.get(type_name, "reverse search")

    def enumerate_induced_subgraphs(n_or_graph, edges=None):
        n, edges_list = _normalize_input(type_name, n_or_graph, edges)
        if n > INDUCED_ENUM_MAX_N:
            raise ValueError(
                "the host graph has {} vertices, which exceeds the supported "
                "maximum {} for induced subgraph enumeration: a host with n "
                "vertices can have up to 2^n {} induced subgraphs and the "
                "result would not fit in memory (use the streaming C++ API "
                "for larger hosts)".format(n, INDUCED_ENUM_MAX_N, display)
            )
        return enum_fn(n, edges_list)

    enumerate_induced_subgraphs.__name__ = "enumerate_{}_induced_subgraphs".format(
        type_name
    )
    enumerate_induced_subgraphs.__qualname__ = enumerate_induced_subgraphs.__name__
    enumerate_induced_subgraphs.__doc__ = (
        "Enumerate every {name} induced subgraph of a graph by {algo}.\n"
        "\n"
        "The solutions are the vertex subsets X of the host for which the\n"
        "induced subgraph G[X] is {name}, reported as sorted vertex lists.\n"
        "The empty set is always a solution and comes first. Because the\n"
        "class is hereditary, a host that is itself {name} makes every one\n"
        "of its 2^n subsets a solution.\n"
        "\n"
        "Args:\n"
        "    n_or_graph: Number of vertices (int, 1-indexed) or a\n"
        "        networkx.Graph.\n"
        "    edges: List of (u, v) tuples (1-indexed). Required when\n"
        "        n_or_graph is an int.\n"
        "\n"
        "Returns:\n"
        "    List of vertex lists (1-indexed, each sorted ascending).\n"
        "\n"
        "Raises:\n"
        "    ValueError: If the host has more than {maxn} vertices (the\n"
        "        materialized result would not fit in memory).\n"
    ).format(name=display, algo=algo_desc, maxn=INDUCED_ENUM_MAX_N)

    return enumerate_induced_subgraphs


for _type_name in _INDUCED_SUBGRAPH_ENUM_TYPES:
    _enum_fn = getattr(
        _core, "_enumerate_{}_induced_subgraphs".format(_type_name)
    )
    globals()["enumerate_{}_induced_subgraphs".format(_type_name)] = (
        _make_enumerate_induced_subgraphs_function(_type_name, _enum_fn)
    )


__all__ = (
    ["__version__"]
    + ["is_{}".format(t) for t in GRAPH_TYPES]
    + ["recognize_{}".format(t) for t in GRAPH_TYPES]
    + ["enumerate_{}_graphs".format(t) for t in _ENUM_TYPES]
    + ["enumerate_{}_subgraphs".format(t) for t in _SUBGRAPH_ENUM_TYPES]
    + [
        "enumerate_{}_induced_subgraphs".format(t)
        for t in _INDUCED_SUBGRAPH_ENUM_TYPES
    ]
    + list(DECOMPOSITIONS)
)
