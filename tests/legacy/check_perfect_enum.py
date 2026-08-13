#!/usr/bin/env python3
import sys
from itertools import combinations


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


def is_induced_cycle(vertices, adj):
    """Check if the given vertices form an induced cycle."""
    k = len(vertices)
    if k < 3:
        return False
    vset = set(vertices)
    for v in vertices:
        cnt = len(adj[v] & vset)
        if cnt != 2:
            return False
    visited = set()
    start = vertices[0]
    cur = start
    prev = -1
    for _ in range(k):
        visited.add(cur)
        nexts = [w for w in adj[cur] if w in vset and w != prev]
        if prev == -1:
            if len(nexts) < 1:
                return False
            nxt = nexts[0]
        else:
            if len(nexts) != 1:
                return False
            nxt = nexts[0]
        prev = cur
        cur = nxt
    return cur == start and len(visited) == k


def has_odd_hole(n, adj):
    """Check for odd induced cycle of length >= 5."""
    for k in range(5, n + 1, 2):
        for subset in combinations(range(1, n + 1), k):
            if is_induced_cycle(list(subset), adj):
                return True
    return False


def is_perfect(n, edges):
    """Check if graph is a perfect graph (SPGT: no odd hole, no odd antihole)."""
    if n <= 4:
        return True

    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    if has_odd_hole(n, adj):
        return False

    comp_adj = [set() for _ in range(n + 1)]
    for u in range(1, n + 1):
        for v in range(u + 1, n + 1):
            if v not in adj[u]:
                comp_adj[u].add(v)
                comp_adj[v].add(u)

    if has_odd_hole(n, comp_adj):
        return False

    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_perfect_enum.py <in> <exp> <out>")
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
        if not is_perfect(n, edges):
            print("non-perfect graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
