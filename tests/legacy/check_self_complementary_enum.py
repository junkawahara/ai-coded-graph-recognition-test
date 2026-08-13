#!/usr/bin/env python3
import sys
from itertools import permutations


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


def is_self_complementary(n, edges):
    """Check if graph is self-complementary (complement is isomorphic to original)."""
    edge_set = set(edges)

    # Edge count must be n*(n-1)/4
    expected_m = n * (n - 1) // 4
    if len(edge_set) != expected_m:
        return False

    # Build complement
    comp = set()
    for u in range(1, n + 1):
        for v in range(u + 1, n + 1):
            if (u, v) not in edge_set:
                comp.add((u, v))

    # Quick check: degree sequences must match
    deg_orig = [0] * (n + 1)
    deg_comp = [0] * (n + 1)
    for u, v in edge_set:
        deg_orig[u] += 1
        deg_orig[v] += 1
    for u, v in comp:
        deg_comp[u] += 1
        deg_comp[v] += 1
    if sorted(deg_orig[1:]) != sorted(deg_comp[1:]):
        return False

    # Brute-force isomorphism check via permutations
    for perm in permutations(range(1, n + 1)):
        mapped = set()
        for u, v in edges:
            a, b = perm[u - 1], perm[v - 1]
            if a > b:
                a, b = b, a
            mapped.add((a, b))
        if mapped == comp:
            return True
    return False


def main():
    if len(sys.argv) != 4:
        print("usage: check_self_complementary_enum.py <in> <exp> <out>")
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
        if not is_self_complementary(n, edges):
            print("non-self-complementary graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
