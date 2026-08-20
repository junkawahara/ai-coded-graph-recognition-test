"""Optional NetworkX integration."""


def _is_networkx_graph(obj):
    """Check if obj is ANY networkx graph object without importing networkx.

    Deliberately matches DiGraph, MultiGraph, MultiDiGraph, and
    subclasses as well: they must be dispatched to ``from_networkx`` so
    it can reject them with a meaningful TypeError. An exact
    class-name match would silently drop them into the integer branch
    and produce a misleading "edges is required" error instead.
    """
    for cls in type(obj).__mro__:
        module = getattr(cls, '__module__', '') or ''
        if module.startswith("networkx"):
            return True
    return False


def _is_networkx_digraph(obj):
    """Check if obj is a directed networkx graph (incl. subclasses)."""
    is_directed = getattr(obj, "is_directed", None)
    return _is_networkx_graph(obj) and callable(is_directed) and obj.is_directed()


def _is_networkx_multigraph(obj):
    """Check if obj is a networkx multigraph (incl. subclasses)."""
    is_multi = getattr(obj, "is_multigraph", None)
    return _is_networkx_graph(obj) and callable(is_multi) and obj.is_multigraph()


def from_networkx_directed(G):
    """Convert a networkx.DiGraph to (n, arcs) with 1-indexed vertices.

    For the directed recognizers (digraph, poset, tournament). Arc
    direction is preserved. Self-loops are passed through: the C++
    checkers detect them and answer NO, matching the CLI.

    Args:
        G: A networkx.DiGraph instance.

    Returns:
        Tuple of (n, arcs) where arcs is a list of (u, v) tuples with
        1-indexed integers, u -> v.

    Raises:
        TypeError: If G is not a directed graph, or is a MultiDiGraph.
    """
    if _is_networkx_multigraph(G):
        raise TypeError(
            "MultiGraph and MultiDiGraph are not supported; pass a simple "
            "networkx.DiGraph (the C++ recognizers assume no parallel arcs)"
        )
    if not _is_networkx_digraph(G):
        raise TypeError(
            "this recognizer interprets edges as directed arcs; pass a "
            "networkx.DiGraph (an undirected Graph would be oriented by "
            "edge-iteration order, which is rarely what you want)"
        )
    nodes = list(G.nodes())
    node_to_idx = {node: i + 1 for i, node in enumerate(nodes)}
    n = len(nodes)
    arcs = [(node_to_idx[u], node_to_idx[v]) for u, v in G.edges()]
    return n, arcs


def from_networkx(G):
    """Convert a networkx.Graph to (n, edges) with 1-indexed vertices.

    Supports arbitrary hashable node types by internal renumbering. Vertex
    indices follow the iteration order of ``G.nodes()`` (NetworkX uses
    insertion-ordered dicts since Python 3.7), which is deterministic but
    depends on how the input graph was constructed.

    Args:
        G: A networkx.Graph instance (undirected only).

    Returns:
        Tuple of (n, edges) where n is the number of vertices and
        edges is a list of (u, v) tuples with 1-indexed integers.

    Raises:
        TypeError: If G is a DiGraph, MultiGraph, MultiDiGraph, or other
            unsupported type.
        ValueError: If G contains a self-loop.
    """
    if _is_networkx_multigraph(G):
        raise TypeError(
            "MultiGraph and MultiDiGraph are not supported; pass a simple "
            "networkx.Graph (the C++ recognizers assume no parallel edges)"
        )
    if _is_networkx_digraph(G):
        raise TypeError(
            "DiGraph is not supported; pass an undirected networkx.Graph"
        )
    nodes = list(G.nodes())
    node_to_idx = {node: i + 1 for i, node in enumerate(nodes)}
    n = len(nodes)
    edges = []
    for u, v in G.edges():
        if u == v:
            raise ValueError(
                "Self-loop at node {!r} is not supported".format(u)
            )
        edges.append((node_to_idx[u], node_to_idx[v]))
    return n, edges
