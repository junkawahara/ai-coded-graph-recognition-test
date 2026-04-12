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

        graphs.append(tuple(edges))

    if len(graphs) != count:
        return None, "count mismatch: header={} body={}".format(count, len(graphs))
    if len(set(graphs)) != len(graphs):
        return None, "duplicate graphs detected"
    return graphs, None


def is_connected(n, adj, exclude):
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
    """Simple planarity check: m <= 3n-6 and no K5 or K3,3 minor (brute force for small n)."""
    if n <= 4:
        return True
    m = len(edges)
    if m > 3 * n - 6:
        return False
    # For small n, check K5 and K3,3 subgraph (not minor, but sufficient for checker)
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)
    # Check K5 subgraph
    verts = list(range(1, n + 1))
    if n >= 5:
        from itertools import combinations
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
    # Check K3,3 subgraph
    if n >= 6:
        for s in combinations(verts, 6):
            from itertools import combinations as comb2
            for part in comb2(range(6), 3):
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
    """Check if graph on n vertices with given edges is 3-connected.

    3-connected: n >= 4, connected, no separating pair of vertices.
    """
    if n < 4:
        return False

    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    # Check min degree >= 3
    for v in range(1, n + 1):
        if len(adj[v]) < 3:
            return False

    # Check connectivity
    if not is_connected(n, adj, set()):
        return False

    # Check no separating pair
    for u in range(1, n + 1):
        for v in range(u + 1, n + 1):
            if not is_connected(n, adj, {u, v}):
                return False

    return True


def is_polyhedral(n, edges):
    """Check if graph is a polyhedral graph (3-connected planar)."""
    return is_planar(n, edges) and is_triconnected(n, edges)


def main():
    if len(sys.argv) != 4:
        print("usage: check_polyhedral_enum.py <in> <exp> <out>")
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
        if not is_polyhedral(n, edges):
            print("non-polyhedral graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
