"""Input validation for graph data."""


def validate_graph_input(n, edges):
    """Validate graph input parameters.

    Args:
        n: Number of vertices.
        edges: List of (u, v) tuples (1-indexed).

    Raises:
        TypeError: If types are wrong.
        ValueError: If values are invalid.
    """
    if not isinstance(n, int):
        raise TypeError(
            "n must be an integer, got {}".format(type(n).__name__)
        )
    if n < 0:
        raise ValueError("n must be non-negative, got {}".format(n))
    for i, edge in enumerate(edges):
        if not (hasattr(edge, '__len__') and len(edge) == 2):
            raise ValueError(
                "Edge {} must be a pair (u, v), got {}".format(i, edge)
            )
        u, v = edge
        if not isinstance(u, int) or not isinstance(v, int):
            raise TypeError(
                "Edge vertices must be integers, got ({}, {})".format(
                    type(u).__name__, type(v).__name__
                )
            )
        if u < 1 or u > n or v < 1 or v > n:
            raise ValueError(
                "Edge ({}, {}) has vertices outside range [1, {}]".format(
                    u, v, n
                )
            )
