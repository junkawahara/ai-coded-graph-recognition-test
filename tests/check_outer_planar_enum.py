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

        graphs.append(tuple(edges))

    if len(graphs) != count:
        return None, "count mismatch: header={} body={}".format(count, len(graphs))
    if len(set(graphs)) != len(graphs):
        return None, "duplicate graphs detected"
    return graphs, None


def is_two_degenerate(n, edges):
    """2-degeneracy test: K4-minor-free iff every subgraph has a vertex of degree <= 2."""
    degree = [0] * (n + 1)
    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)
        degree[u] += 1
        degree[v] += 1

    alive = [False] + [True] * n
    queue = [v for v in range(1, n + 1) if degree[v] <= 2]
    removed = 0
    qi = 0
    while qi < len(queue):
        v = queue[qi]
        qi += 1
        if not alive[v]:
            continue
        if degree[v] > 2:
            continue
        alive[v] = False
        removed += 1
        for u in adj[v]:
            if alive[u]:
                degree[u] -= 1
                if degree[u] <= 2:
                    queue.append(u)

    return removed == n


def has_k23_subgraph(n, edges):
    """Check if graph contains K2,3 as a subgraph."""
    if n < 5:
        return False
    edge_set = set()
    for u, v in edges:
        edge_set.add((min(u, v), max(u, v)))

    vertices = list(range(1, n + 1))
    for two_side in combinations(vertices, 2):
        a, b = two_side
        remaining = [v for v in vertices if v != a and v != b]
        for three_side in combinations(remaining, 3):
            c, d, e = three_side
            all_edges = True
            for s in (a, b):
                for t in (c, d, e):
                    if (min(s, t), max(s, t)) not in edge_set:
                        all_edges = False
                        break
                if not all_edges:
                    break
            if all_edges:
                return True
    return False


def is_outer_planar(n, edges):
    """Outerplanar = K4-minor-free AND K2,3-minor-free."""
    m = len(edges)
    if n >= 2 and m > 2 * n - 3:
        return False
    if not is_two_degenerate(n, edges):
        return False
    if has_k23_subgraph(n, edges):
        return False
    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_outer_planar_enum.py <in> <exp> <out>")
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
        if not is_outer_planar(n, edges):
            print("non-outer-planar graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
