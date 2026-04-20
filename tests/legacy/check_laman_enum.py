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


def is_laman(n, edges):
    """Check if graph on n vertices with given edges is a Laman graph.

    Laman graph: (2,3)-tight.
    1. Exactly 2n-3 edges (for n >= 2; for n=1: 0 edges)
    2. Every subset S with |S| >= 2 has at most 2|S|-3 edges
    """
    if n <= 0:
        return False
    if n == 1:
        return len(edges) == 0

    # Check edge count (tightness)
    if len(edges) != 2 * n - 3:
        return False

    # Build adjacency for subset checks
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    # Check (2,3)-sparsity: every subset of size k >= 2 has <= 2k-3 edges
    verts = list(range(1, n + 1))
    for k in range(2, n):  # k=n is already checked by edge count
        for subset in combinations(verts, k):
            s = set(subset)
            sub_edges = 0
            for u in subset:
                for v in adj[u]:
                    if v in s and v > u:
                        sub_edges += 1
            if sub_edges > 2 * k - 3:
                return False
    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_laman_enum.py <in> <exp> <out>")
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
        if not is_laman(n, edges):
            print("non-Laman graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
