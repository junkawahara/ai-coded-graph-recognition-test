"""Graph decompositions.

Where the ``is_*`` functions answer whether a graph belongs to a class, these
return the structure behind the answer: the modular decomposition tree, the
split decomposition, the SPQR tree, a planar embedding, and the smaller
pieces the recognizers are built on.

Every result is plain Python data -- dicts, lists and tuples of ints. Vertices
are 1-indexed as everywhere else in this package, and a vertex-indexed list
therefore has ``n + 1`` entries whose index 0 is unused.
"""

from graph_recognition._networkx import _is_networkx_graph, from_networkx
from graph_recognition._validation import validate_graph_input
import graph_recognition._core as _core


def _normalize(n_or_graph, edges):
    """Shared (n, edges) extraction and validation."""
    if _is_networkx_graph(n_or_graph):
        if edges is not None:
            raise TypeError(
                "When passing a NetworkX graph, 'edges' must not be given"
            )
        n, edges_list = from_networkx(n_or_graph)
    else:
        n = n_or_graph
        if edges is None:
            raise TypeError("edges is required when n_or_graph is an integer")
        edges_list = list(edges)
    validate_graph_input(n, edges_list)
    return n, edges_list


def connected_components(n_or_graph, edges=None):
    """Connected components.

    Returns:
        list[list[int]]: the vertices of each component.
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._connected_components(n, e)


def co_components(n_or_graph, edges=None):
    """Connected components of the complement, without building it.

    Returns:
        list[list[int]]: the vertices of each co-component.
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._co_components(n, e)


def twin_quotient(n_or_graph, edges=None, kind="both"):
    """Contract twin classes.

    Two distinct vertices are true twins when N[u] == N[v] and false twins
    when N(u) == N(v). The true-twin classes are the critical cliques.

    Args:
        kind: 'true', 'false', or 'both' (contracted to a fixpoint).

    Returns:
        dict with 'members' (the class of each quotient vertex, in order),
        'block_of' (vertex-indexed quotient vertex) and 'quotient_edges'.
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._twin_quotient(n, e, kind)


def block_cut_tree(n_or_graph, edges=None):
    """Biconnected components, cut vertices, bridges and their incidences.

    An isolated vertex is reported as a block of its own.

    Returns:
        dict with 'blocks', 'block_edges', 'cut_vertices', 'bridges' and
        'tree_edges' (pairs of block index and cut vertex).
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._block_cut_tree(n, e)


def modular_decomposition(n_or_graph, edges=None):
    """Modular decomposition tree.

    Returns:
        dict with 'root' (node index, -1 for the empty graph) and 'nodes',
        each a dict with 'kind' ('leaf', 'series', 'parallel' or 'prime'),
        'vertex', 'parent', 'children' and 'vertices'.
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._modular_decomposition(n, e)


def cotree(n_or_graph, edges=None, algorithm=None):
    """Cotree of a cograph.

    The same tree shape as ``modular_decomposition``, and for a cograph it is
    the same tree: a cotree is a modular decomposition without prime nodes.

    Args:
        algorithm: 'cotree', 'partition_refinement' or 'modular'.

    Returns:
        dict with 'is_cograph', 'root' and 'nodes'. The tree is empty when
        the graph is not a cograph.
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._cotree(n, e, algorithm if algorithm is not None else "")


def transitive_orientation(n_or_graph, edges=None, algorithm=None):
    """A transitive orientation, if the graph is a comparability graph.

    Args:
        algorithm: 'forcing' or 'backtracking'.

    Returns:
        dict with 'is_comparability' and 'orientation' (each edge once, as
        (u, v) meaning u -> v).
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._transitive_orientation(n, e, algorithm if algorithm is not None else "")


def permutation_realizer(n_or_graph, edges=None):
    """A permutation diagram, if the graph is a permutation graph.

    Two vertices are adjacent exactly when their segments cross, that is when
    their order differs between the two lines.

    Returns:
        dict with 'is_permutation', 'pos1', 'pos2' (vertex-indexed positions)
        and 'pi' (the second-line position of the vertex at each first-line
        position).
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._permutation_realizer(n, e)


