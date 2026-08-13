#!/usr/bin/env python3
import sys
from collections import deque


def read_n(path):
    with open(path, "r", encoding="ascii") as f:
        data = f.read().strip().split()
    if not data:
        return None
    return int(data[0])


def read_expected_count(path):
    with open(path, "r", encoding="ascii") as f:
        txt = f.read().strip()
    return int(txt)


def parse_output(path, n):
    with open(path, "r", encoding="ascii") as f:
        lines = [ln.strip() for ln in f.readlines() if ln.strip() != ""]
    if not lines:
        return None, "empty output"

    try:
        count = int(lines[0])
    except ValueError:
        return None, "first line must be an integer count"
    if count < 0:
        return None, "count must be non-negative"

    graphs = []
    for line_no, ln in enumerate(lines[1:], start=2):
        parts = ln.split()
        vals = []
        for x in parts:
            try:
                vals.append(int(x))
            except ValueError:
                return None, "line {} contains non-integer token".format(line_no)
        if not vals:
            return None, "line {} is empty".format(line_no)
        m = vals[0]
        if m < 0:
            return None, "line {} has negative edge count".format(line_no)
        if len(vals) != 1 + 2 * m:
            return None, "line {} does not match m u1 v1 ... format".format(line_no)

        edge_set = set()
        edges = []
        for i in range(m):
            u = vals[1 + 2 * i]
            v = vals[2 + 2 * i]
            if not (1 <= u <= n and 1 <= v <= n):
                return None, "line {} has out-of-range endpoint".format(line_no)
            if u >= v:
                return None, "line {} must satisfy u < v".format(line_no)
            if (u, v) in edge_set:
                return None, "line {} has duplicate edge".format(line_no)
            edge_set.add((u, v))
            edges.append((u, v))

        graphs.append(tuple(sorted(edges)))

    if len(graphs) != count:
        return None, "count mismatch: header={} body={}".format(count, len(graphs))
    if len(set(graphs)) != len(graphs):
        return None, "duplicate graphs detected"
    return graphs, None


def _golumbic_orientation(edge_set):
    """Transitive orientation via Golumbic's G-decomposition, or None.

    Repeatedly orient the implication class of an unoriented edge within
    the remaining graph: (x,y) forces (x,c) when yc is a non-edge and
    forces (c,y) when xc is a non-edge. By the TRO theorem this
    succeeds iff the graph is a comparability graph. The result is
    additionally verified for transitivity, so a returned orientation
    is always a genuine certificate (no size fallback).
    """
    adj = {}
    remaining = set()
    for u, v in edge_set:
        adj.setdefault(u, set()).add(v)
        adj.setdefault(v, set()).add(u)
        remaining.add((u, v) if u < v else (v, u))

    orient = {}
    while remaining:
        a, b = min(remaining)
        cls = set([(a, b)])
        stack = [(a, b)]
        while stack:
            x, y = stack.pop()
            for c in adj[x]:
                if c != y and c not in adj[y]:
                    key = (x, c) if x < c else (c, x)
                    if key in remaining and (x, c) not in cls:
                        cls.add((x, c))
                        stack.append((x, c))
            for c in adj[y]:
                if c != x and c not in adj[x]:
                    key = (c, y) if c < y else (y, c)
                    if key in remaining and (c, y) not in cls:
                        cls.add((c, y))
                        stack.append((c, y))
        for x, y in cls:
            if (y, x) in cls:
                return None
        for x, y in cls:
            orient[(x, y)] = True
            remaining.discard((x, y) if x < y else (y, x))

    for a, b in list(orient):
        for c in adj[b]:
            if (b, c) in orient and (a, c) not in orient:
                return None
    return orient


def find_transitive_orientation(n, edge_set):
    """Find a transitive orientation of undirected edges, or return None.

    Returns a dict orient where (a, b) in orient means a -> b.
    """
    return _golumbic_orientation(edge_set)


def check_interval_dimension_leq2(n, orient):
    """Check if poset given by orient has interval dimension <= 2.

    orient: dict where (x, y) in orient means x <_P y.
    Returns True if idim(P) <= 2 (incompatibility graph of B(P) is bipartite).
    """
    # Build B(P): directed edges (x, y) where x != y and NOT x <_P y
    b_edges = []
    for x in range(1, n + 1):
        for y in range(1, n + 1):
            if x == y:
                continue
            if (x, y) in orient:
                continue
            b_edges.append((x, y))

    m = len(b_edges)
    if m == 0:
        return True

    # Build incompatibility graph on B(P) edges
    # Two edges (x1,y1) and (x2,y2) are incompatible if
    # all four endpoints are distinct and x1 <_P y2 and x2 <_P y1
    inc_adj = [[] for _ in range(m)]
    for i in range(m):
        x1, y1 = b_edges[i]
        for j in range(i + 1, m):
            x2, y2 = b_edges[j]
            if x1 == x2 or x1 == y2 or y1 == x2 or y1 == y2:
                continue
            if (x1, y2) in orient and (x2, y1) in orient:
                inc_adj[i].append(j)
                inc_adj[j].append(i)

    # BFS bipartiteness check
    color = [-1] * m
    for i in range(m):
        if color[i] != -1:
            continue
        color[i] = 0
        queue = deque([i])
        while queue:
            u = queue.popleft()
            for j in inc_adj[u]:
                if color[j] == -1:
                    color[j] = 1 - color[u]
                    queue.append(j)
                elif color[j] == color[u]:
                    return False
    return True


def is_trapezoid(n, edges):
    """Check if graph is a trapezoid graph."""
    if n <= 1:
        return True

    edge_set = set()
    for u, v in edges:
        edge_set.add((min(u, v), max(u, v)))

    # Build complement edges (both directions for orientation search)
    complement_edges = set()
    for u in range(1, n + 1):
        for v in range(u + 1, n + 1):
            if (u, v) not in edge_set:
                complement_edges.add((u, v))
                complement_edges.add((v, u))

    # Step 1: Find transitive orientation of complement (co-comparability check)
    orient = find_transitive_orientation(n, complement_edges)
    if orient is None:
        return False

    # Step 2: Check interval dimension <= 2
    return check_interval_dimension_leq2(n, orient)


def main():
    if len(sys.argv) != 4:
        print("usage: check_trapezoid_enum.py <in> <exp> <out>")
        return 2

    in_path, exp_path, out_path = sys.argv[1:4]
    n = read_n(in_path)
    if n is None:
        print("bad input file")
        return 2

    try:
        expected = read_expected_count(exp_path)
    except ValueError:
        print("bad expected file")
        return 2

    graphs, err = parse_output(out_path, n)
    if err is not None:
        print(err)
        return 1

    if len(graphs) != expected:
        print("mismatch: expected count", expected, "got", len(graphs))
        return 1

    for edges in graphs:
        if not is_trapezoid(n, edges):
            print("non-trapezoid graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
