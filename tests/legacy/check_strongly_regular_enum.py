#!/usr/bin/env python3
import sys


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


def is_strongly_regular(n, edges):
    """Check if graph is strongly regular (some srg(n,k,lam,mu))."""
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    # All degrees must be equal
    degrees = [len(adj[v]) for v in range(1, n + 1)]
    if not degrees:
        return False
    k = degrees[0]
    for d in degrees:
        if d != k:
            return False

    # Exclude complete and empty graphs
    if k == 0 or k == n - 1:
        return False

    # Check lambda (common neighbors of adjacent pairs)
    lam = None
    for u in range(1, n + 1):
        for v in range(u + 1, n + 1):
            if v in adj[u]:
                cn = len(adj[u] & adj[v])
                if lam is None:
                    lam = cn
                elif cn != lam:
                    return False

    # Check mu (common neighbors of non-adjacent pairs)
    mu_val = None
    for u in range(1, n + 1):
        for v in range(u + 1, n + 1):
            if v not in adj[u]:
                cn = len(adj[u] & adj[v])
                if mu_val is None:
                    mu_val = cn
                elif cn != mu_val:
                    return False

    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_strongly_regular_enum.py <in> <exp> <out>")
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
        if not is_strongly_regular(n, edges):
            print("non-strongly-regular graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
