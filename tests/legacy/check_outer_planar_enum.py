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

        graphs.append(tuple(sorted(edges)))

    if len(graphs) != count:
        return None, "count mismatch: header={} body={}".format(count, len(graphs))
    if len(set(graphs)) != len(graphs):
        return None, "duplicate graphs detected"
    return graphs, None


from itertools import combinations as _minor_comb


def _minor_edges(edges):
    return frozenset((u, v) if u < v else (v, u) for u, v in edges)


def _minor_contract(edges, x, y):
    out = set()
    for a, b in edges:
        if a == y:
            a = x
        if b == y:
            b = x
        if a != b:
            out.add((a, b) if a < b else (b, a))
    return frozenset(out)


def _minor_adj(edges):
    adj = {}
    for a, b in edges:
        adj.setdefault(a, set()).add(b)
        adj.setdefault(b, set()).add(a)
    return adj


def _sub_k5(edges):
    adj = _minor_adj(edges)
    vs = sorted(v for v in adj if len(adj[v]) >= 4)
    for s in _minor_comb(vs, 5):
        if all(s[j] in adj[s[i]] for i in range(5) for j in range(i + 1, 5)):
            return True
    return False


def _sub_k33(edges):
    adj = _minor_adj(edges)
    vs = sorted(v for v in adj if len(adj[v]) >= 3)
    for s in _minor_comb(vs, 3):
        if len(adj[s[0]] & adj[s[1]] & adj[s[2]]) >= 3:
            return True
    return False


def _sub_k4(edges):
    adj = _minor_adj(edges)
    vs = sorted(v for v in adj if len(adj[v]) >= 3)
    for s in _minor_comb(vs, 4):
        if all(s[j] in adj[s[i]] for i in range(4) for j in range(i + 1, 4)):
            return True
    return False


def _sub_k23(edges):
    adj = _minor_adj(edges)
    vs = sorted(adj)
    for s in _minor_comb(vs, 2):
        if len((adj[s[0]] & adj[s[1]]) - set(s)) >= 3:
            return True
    return False


def _has_minor(edges, sub_check, min_v, min_e, memo):
    res = memo.get(edges)
    if res is not None:
        return res
    nv = len(_minor_adj(edges))
    if nv < min_v or len(edges) < min_e:
        memo[edges] = False
        return False
    if sub_check(edges):
        memo[edges] = True
        return True
    if nv > min_v:
        for a, b in sorted(edges):
            if _has_minor(_minor_contract(edges, a, b), sub_check, min_v,
                          min_e, memo):
                memo[edges] = True
                return True
    memo[edges] = False
    return False


def minor_planar(edges):
    """Exact planarity via Wagner's theorem: no K5 minor and no K3,3 minor.

    Subgraph-only tests miss subdivisions/minors (e.g. a subdivided K5),
    so the search branches over all edge contractions with memoization.
    """
    es = _minor_edges(edges)
    nv = len(_minor_adj(es))
    if nv >= 3 and len(es) > 3 * nv - 6:
        return False
    if _has_minor(es, _sub_k5, 5, 10, {}):
        return False
    if _has_minor(es, _sub_k33, 6, 9, {}):
        return False
    return True


def minor_outerplanar(edges):
    """Exact outerplanarity: no K4 minor and no K2,3 minor."""
    es = _minor_edges(edges)
    nv = len(_minor_adj(es))
    if nv >= 2 and len(es) > 2 * nv - 3:
        return False
    if _has_minor(es, _sub_k4, 4, 6, {}):
        return False
    if _has_minor(es, _sub_k23, 5, 6, {}):
        return False
    return True


def is_outer_planar(n, edges):
    """Exact outerplanarity check (K4 / K2,3 minor search)."""
    return minor_outerplanar(edges)


def main():
    if len(sys.argv) != 4:
        print("usage: check_outer_planar_enum.py <in> <exp> <out>")
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
        if not is_outer_planar(n, edges):
            print("non-outer-planar graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
