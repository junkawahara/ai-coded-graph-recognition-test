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


def is_caterpillar(n, edges):
    """Check if graph is a caterpillar tree."""
    # Build adjacency list
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    # Check tree: connected and m = n - 1
    if len(edges) != n - 1 and n > 1:
        return False
    if n == 1:
        return len(edges) == 0

    # BFS connectivity check
    visited = [False] * (n + 1)
    queue = deque([1])
    visited[1] = True
    count = 1
    while queue:
        u = queue.popleft()
        for v in adj[u]:
            if not visited[v]:
                visited[v] = True
                count += 1
                queue.append(v)
    if count != n:
        return False

    # Remove all leaves (degree 1 vertices); remaining should be a path (or empty)
    degree = [0] * (n + 1)
    for v in range(1, n + 1):
        degree[v] = len(adj[v])

    # Vertices that are not leaves
    spine = [v for v in range(1, n + 1) if degree[v] != 1]

    if len(spine) == 0:
        # n <= 2: all vertices are leaves (or isolated), valid caterpillar
        return True

    # Check that spine induces a path
    # A path on k vertices has k-1 edges, max degree 2 in the induced subgraph
    spine_set = set(spine)
    spine_adj = {v: [] for v in spine}
    spine_edges = 0
    for v in spine:
        for u in adj[v]:
            if u in spine_set:
                spine_adj[v].append(u)
        if len(spine_adj[v]) > 2:
            return False  # degree > 2 in spine subgraph
    for v in spine:
        spine_edges += len(spine_adj[v])
    spine_edges //= 2

    if spine_edges != len(spine) - 1:
        return False  # not a tree (not connected or has cycle)

    # Check connectivity of spine
    if len(spine) > 1:
        vis = set()
        q = deque([spine[0]])
        vis.add(spine[0])
        while q:
            u = q.popleft()
            for v in spine_adj[u]:
                if v not in vis:
                    vis.add(v)
                    q.append(v)
        if len(vis) != len(spine):
            return False

    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_caterpillar_enum.py <in> <exp> <out>")
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
        if not is_caterpillar(n, edges):
            print("non-caterpillar graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
