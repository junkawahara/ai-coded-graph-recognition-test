#!/usr/bin/env python3
"""
4-leaf power 列挙チェッカー。
各グラフが 4-leaf power であることを critical clique + tree subdivision で検証。
"""
import itertools
import sys
from collections import defaultdict


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


def prufer_decode(seq, n):
    degree = [1] * n
    for x in seq:
        degree[x] += 1
    edges = []
    for x in seq:
        for leaf in range(n):
            if degree[leaf] == 1:
                break
        edges.append((leaf, x))
        degree[leaf] -= 1
        degree[x] -= 1
    remaining = [i for i in range(n) if degree[i] == 1]
    edges.append((remaining[0], remaining[1]))
    return edges


def tree_dist_and_paths(edges, k):
    adj = defaultdict(list)
    for idx, (u, v) in enumerate(edges):
        adj[u].append((v, idx))
        adj[v].append((u, idx))
    dist = [[0] * k for _ in range(k)]
    paths = [[[] for _ in range(k)] for _ in range(k)]
    for s in range(k):
        visited = [False] * k
        visited[s] = True
        parent_edge = [-1] * k
        parent_node = [-1] * k
        queue = [s]
        qi = 0
        while qi < len(queue):
            u = queue[qi]
            qi += 1
            for v, eidx in adj[u]:
                if not visited[v]:
                    visited[v] = True
                    dist[s][v] = dist[s][u] + 1
                    parent_edge[v] = eidx
                    parent_node[v] = u
                    queue.append(v)
        for t in range(k):
            if t == s:
                continue
            path = []
            cur = t
            while cur != s:
                path.append(parent_edge[cur])
                cur = parent_node[cur]
            paths[s][t] = path
    return dist, paths


def check_subdivision(tree_edges, k, Q):
    if k <= 1:
        return True
    dist, paths = tree_dist_and_paths(tree_edges, k)
    num_e = len(tree_edges)
    for i in range(k):
        for j in range(i + 1, k):
            if Q[i][j] and dist[i][j] > 2:
                return False
    upper = [2] * num_e
    for i in range(k):
        for j in range(i + 1, k):
            if not Q[i][j]:
                continue
            path = paths[i][j]
            slack = 2 - dist[i][j]
            if slack == 0:
                for e in path:
                    upper[e] = 0
            else:
                for e in path:
                    upper[e] = min(upper[e], slack)
    for i in range(k):
        for j in range(i + 1, k):
            if Q[i][j]:
                continue
            path = paths[i][j]
            needed = 3 - dist[i][j]
            if needed <= 0:
                continue
            if sum(upper[e] for e in path) < needed:
                return False
    return True


def is_four_leaf_power(n, edges):
    """4-leaf power 判定 (critical clique + tree subdivision)"""
    if n <= 1:
        return True

    # 1-indexed → 0-indexed
    adj = [set() for _ in range(n)]
    for u, v in edges:
        adj[u - 1].add(v - 1)
        adj[v - 1].add(u - 1)

    # Strongly chordal check (simplicial elimination for chordal first)
    remaining = set(range(n))
    for _ in range(n):
        found = False
        for v in sorted(remaining):
            nbrs = [u for u in adj[v] if u in remaining and u != v]
            is_clique = True
            for i in range(len(nbrs)):
                for j in range(i + 1, len(nbrs)):
                    if nbrs[j] not in adj[nbrs[i]]:
                        is_clique = False
                        break
                if not is_clique:
                    break
            if is_clique:
                remaining.remove(v)
                found = True
                break
        if not found:
            return False

    # Critical cliques
    closed_nbr = {}
    for v in range(n):
        closed_nbr[v] = tuple(sorted(adj[v] | {v}))
    groups = defaultdict(list)
    for v in range(n):
        groups[closed_nbr[v]].append(v)
    ccs = list(groups.values())
    k = len(ccs)

    cc_id = [0] * n
    for i, cc in enumerate(ccs):
        for v in cc:
            cc_id[v] = i

    Q = [[False] * k for _ in range(k)]
    for i in range(k):
        Q[i][i] = True
    for i in range(k):
        rep = ccs[i][0]
        for u in adj[rep]:
            j = cc_id[u]
            if j != i:
                Q[i][j] = True
                Q[j][i] = True

    # Consistency check
    for i in range(k):
        rep = ccs[i][0]
        my_size = len(ccs[i])
        external = len(adj[rep]) - (my_size - 1)
        expected = sum(len(ccs[j]) for j in range(k) if j != i and Q[i][j])
        if external != expected:
            return False

    if k == 1:
        return True

    if k == 2:
        te = [(0, 1)]
        return check_subdivision(te, 2, Q)

    for seq in itertools.product(range(k), repeat=k - 2):
        te = prufer_decode(list(seq), k)
        if check_subdivision(te, k, Q):
            return True

    return False


def main():
    if len(sys.argv) != 4:
        print("usage: check_four_leaf_power_enum.py <in> <exp> <out>")
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
        if not is_four_leaf_power(n, edges):
            print("non-four-leaf-power graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
