#!/usr/bin/env python3
import sys
from collections import deque
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
                    return False
    return True


def has_induced_even_cycle_ge6(n, edges):
    """Check if graph has an induced even cycle of length >= 6."""
    adj_set = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj_set[u].add(v)
        adj_set[v].add(u)

    vertices = list(range(1, n + 1))
    for size in range(6, n + 1, 2):
        for cycle in combinations(vertices, size):
            # Try all orderings as a cycle - check if it forms an induced cycle
            # For efficiency, use the sorted subset and check all cyclic orderings
            from itertools import permutations as perms
            subset = list(cycle)
            sub_set = set(subset)
            # Check if subset induces a cycle of exactly 'size' edges
            # Count edges within subset
            sub_edges = []
            for u in subset:
                for v in subset:
                    if u < v and v in adj_set[u]:
                        sub_edges.append((u, v))
            if len(sub_edges) != size:
                continue
            # Check if these edges form a single Hamiltonian cycle on the subset
            sub_adj = {v: [] for v in subset}
            for u, v in sub_edges:
                sub_adj[u].append(v)
                sub_adj[v].append(u)
            # Every vertex must have degree exactly 2
            if all(len(sub_adj[v]) == 2 for v in subset):
                # Check connectivity (single cycle, not union of cycles)
                visited = set()
                start = subset[0]
                stack = [start]
                while stack:
                    v = stack.pop()
                    if v in visited:
                        continue
                    visited.add(v)
                    for u in sub_adj[v]:
                        if u not in visited:
                            stack.append(u)
                if len(visited) == size:
                    return True
    return False


def is_chordal_bipartite(n, edges):
    """Check if graph is chordal bipartite."""
    if not is_bipartite(n, edges):
        return False
    if has_induced_even_cycle_ge6(n, edges):
        return False
    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_chordal_bipartite_enum.py <in> <exp> <out>")
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
        if not is_chordal_bipartite(n, edges):
            print("non-chordal-bipartite graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
