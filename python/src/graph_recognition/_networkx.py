"""Optional NetworkX integration."""


def _is_networkx_graph(obj):
    """Check if obj is a networkx.Graph without importing networkx."""
    cls = type(obj)
    module = getattr(cls, '__module__', '') or ''
    name = cls.__name__
    return module.startswith("networkx") and name in ("Graph", "DiGraph")


def from_networkx(G):
    """Convert a networkx.Graph to (n, edges) with 1-indexed vertices.

    Supports arbitrary hashable node types by internal renumbering.

    Args:
        G: A networkx.Graph instance.

    Returns:
        Tuple of (n, edges) where n is the number of vertices and
        edges is a list of (u, v) tuples with 1-indexed integers.
    """
    nodes = sorted(G.nodes())
    node_to_idx = {node: i + 1 for i, node in enumerate(nodes)}
    n = len(nodes)
    edges = [(node_to_idx[u], node_to_idx[v]) for u, v in G.edges()]
    return n, edges
