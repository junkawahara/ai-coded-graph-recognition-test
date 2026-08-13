#!/usr/bin/env python3
import sys
from collections import deque


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
    return graphs, None


def is_tree(n, edges):
    """Check if graph is a tree (connected acyclic graph)."""
    if n == 1:
        return len(edges) == 0
    if len(edges) != n - 1:
        return False

    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)

    visited = [False] * (n + 1)
    queue = deque([1])
    visited[1] = True
    count = 1
    while queue:
        u = queue.popleft()
        for v in adj[u]:
            if not visited[v]:
                visited[v] = True
                count += 1
                queue.append(v)
    return count == n


def find_centroids(n, adj):
    """Find centroid(s) of a tree by iterative leaf removal."""
    if n == 1:
        return [1]
    if n == 2:
        return [1, 2]

    degree = [0] * (n + 1)
    for v in range(1, n + 1):
        degree[v] = len(adj[v])

    leaves = [v for v in range(1, n + 1) if degree[v] == 1]
    removed = [False] * (n + 1)
    remaining = n

    while remaining > 2:
        new_leaves = []
        for v in leaves:
            removed[v] = True
            remaining -= 1
            for u in adj[v]:
                if not removed[u]:
                    degree[u] -= 1
                    if degree[u] == 1:
                        new_leaves.append(u)
        leaves = new_leaves

    return [v for v in range(1, n + 1) if not removed[v]]


def ahu_hash(root, parent, adj):
    """Compute AHU canonical hash for a rooted tree."""
    children_hashes = []
    for child in adj[root]:
        if child == parent:
            continue
        children_hashes.append(ahu_hash(child, root, adj))
    children_hashes.sort()
    return tuple(children_hashes)


def canonical_tree_hash(n, edges):
    """Compute a canonical hash for a free tree using centroid + AHU."""
    if n == 1:
        return ((),)

    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)

    centroids = find_centroids(n, adj)

    if len(centroids) == 1:
        return (ahu_hash(centroids[0], -1, adj),)
    else:
        c1, c2 = centroids[0], centroids[1]
        h1 = ahu_hash(c1, c2, adj)
        h2 = ahu_hash(c2, c1, adj)
        return tuple(sorted([h1, h2]))


def main():
    if len(sys.argv) != 4:
        print("usage: check_tree_enum.py <in> <exp> <out>")
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

    # Validate each graph is a tree
    for idx, edges in enumerate(graphs):
        if not is_tree(n, edges):
            print("graph {} is not a tree: {}".format(idx + 1, edges))
            return 1

    # Check no two trees are isomorphic (using AHU canonical hash)
    hashes = set()
    for idx, edges in enumerate(graphs):
        h = canonical_tree_hash(n, edges)
        if h in hashes:
            print("duplicate tree (isomorphic) found at graph {}".format(idx + 1))
            return 1
        hashes.add(h)

    return 0


if __name__ == "__main__":
    sys.exit(main())
