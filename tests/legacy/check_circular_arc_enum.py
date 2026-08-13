#!/usr/bin/env python3
import sys
from itertools import permutations


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


def find_maximal_cliques(n, adj):
    """Enumerate all maximal cliques (Bron-Kerbosch)."""
    cliques = []

    def bron_kerbosch(R, P, X):
        if not P and not X:
            cliques.append(frozenset(R))
            return
        pivot = max(P | X, key=lambda u: len(P & adj[u]))
        for v in list(P - adj[pivot]):
            bron_kerbosch(R | {v}, P & adj[v], X & adj[v])
            P = P - {v}
            X = X | {v}

    bron_kerbosch(set(), set(range(1, n + 1)), set())
    return cliques


def has_c1p(member, k):
    """Check if the matrix has C1P (linear consecutive 1's for columns)."""
    if k <= 1:
        return True

    rows = [s for s in member if len(s) > 1]
    if not rows:
        return True

    for perm in permutations(range(1, k)):
        order = [0] + list(perm)
        pos = {c: i for i, c in enumerate(order)}
        valid = True
        for v_cols in rows:
            positions = sorted(pos[c] for c in v_cols)
            for i in range(1, len(positions)):
                if positions[i] - positions[i - 1] != 1:
                    valid = False
                    break
            if not valid:
                break
        if valid:
            return True
    return False


def has_c1cp(member, k):
    """Check circular 1's property for columns."""
    if k <= 2:
        return True

    all_cols = set(range(k))
    c = 0
    new_member = []
    for v_cols in member:
        if c in v_cols:
            new_member.append(all_cols - v_cols)
        else:
            new_member.append(v_cols)
    return has_c1p(new_member, k)


def is_circular_arc(n, edges):
    """Check if graph is circular-arc via brute force C1CP."""
    if n <= 2:
        return True

    adj = {v: set() for v in range(1, n + 1)}
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    cliques = find_maximal_cliques(n, adj)
    k = len(cliques)

    if k == 0:
        return True

    clique_list = list(cliques)
    member = []
    for v in range(1, n + 1):
        v_cliques = set()
        for i, c in enumerate(clique_list):
            if v in c:
                v_cliques.add(i)
        member.append(v_cliques)

    return has_c1cp(member, k)


def main():
    if len(sys.argv) != 4:
        print("usage: check_circular_arc_enum.py <in> <exp> <out>")
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
        if not is_circular_arc(n, edges):
            print("non-circular-arc graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
