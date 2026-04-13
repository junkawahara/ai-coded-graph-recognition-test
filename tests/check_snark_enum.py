#!/usr/bin/env python3
"""Checker for snark graph enumeration.

Validates that every output graph satisfies ALL snark properties:
1. Cubic (3-regular)
2. Girth >= 5 (no cycles of length 3 or 4)
3. Bridgeless (2-edge-connected)
4. Cyclically 4-edge-connected
5. NOT 3-edge-colorable (chromatic index = 4)
"""
import sys
from collections import deque


def read_n(path):
    with open(path, "r", encoding="ascii") as f:
        data = f.read().strip().split()
    if len(data) < 1:
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

        graphs.append(tuple(edges))

    if len(graphs) != count:
        return None, "count mismatch: header={} body={}".format(count, len(graphs))
    if len(set(graphs)) != len(graphs):
        return None, "duplicate graphs detected"
    return graphs, None


def is_cubic(n, edges):
    """Check if graph is 3-regular (cubic)."""
    deg = [0] * (n + 1)
    for u, v in edges:
        deg[u] += 1
        deg[v] += 1
    for v in range(1, n + 1):
        if deg[v] != 3:
            return False
    return True


def compute_girth(n, edges):
    """Compute the girth of the graph. Returns float('inf') if acyclic."""
    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)
    min_cycle = float('inf')
    for s in range(1, n + 1):
        dist = [-1] * (n + 1)
        parent = [-1] * (n + 1)
        dist[s] = 0
        q = deque([s])
        while q:
            u = q.popleft()
            for w in adj[u]:
                if dist[w] == -1:
                    dist[w] = dist[u] + 1
                    parent[w] = u
                    q.append(w)
                elif w != parent[u]:
                    cycle_len = dist[u] + dist[w] + 1
                    if cycle_len < min_cycle:
                        min_cycle = cycle_len
    return min_cycle


def is_bridgeless(n, edges):
    """Check that removing any single edge keeps the graph connected."""
    adj_set = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj_set[u].add(v)
        adj_set[v].add(u)
    for eu, ev in edges:
        visited = [False] * (n + 1)
        q = deque([1])
        visited[1] = True
        cnt = 1
        while q:
            u = q.popleft()
            for w in adj_set[u]:
                if visited[w]:
                    continue
                if (u == eu and w == ev) or (u == ev and w == eu):
                    continue
                visited[w] = True
                cnt += 1
                q.append(w)
        if cnt != n:
            return False
    return True


def is_cyc4ec(n, edges):
    """Check cyclically 4-edge-connected for a cubic graph.

    For each vertex subset S with 2 <= |S| <= n-2,
    check that cross-edges = 3|S| - 2|E(G[S])| >= 4.
    """
    adj = [[False] * (n + 1) for _ in range(n + 1)]
    for u, v in edges:
        adj[u][v] = True
        adj[v][u] = True
    verts = list(range(1, n + 1))
    for mask in range(1, 1 << n):
        sz = bin(mask).count('1')
        if sz < 2 or sz > n - 2:
            continue
        if sz > n // 2:
            continue
        S = [verts[i] for i in range(n) if (mask >> i) & 1]
        internal = 0
        for i in range(len(S)):
            for j in range(i + 1, len(S)):
                if adj[S[i]][S[j]]:
                    internal += 1
        cross = 3 * sz - 2 * internal
        if cross < 4:
            return False
    return True


def is_3_edge_colorable(n, edges):
    """Backtracking 3-edge-coloring check.

    Returns True if edges can be properly colored with 3 colors.
    """
    m = len(edges)
    vc = [0] * (n + 1)  # bitmask of used colors per vertex

    def try_color(idx):
        if idx == m:
            return True
        u, v = edges[idx]
        for c in range(3):
            bit = 1 << c
            if (vc[u] & bit) or (vc[v] & bit):
                continue
            vc[u] |= bit
            vc[v] |= bit
            if try_color(idx + 1):
                return True
            vc[u] &= ~bit
            vc[v] &= ~bit
        return False

    return try_color(0)


def is_snark(n, edges):
    """Check all 5 snark properties."""
    if not is_cubic(n, edges):
        return False, "not cubic"
    g = compute_girth(n, edges)
    if g < 5:
        return False, "girth {} < 5".format(g)
    if not is_bridgeless(n, edges):
        return False, "has bridge"
    if not is_cyc4ec(n, edges):
        return False, "not cyclically 4-edge-connected"
    if is_3_edge_colorable(n, edges):
        return False, "3-edge-colorable (not a snark)"
    return True, None


def main():
    if len(sys.argv) != 4:
        print("usage: check_snark_enum.py <in> <exp> <out>")
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

    for i, edges in enumerate(graphs):
        ok, reason = is_snark(n, edges)
        if not ok:
            print("graph {}: {}".format(i + 1, reason))
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
