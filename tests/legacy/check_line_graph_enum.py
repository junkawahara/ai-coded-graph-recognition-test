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


def is_line_graph(n, edges):
    """Check if graph is a line graph via Krausz partition backtracking."""
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    edge_list = list(edges)
    m = len(edge_list)
    if m == 0:
        return True

    edge_idx = {}
    for i, (u, v) in enumerate(edge_list):
        edge_idx[(min(u, v), max(u, v))] = i

    edge_clique = [-1] * m
    vertex_clique_count = [0] * (n + 1)

    def solve(start):
        first = start
        while first < m and edge_clique[first] != -1:
            first += 1
        if first >= m:
            return True
        eu, ev = edge_list[first]
        if vertex_clique_count[eu] >= 2 or vertex_clique_count[ev] >= 2:
            return False
        common = [w for w in adj[eu] & adj[ev]
                  if w != eu and w != ev and vertex_clique_count[w] < 2]
        for mask in range(1 << len(common)):
            clique_verts = [eu, ev]
            for i in range(len(common)):
                if mask & (1 << i):
                    clique_verts.append(common[i])
            # Check clique
            is_clique = True
            for i in range(len(clique_verts)):
                for j in range(i + 1, len(clique_verts)):
                    if clique_verts[j] not in adj[clique_verts[i]]:
                        is_clique = False
                        break
                if not is_clique:
                    break
            if not is_clique:
                continue
            # Collect clique edges
            clique_edges = []
            ok = True
            for i in range(len(clique_verts)):
                for j in range(i + 1, len(clique_verts)):
                    a, b = min(clique_verts[i], clique_verts[j]), max(clique_verts[i], clique_verts[j])
                    eid = edge_idx.get((a, b), -1)
                    if eid == -1 or edge_clique[eid] != -1:
                        ok = False
                        break
                    clique_edges.append(eid)
                if not ok:
                    break
            if not ok:
                continue
            # Assign
            for eid in clique_edges:
                edge_clique[eid] = 1
            for v in clique_verts:
                vertex_clique_count[v] += 1
            if solve(first + 1):
                return True
            for eid in clique_edges:
                edge_clique[eid] = -1
            for v in clique_verts:
                vertex_clique_count[v] -= 1
        return False

    return solve(0)


def main():
    if len(sys.argv) != 4:
        print("usage: check_line_graph_enum.py <in> <exp> <out>")
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
        if not is_line_graph(n, edges):
            print("non-line-graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
