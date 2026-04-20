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

        graphs.append(tuple(edges))

    if len(graphs) != count:
        return None, "count mismatch: header={} body={}".format(count, len(graphs))
    return graphs, None


def is_forest(n, edges):
    """Check if graph is a forest (acyclic graph, possibly disconnected)."""
    if n == 0:
        return len(edges) == 0

    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)

    visited = [False] * (n + 1)
    for start in range(1, n + 1):
        if visited[start]:
            continue
        # BFS to check tree property for this component
        queue = deque([(start, 0)])
        visited[start] = True
        comp_vertices = 1
        comp_edges = 0
        while queue:
            u, parent = queue.popleft()
            for v in adj[u]:
                if v == parent:
                    parent = -1  # allow first back-edge to parent
                    continue
                if visited[v]:
                    return False  # cycle detected
                visited[v] = True
                comp_vertices += 1
                queue.append((v, u))
        # Count edges in component
    # A forest has exactly n - (number of components) edges
    # But simpler: just check no cycles (done above) and edge count
    # For a forest: m <= n - 1 and acyclic
    return True


def get_components(n, edges):
    """Get connected components as sorted tuples of (vertex_count, edge_list)."""
    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)

    visited = [False] * (n + 1)
    components = []
    for start in range(1, n + 1):
        if visited[start]:
            continue
        queue = deque([start])
        visited[start] = True
        comp = [start]
        while queue:
            u = queue.popleft()
            for v in adj[u]:
                if not visited[v]:
                    visited[v] = True
                    comp.append(v)
                    queue.append(v)
        components.append(sorted(comp))
    return components


def ahu_hash(root, parent, adj):
    """Compute AHU canonical hash for a rooted tree."""
    children_hashes = []
    for child in adj[root]:
        if child == parent:
            continue
        children_hashes.append(ahu_hash(child, root, adj))
    children_hashes.sort()
    return tuple(children_hashes)


def find_centroids(vertices, adj):
    """Find centroid(s) of a tree."""
    n = len(vertices)
    if n == 1:
        return [vertices[0]]
    if n == 2:
        return sorted(vertices)

    vset = set(vertices)
    degree = {}
    for v in vertices:
        degree[v] = sum(1 for u in adj[v] if u in vset)

    leaves = [v for v in vertices if degree[v] == 1]
    removed = set()
    remaining = n

    while remaining > 2:
        new_leaves = []
        for v in leaves:
            removed.add(v)
            remaining -= 1
            for u in adj[v]:
                if u in vset and u not in removed:
                    degree[u] -= 1
                    if degree[u] == 1:
                        new_leaves.append(u)
        leaves = new_leaves

    return sorted([v for v in vertices if v not in removed])


def canonical_tree_hash(vertices, edges, adj):
    """Compute a canonical hash for a free tree."""
    n = len(vertices)
    if n == 1:
        return ((),)

    centroids = find_centroids(vertices, adj)

    if len(centroids) == 1:
        return (ahu_hash(centroids[0], -1, adj),)
    else:
        c1, c2 = centroids[0], centroids[1]
        h1 = ahu_hash(c1, c2, adj)
        h2 = ahu_hash(c2, c1, adj)
        return tuple(sorted([h1, h2]))


def canonical_forest_hash(n, edges):
    """Compute canonical hash for a forest (multiset of tree hashes)."""
    if n == 0:
        return ()

    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)

    components = get_components(n, edges)
    tree_hashes = []
    for comp in components:
        comp_edges = []
        comp_set = set(comp)
        for u, v in edges:
            if u in comp_set:
                comp_edges.append((u, v))
        tree_hashes.append(canonical_tree_hash(comp, comp_edges, adj))

    tree_hashes.sort()
    return tuple(tree_hashes)


def main():
    if len(sys.argv) != 4:
        print("usage: check_forest_enum.py <in> <exp> <out>")
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

    # Validate each graph is a forest
    for idx, edges in enumerate(graphs):
        if not is_forest(n, edges):
            print("graph {} is not a forest: {}".format(idx + 1, edges))
            return 1

    # Check no two forests are isomorphic
    hashes = set()
    for idx, edges in enumerate(graphs):
        h = canonical_forest_hash(n, edges)
        if h in hashes:
            print("duplicate forest (isomorphic) found at graph {}".format(idx + 1))
            return 1
        hashes.add(h)

    return 0


if __name__ == "__main__":
    sys.exit(main())
