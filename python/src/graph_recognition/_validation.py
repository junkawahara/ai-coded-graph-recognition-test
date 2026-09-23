"""Input validation for graph data."""

import collections.abc
import numbers

#: Largest vertex count the C++ core can represent: the 1-indexed arrays
#: have n + 1 entries, and that size must still fit in a C ``int``
#: (``Graph::max_vertices`` in ``include/util/graph.h``). Larger values are
#: rejected here so they fail with a ValueError instead of an int-conversion
#: TypeError (n > INT_MAX) or an exception from the core (n == INT_MAX).
MAX_VERTICES = 2**31 - 2


def is_integral(x):
    """True for the integer types the package accepts as n / vertex labels.

    Any ``numbers.Integral`` except ``bool``: Python ``int`` and the NumPy
    integer scalars (``np.int64`` ...), which the pybind11 core converts
    through ``__index__``. ``bool`` is excluded because ``True`` as a vertex
    count or a vertex label is almost always a bug.
    """
    return isinstance(x, numbers.Integral) and not isinstance(x, bool)


def validate_graph_input(n, edges, directed=False):
    """Validate graph input parameters.

    Args:
        n: Number of vertices (any ``numbers.Integral`` except ``bool``).
        edges: Iterable of (u, v) pairs (1-indexed). Each pair must be a
            sequence -- a tuple, a list or a NumPy row -- with integral
            endpoints; unordered containers (``set`` / ``frozenset``) are
            rejected because the endpoint order is the arc direction for
            the directed recognizers.
        directed: True for the directed recognizers (digraph, poset,
            tournament). For those, range and self-loop violations are NOT
            rejected here: detecting them is exactly what the C++ checkers
            do (they answer NO), and raising instead would diverge from the
            CLI. Only structural/type errors raise for directed input.

    Raises:
        TypeError: If types are wrong.
        ValueError: If values are invalid.
    """
    if not is_integral(n):
        raise TypeError(
            "n must be an integer, got {}".format(type(n).__name__)
        )
    if n < 0:
        raise ValueError("n must be non-negative, got {}".format(n))
    if n > MAX_VERTICES:
        raise ValueError(
            "n must be at most {} (the C++ vertex limit), got {}".format(
                MAX_VERTICES, n
            )
        )
    for i, edge in enumerate(edges):
        if (isinstance(edge, (collections.abc.Set, collections.abc.Mapping))
                or not hasattr(edge, '__len__')
                or not hasattr(edge, '__getitem__')):
            raise TypeError(
                "Edge {} must be a sequence (u, v), got {}".format(
                    i, type(edge).__name__
                )
            )
        if len(edge) != 2:
            raise ValueError(
                "Edge {} must be a pair (u, v), got {}".format(i, edge)
            )
        u, v = edge
        if not is_integral(u) or not is_integral(v):
            raise TypeError(
                "Edge vertices must be integers, got ({}, {})".format(
                    type(u).__name__, type(v).__name__
                )
            )
        if directed:
            continue
        if u < 1 or u > n or v < 1 or v > n:
            raise ValueError(
                "Edge ({}, {}) has vertices outside range [1, {}]".format(
                    u, v, n
                )
            )
        if u == v:
            raise ValueError(
                "Self-loop ({}, {}) is not allowed".format(u, v)
            )