def clique_tree(n_or_graph, edges=None):
    """Maximal cliques and a clique tree of a chordal graph.

    Returns:
        dict with 'is_chordal', 'cliques' and 'tree_edges' (index pairs).
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._clique_tree(n, e)


def tree_decomposition(n_or_graph, edges=None):
    """Tree decomposition of a chordal graph.

    The clique tree already is an optimal decomposition, so the width
    reported is the treewidth. Fails on non-chordal input: computing
    treewidth in general is NP-hard.

    Returns:
        dict with 'success', 'bags', 'tree_edges' and 'width'.
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._tree_decomposition(n, e)


def split_decomposition(n_or_graph, edges=None):
    """Cunningham's canonical split decomposition of a connected graph.

    Returns:
        dict with 'success' (false for disconnected input),
        'totally_decomposable' (no prime bag, i.e. distance-hereditary),
        'bags' -- each a dict with 'kind' ('clique', 'star' or 'prime'),
        'label' (original vertex per skeleton vertex, 0 for a marker),
        'edges' and 'center' -- and 'tree_edges', each a
        (bag_u, marker_u, bag_v, marker_v) tuple.
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._split_decomposition(n, e)


def spqr_tree(n_or_graph, edges=None):
    """Tutte's decomposition into 3-connected components of a biconnected graph.

    Returns:
        dict with 'success' (false unless the input is biconnected), 'nodes'
        -- each a dict with 'kind' ('S' for a polygon, 'P' for a bond, 'R'
        for a rigid 3-connected skeleton), 'vertices', 'edges' and
        'edge_orig' (index into the input edge list, -1 for a virtual edge)
        -- and 'tree_edges', each a (node_u, edge_u, node_v, edge_v) tuple.
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._spqr_tree(n, e)


def planar_embedding(n_or_graph, edges=None, algorithm=None):
    """A planar embedding: a rotation system and its faces.

    Args:
        algorithm: 'dmp_general' (any planar graph, the default here) or
            'tutte_3connected' (3-connected planar graphs only).

    Returns:
        dict with 'success', 'rotation' (vertex-indexed cyclic neighbour
        orders) and 'faces'.
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._planar_embedding(n, e, algorithm if algorithm is not None else "")


def strong_elimination_ordering(n_or_graph, edges=None):
    """A strong elimination ordering, if the graph is strongly chordal.

    Returns:
        dict with 'success', 'order' (the i-th vertex eliminated) and
        'number' (the position of each vertex).
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._strong_elimination_ordering(n, e)


def indifference_tree_layout(n_or_graph, edges=None):
    """An indifference tree-layout, if the graph is proper chordal.

    Returns:
        dict with 'success' and 'parent' (vertex-indexed, 0 at a root).
    """
    n, e = _normalize(n_or_graph, edges)
    return _core._indifference_tree_layout(n, e)


def consecutive_ones(num_columns, rows):
    """Test the consecutive ones property of a 0/1 matrix.

    Args:
        num_columns: columns are numbered 1 .. num_columns.
        rows: one entry per row, listing the columns holding a 1.

    Returns:
        dict with 'success' and 'column_order' (the column at each position,
        1-indexed, index 0 unused).
    """
    if isinstance(num_columns, bool) or not isinstance(num_columns, int):
        raise TypeError(
            "num_columns must be an integer, got {}".format(type(num_columns).__name__)
        )
    if num_columns < 0:
        raise ValueError("num_columns must be non-negative, got {}".format(num_columns))
    row_lists = [list(r) for r in rows]
    for i, row in enumerate(row_lists):
        for c in row:
            if isinstance(c, bool) or not isinstance(c, int):
                raise TypeError(
                    "Row {} has a non-integer column {}".format(i, c)
                )
    return _core._consecutive_ones(num_columns, row_lists)


DECOMPOSITIONS = [
    "connected_components",
    "co_components",
    "twin_quotient",
    "block_cut_tree",
    "modular_decomposition",
    "cotree",
    "transitive_orientation",
    "permutation_realizer",
    "clique_tree",
    "tree_decomposition",
    "split_decomposition",
    "spqr_tree",
    "planar_embedding",
    "strong_elimination_ordering",
    "indifference_tree_layout",
    "consecutive_ones",
]
