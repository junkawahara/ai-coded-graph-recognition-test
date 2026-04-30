"""Optional NetworkX integration."""


def _is_networkx_graph(obj):
    """Check if obj is a networkx.Graph (undirected) without importing networkx."""
    cls = type(obj)
    module = getattr(cls, '__module__', '') or ''
    name = cls.__name__
    return module.startswith("networkx") and name == "Graph"


def _is_networkx_digraph(obj):
    """Check if obj is a networkx.DiGraph without importing networkx."""
    cls = type(obj)
    module = getattr(cls, '__module__', '') or ''
    name = cls.__name__
    return module.startswith("networkx") and name == "DiGraph"


def _is_networkx_multigraph(obj):
    """Check if obj is a networkx.MultiGraph or MultiDiGraph."""
    cls = type(obj)
    module = getattr(cls, '__module__', '') or ''
    name = cls.__name__
    return module.startswith("networkx") and name in ("MultiGraph", "MultiDiGraph")


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
    """
    if _is_networkx_digraph(G):
        raise TypeError(
            "DiGraph is not supported; pass an undirected networkx.Graph"
        )
    if _is_networkx_multigraph(G):
        raise TypeError(
            "MultiGraph and MultiDiGraph are not supported; pass a simple "
            "networkx.Graph (the C++ recognizers assume no parallel edges)"
        )
    nodes = list(G.nodes())
    node_to_idx = {node: i + 1 for i, node in enumerate(nodes)}
    n = len(nodes)
    edges = [(node_to_idx[u], node_to_idx[v]) for u, v in G.edges()]
    return n, edges
