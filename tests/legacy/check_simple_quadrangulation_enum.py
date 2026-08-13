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


def is_bipartite(n, edges):
    """BFS 2-coloring."""
    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)
    color = [-1] * (n + 1)
    for s in range(1, n + 1):
        if color[s] != -1:
            continue
        color[s] = 0
        q = deque([s])
        while q:
            v = q.popleft()
            for u in adj[v]:
                if color[u] == -1:
                    color[u] = 1 - color[v]
                    q.append(u)
                elif color[u] == color[v]:
                    return False
    return True


def is_connected_excluding(n, adj, exclude):
    """Check if graph on {1,...,n} minus exclude is connected."""
    remaining = [v for v in range(1, n + 1) if v not in exclude]
    if not remaining:
        return True
    vis = set()
    stack = [remaining[0]]
    while stack:
        v = stack.pop()
        if v in vis:
            continue
        vis.add(v)
        for u in adj[v]:
            if u not in exclude and u not in vis:
                stack.append(u)
    return len(vis) == len(remaining)


def is_planar(n, edges):
    """Planarity check: m <= 3n-6 and no K5 or K3,3 subgraph."""
    if n <= 4:
        return True
    m = len(edges)
    if m > 3 * n - 6:
        return False
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)
    from itertools import combinations
    verts = list(range(1, n + 1))
    # K5
    if n >= 5:
        for s in combinations(verts, 5):
            complete = True
            for i in range(5):
                for j in range(i + 1, 5):
                    if s[j] not in adj[s[i]]:
                        complete = False
                        break
                if not complete:
                    break
            if complete:
                return False
    # K3,3
    if n >= 6:
        for s in combinations(verts, 6):
            for part in combinations(range(6), 3):
                a = [s[i] for i in part]
                b = [s[i] for i in range(6) if i not in part]
                complete = True
                for u in a:
                    for v in b:
                        if v not in adj[u]:
                            complete = False
                            break
                    if not complete:
                        break
                if complete:
                    return False
    return True


def is_triconnected(n, edges):
    """3-connected: n >= 4, min-deg >= 3, no separating pair."""
    if n < 4:
        return False
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)
    for v in range(1, n + 1):
        if len(adj[v]) < 3:
            return False
    if not is_connected_excluding(n, adj, set()):
        return False
    for u in range(1, n + 1):
        for v in range(u + 1, n + 1):
            if not is_connected_excluding(n, adj, {u, v}):
                return False
    return True


def is_simple_quadrangulation(n, edges):
    """simple quadrangulation = bipartite + planar + 3-connected + m = 2n-4

    For non-isomorphic enumeration, n is the number of vertices of the
    quadrangulation (dual vertices correspond to faces of the 4-regular
    planar graph).
    """
    # Determine actual vertex count from edges
    verts = set()
    for u, v in edges:
        verts.add(u)
        verts.add(v)
    actual_n = max(verts) if verts else 0
    if actual_n != n:
        return False
    m = len(edges)
    if m != 2 * n - 4:
        return False
    if not is_bipartite(n, edges):
        return False
    if not is_planar(n, edges):
        return False
    if not is_triconnected(n, edges):
        return False
    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_simple_quadrangulation_enum.py <in> <exp> <out>")
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
        if not is_simple_quadrangulation(n, edges):
            print("non-simple-quadrangulation graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
