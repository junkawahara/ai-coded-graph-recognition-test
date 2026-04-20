#!/usr/bin/env python3
import sys
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


def is_p5_free(n, edges):
    """Check if graph is P5-free."""
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)
    # P5: induced path a-b-c-d-e
    # edges: ab, bc, cd, de
    # non-edges: ac, ad, ae, bd, be, ce
    for subset in combinations(range(1, n + 1), 5):
        for a in subset:
            rest = [x for x in subset if x != a]
            for b in rest:
                if b not in adj[a]:
                    continue
                rest2 = [x for x in rest if x != b]
                for c in rest2:
                    if c not in adj[b]:
                        continue
                    if c in adj[a]:
                        continue
                    rest3 = [x for x in rest2 if x != c]
                    for d in rest3:
                        if d not in adj[c]:
                            continue
                        if d in adj[a] or d in adj[b]:
                            continue
                        rest4 = [x for x in rest3 if x != d]
                        e = rest4[0]
                        if e not in adj[d]:
                            continue
                        if e in adj[a] or e in adj[b] or e in adj[c]:
                            continue
                        return False
    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_p5_free_enum.py <in> <exp> <out>")
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
        if not is_p5_free(n, edges):
            print("non-P5-free graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
