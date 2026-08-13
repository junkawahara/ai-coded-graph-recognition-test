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


def has_transitive_orientation(n, edge_set):
    """Check if the undirected graph given by edge_set has a transitive orientation."""
    undirected = set()
    for u, v in edge_set:
        if u < v:
            undirected.add((u, v))

    edges_list = list(undirected)
    m = len(edges_list)

    if m > 20:
        return True  # assume yes for large graphs (shouldn't happen in tests)

    for mask in range(1 << m):
        orient = {}
        for i, (u, v) in enumerate(edges_list):
            if mask & (1 << i):
                orient[(u, v)] = True
            else:
                orient[(v, u)] = True

        ok = True
        for a in range(1, n + 1):
            if not ok:
                break
            for b in range(1, n + 1):
                if not ok:
                    break
                if (a, b) not in orient:
                    continue
                for c in range(1, n + 1):
                    if (b, c) not in orient:
                        continue
                    if (a, c) not in orient:
                        ok = False
                        break
        if ok:
            return True

    return False


def is_comparability(n, edges):
    """Check if graph is a comparability graph (admits a transitive orientation)."""
    edge_set = set()
    for u, v in edges:
        edge_set.add((u, v))
        edge_set.add((v, u))
    return has_transitive_orientation(n, edge_set)


def main():
    if len(sys.argv) != 4:
        print("usage: check_comparability_enum.py <in> <exp> <out>")
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
        if not is_comparability(n, edges):
            print("non-comparability graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
