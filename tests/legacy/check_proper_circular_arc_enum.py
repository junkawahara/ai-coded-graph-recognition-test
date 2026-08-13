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


def _components(n, adj):
    seen = [False] * (n + 1)
    comps = []
    for s in range(1, n + 1):
        if seen[s]:
            continue
        seen[s] = True
        comp = [s]
        stack = [s]
        while stack:
            v = stack.pop()
            for u in adj[v]:
                if not seen[u]:
                    seen[u] = True
                    comp.append(u)
                    stack.append(u)
        comps.append(comp)
    return comps


def _local_tournament_orientable(comp, adj):
    """Backtracking search for a local tournament orientation.

    Deng, Hell, Huang (1996): a connected graph is a proper circular-arc
    graph iff it admits an orientation in which every out-neighborhood
    and every in-neighborhood induces a tournament (pairwise adjacent).
    """
    edges = []
    comp_set = set(comp)
    for v in comp:
        for u in adj[v]:
            if u in comp_set and v < u:
                edges.append((v, u))
    outs = {v: set() for v in comp}
    ins = {v: set() for v in comp}

    def bt(i):
        if i == len(edges):
            return True
        u, v = edges[i]
        for a, b in ((u, v), (v, u)):
            # orient a -> b
            ok = all(w in adj[a] for w in ins[b]) and \
                 all(w in adj[b] for w in outs[a])
            if ok:
                outs[a].add(b)
                ins[b].add(a)
                if bt(i + 1):
                    return True
                outs[a].discard(b)
                ins[b].discard(a)
        return False

    return bt(0)


def _is_proper_interval(comp, adj):
    """Proper interval test: some linear order makes every closed
    neighborhood consecutive (brute force over orderings)."""
    from itertools import permutations as _pi
    k = len(comp)
    if k <= 2:
        return True
    for perm in _pi(comp):
        pos = {v: i for i, v in enumerate(perm)}
        good = True
        for v in comp:
            idxs = sorted([pos[v]] + [pos[u] for u in adj[v] if u in pos])
            if idxs[-1] - idxs[0] != len(idxs) - 1:
                good = False
                break
        if good:
            return True
    return False


def is_proper_circular_arc(n, edges):
    """Exact proper circular-arc test.

    "circular-arc AND claw-free" is NOT a characterization: the net and
    W5 are CA and claw-free but not PCA, while K_{2,2,2} is PCA but
    fails the Helly-only CA test. Instead use Deng-Hell-Huang: a
    connected graph is PCA iff it is local-tournament orientable. A
    disconnected graph is PCA iff every component is a proper interval
    graph (a non-interval component's arcs would cover the circle).
    """
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)
    comps = _components(n, adj)
    real = [c for c in comps if len(c) > 1]
    if len(comps) == 1:
        return _local_tournament_orientable(comps[0], adj)
    return all(_is_proper_interval(c, adj) for c in real)


def main():
    if len(sys.argv) != 4:
        print("usage: check_proper_circular_arc_enum.py <in> <exp> <out>")
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
        if not is_proper_circular_arc(n, edges):
            print("non-proper-circular-arc graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
