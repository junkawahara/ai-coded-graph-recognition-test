#!/usr/bin/env python3
"""
3-leaf power 列挙チェッカー。
各グラフが (bull, dart, gem)-free chordal であることを検証。
"""
import itertools
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


def has_induced_subgraph(adj, n, pattern_edges, pattern_n):
    for combo in itertools.permutations(range(1, n + 1), pattern_n):
        ok = True
        for a in range(pattern_n):
            for b in range(a + 1, pattern_n):
                edge_in_g = combo[b] in adj[combo[a]]
                edge_in_p = (a, b) in pattern_edges or (b, a) in pattern_edges
                if edge_in_g != edge_in_p:
                    ok = False
                    break
            if not ok:
                break
        if ok:
            return True
    return False


def is_chordal(adj, n):
    remaining = set(range(1, n + 1))
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
    return True


def is_three_leaf_power(n, edges):
    adj = {v: set() for v in range(1, n + 1)}
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    if not is_chordal(adj, n):
        return False

    # Bull: triangle 1-2-3 with pendants 0-1 and 3-4
    bull = {(0, 1), (1, 2), (2, 3), (1, 3), (3, 4)}
    if has_induced_subgraph(adj, n, bull, 5):
        return False

    # Dart: K4 minus edge 2-3, pendant 4-0
    dart = {(0, 1), (0, 2), (0, 3), (1, 2), (1, 3), (0, 4)}
    if has_induced_subgraph(adj, n, dart, 5):
        return False

    # Gem: path 1-2-3-4, vertex 0 adjacent to all
    gem = {(0, 1), (0, 2), (0, 3), (0, 4), (1, 2), (2, 3), (3, 4)}
    if has_induced_subgraph(adj, n, gem, 5):
        return False

    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_three_leaf_power_enum.py <in> <exp> <out>")
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
        if not is_three_leaf_power(n, edges):
            print("non-three-leaf-power graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
