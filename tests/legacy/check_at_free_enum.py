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
    if len(set(graphs)) != len(graphs):
        return None, "duplicate graphs detected"
    return graphs, None


def is_at_free(n, edges):
    """Check if graph is AT-free (asteroidal triple free).

    A graph is AT-free if it contains no asteroidal triple: three
    independent vertices such that between any two of them there
    exists a path that avoids the closed neighborhood of the third.
    """
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    if n < 3:
        return True

    # comp[v][u] = component label of u in G - N[v]
    comp = [[-1] * (n + 1) for _ in range(n + 1)]

    for v in range(1, n + 1):
        blocked = set([v]) | adj[v]
        label = 0
        for u in range(1, n + 1):
            if u in blocked or comp[v][u] >= 0:
                continue
            comp[v][u] = label
            q = deque([u])
            while q:
                cur = q.popleft()
                for w in adj[cur]:
                    if w in blocked or comp[v][w] >= 0:
                        continue
                    comp[v][w] = label
                    q.append(w)
            label += 1

    for a in range(1, n + 1):
        for b in range(a + 1, n + 1):
            for c in range(b + 1, n + 1):
                if (comp[c][a] >= 0 and comp[c][b] >= 0 and
                        comp[c][a] == comp[c][b] and
                        comp[b][a] >= 0 and comp[b][c] >= 0 and
                        comp[b][a] == comp[b][c] and
                        comp[a][b] >= 0 and comp[a][c] >= 0 and
                        comp[a][b] == comp[a][c]):
                    return False

    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_at_free_enum.py <in> <exp> <out>")
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
        if not is_at_free(n, edges):
            print("non-AT-free graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
