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
            return None, "line {} has negative arc count".format(line_no)
        if len(vals) != 1 + 2 * m:
            return None, "line {} does not match m u1 v1 ... format".format(line_no)

        arc_set = set()
        arcs = []
        for i in range(m):
            u = vals[1 + 2 * i]
            v = vals[2 + 2 * i]
            if not (1 <= u <= n and 1 <= v <= n):
                return None, "line {} has out-of-range endpoint".format(line_no)
            if u == v:
                return None, "line {} has self-loop".format(line_no)
            if (u, v) in arc_set:
                return None, "line {} has duplicate arc".format(line_no)
            arc_set.add((u, v))
            arcs.append((u, v))

        graphs.append(tuple(arcs))

    if len(graphs) != count:
        return None, "count mismatch: header={} body={}".format(count, len(graphs))
    if len(set(graphs)) != len(graphs):
        return None, "duplicate graphs detected"
    return graphs, None


def is_tournament(n, arcs):
    """Check if the arc set forms a tournament on {1, ..., n}."""
    arc_set = set(arcs)
    expected_arcs = n * (n - 1) // 2
    if len(arcs) != expected_arcs:
        return False
    for i in range(1, n + 1):
        for j in range(i + 1, n + 1):
            has_ij = (i, j) in arc_set
            has_ji = (j, i) in arc_set
            if has_ij == has_ji:
                return False
    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_tournament_enum.py <in> <exp> <out>")
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

    for arcs in graphs:
        if not is_tournament(n, arcs):
            print("non-tournament graph found:", arcs)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
