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


def has_k5_subgraph(n, edge_set):
    """Check if graph contains K5 as a subgraph."""
    if n < 5:
        return False
    vertices = list(range(1, n + 1))
    for five in combinations(vertices, 5):
        all_edges = True
        for i in range(5):
            for j in range(i + 1, 5):
                if (min(five[i], five[j]), max(five[i], five[j])) not in edge_set:
                    all_edges = False
                    break
            if not all_edges:
                break
        if all_edges:
            return True
    return False


def has_k33_subgraph(n, edge_set):
    """Check if graph contains K3,3 as a subgraph."""
    if n < 6:
        return False
    vertices = list(range(1, n + 1))
    for three_a in combinations(vertices, 3):
        remaining = [v for v in vertices if v not in three_a]
        for three_b in combinations(remaining, 3):
            all_edges = True
            for a in three_a:
                for b in three_b:
                    if (min(a, b), max(a, b)) not in edge_set:
                        all_edges = False
                        break
                if not all_edges:
                    break
            if all_edges:
                return True
    return False


def is_planar(n, edges):
    """Planarity check: m <= 3n-6, no K5 subgraph, no K3,3 subgraph."""
    m = len(edges)
    if n >= 3 and m > 3 * n - 6:
        return False
    edge_set = set()
    for u, v in edges:
        edge_set.add((min(u, v), max(u, v)))
    if has_k5_subgraph(n, edge_set):
        return False
    if has_k33_subgraph(n, edge_set):
        return False
    return True


def is_maximal_planar(n, edges):
    """Check if graph is maximal planar (planar + cannot add any edge)."""
    if not is_planar(n, edges):
        return False
    m = len(edges)
    if n >= 3:
        return m == 3 * n - 6
    else:
        # n < 3: must be the complete graph
        return m == n * (n - 1) // 2


def main():
    if len(sys.argv) != 4:
        print("usage: check_maximal_planar_enum.py <in> <exp> <out>")
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
        if not is_maximal_planar(n, edges):
            print("non-maximal-planar graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
