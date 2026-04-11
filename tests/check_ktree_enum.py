#!/usr/bin/env python3
import sys


def read_nk(path):
    with open(path, "r", encoding="ascii") as f:
        data = f.read().strip().split()
    if len(data) < 2:
        return None, None
    return int(data[0]), int(data[1])


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


def is_ktree(n, k, edges):
    """Check if graph is a k-tree by iterative simplicial vertex removal."""
    if k < 0:
        return False
    if n < k + 1:
        return n == 0 and len(edges) == 0
    if n == k + 1:
        # Must be K_{k+1}
        return len(edges) == k * (k + 1) // 2

    # Check expected edge count: k*n - k*(k+1)/2
    expected_edges = k * n - k * (k + 1) // 2
    if len(edges) != expected_edges:
        return False

    # Build adjacency
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    alive = [False] * (n + 1)
    for v in range(1, n + 1):
        alive[v] = True
    remaining = n

    # Iteratively remove simplicial vertices of degree k
    while remaining > k + 1:
        found = False
        for v in range(1, n + 1):
            if not alive[v]:
                continue
            nbrs = [u for u in adj[v] if alive[u]]
            if len(nbrs) != k:
                continue
            # Check nbrs form a clique
            is_clique = True
            for i in range(len(nbrs)):
                for j in range(i + 1, len(nbrs)):
                    if nbrs[j] not in adj[nbrs[i]]:
                        is_clique = False
                        break
                if not is_clique:
                    break
            if is_clique:
                alive[v] = False
                for u in nbrs:
                    adj[u].discard(v)
                remaining -= 1
                found = True
                break
        if not found:
            return False

    # Check remaining vertices form K_{k+1}
    rem_verts = [v for v in range(1, n + 1) if alive[v]]
    if len(rem_verts) != k + 1:
        return False
    for i in range(len(rem_verts)):
        for j in range(i + 1, len(rem_verts)):
            if rem_verts[j] not in adj[rem_verts[i]]:
                return False
    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_ktree_enum.py <in> <exp> <out>")
        return 2

    in_path, exp_path, out_path = sys.argv[1:4]
    n, k = read_nk(in_path)
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
        if not is_ktree(n, k, edges):
            print("non-ktree graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
