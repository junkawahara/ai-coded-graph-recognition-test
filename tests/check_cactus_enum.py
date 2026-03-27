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


def is_cactus(n, edges):
    """Check if graph is a cactus via biconnected component decomposition."""
    adj = [[] for _ in range(n + 1)]
    for idx, (u, v) in enumerate(edges):
        adj[u].append((v, idx))
        adj[v].append((u, idx))

    tin = [0] * (n + 1)
    low = [0] * (n + 1)
    timer = [0]
    edge_stack = []

    def check_component(stop_idx):
        comp_edges = []
        while True:
            if not edge_stack:
                return False
            eid = edge_stack.pop()
            comp_edges.append(eid)
            if eid == stop_idx:
                break
        if len(comp_edges) == 1:
            return True
        verts = set()
        for eid in comp_edges:
            u, v = edges[eid]
            verts.add(u)
            verts.add(v)
        if len(comp_edges) != len(verts):
            return False
        deg = {}
        for eid in comp_edges:
            u, v = edges[eid]
            deg[u] = deg.get(u, 0) + 1
            deg[v] = deg.get(v, 0) + 1
        return all(d == 2 for d in deg.values())

    for start in range(1, n + 1):
        if tin[start] != 0:
            continue
        stack = [(start, -1, 0)]
        timer[0] += 1
        tin[start] = low[start] = timer[0]
        while stack:
            v, parent_eid, i = stack[-1]
            if i < len(adj[v]):
                stack[-1] = (v, parent_eid, i + 1)
                to, eid = adj[v][i]
                if eid == parent_eid:
                    continue
                if tin[to] == 0:
                    edge_stack.append(eid)
                    timer[0] += 1
                    tin[to] = low[to] = timer[0]
                    stack.append((to, eid, 0))
                elif tin[to] < tin[v]:
                    edge_stack.append(eid)
                    low[v] = min(low[v], tin[to])
            else:
                stack.pop()
                if stack:
                    pv, p_parent_eid, pi = stack[-1]
                    p_eid = adj[pv][pi - 1][1]
                    low[pv] = min(low[pv], low[v])
                    if low[v] >= tin[pv]:
                        if not check_component(p_eid):
                            return False
        if edge_stack:
            return False
    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_cactus_enum.py <in> <exp> <out>")
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
        if not is_cactus(n, edges):
            print("non-cactus graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
