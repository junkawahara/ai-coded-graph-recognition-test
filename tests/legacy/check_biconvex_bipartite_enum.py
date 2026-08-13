#!/usr/bin/env python3
import sys
from collections import deque
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


def is_bipartite(n, edges):
    """BFS 2-coloring check."""
    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)

    color = [-1] * (n + 1)
    for s in range(1, n + 1):
        if color[s] >= 0:
            continue
        color[s] = 0
        queue = deque([s])
        while queue:
            u = queue.popleft()
            for v in adj[u]:
                if color[v] < 0:
                    color[v] = 1 - color[u]
                    queue.append(v)
                elif color[v] == color[u]:
                    return False, None
    return True, color


def has_c1p(rows, num_cols):
    """Check consecutive ones property by trying all column permutations."""
    if num_cols == 0:
        return True
    for perm in permutations(range(num_cols)):
        pos = {perm[i]: i for i in range(num_cols)}
        ok = True
        for row in rows:
            if not row:
                continue
            positions = sorted(pos[c] for c in row)
            if positions[-1] - positions[0] + 1 != len(positions):
                ok = False
                break
        if ok:
            return True
    return False


def check_both_sides(adj, x_verts, y_verts):
    """Check C1P on both sides simultaneously."""
    # C1P on y_verts (columns=y, rows=x)
    if y_verts:
        col_idx = {v: i for i, v in enumerate(y_verts)}
        rows = []
        for u in x_verts:
            row = [col_idx[v] for v in adj[u] if v in col_idx]
            rows.append(row)
        if not has_c1p(rows, len(y_verts)):
            return False
    # C1P on x_verts (columns=x, rows=y)
    if x_verts:
        col_idx = {v: i for i, v in enumerate(x_verts)}
        rows = []
        for u in y_verts:
            row = [col_idx[v] for v in adj[u] if v in col_idx]
            rows.append(row)
        if not has_c1p(rows, len(x_verts)):
            return False
    return True


def is_biconvex_bipartite(n, edges):
    """Check if graph is biconvex bipartite (both sides C1P)."""
    bip, color = is_bipartite(n, edges)
    if not bip:
        return False

    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)

    side0 = [v for v in range(1, n + 1) if color[v] == 0]
    side1 = [v for v in range(1, n + 1) if color[v] == 1]

    if check_both_sides(adj, side0, side1):
        return True
    if check_both_sides(adj, side1, side0):
        return True
    return False


def main():
    if len(sys.argv) != 4:
        print("usage: check_biconvex_bipartite_enum.py <in> <exp> <out>")
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
        if not is_biconvex_bipartite(n, edges):
            print("non-biconvex-bipartite graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
