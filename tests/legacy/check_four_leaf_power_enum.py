#!/usr/bin/env python3
"""
4-leaf power 列挙チェッカー。
各グラフが 4-leaf power であることを critical clique + tree subdivision で検証。
"""
import itertools
import sys
from collections import defaultdict


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


def _q_maximal_cliques(k, Q):
    """Maximal cliques of the quotient (Bron-Kerbosch)."""
    adjq = {i: set(j for j in range(k) if j != i and Q[i][j]) for i in range(k)}
    cliques = []

    def bk(R, P, X):
        if not P and not X:
            cliques.append(sorted(R))
            return
        pivot = max(P | X, key=lambda u: len(P & adjq[u]))
        for v in sorted(P - adjq[pivot]):
            bk(R | {v}, P & adjq[v], X & adjq[v])
            P = P - {v}
            X = X | {v}

    bk(set(), set(range(k)), set())
    return cliques


def _has_steiner_two_root(k, Q):
    """Does the quotient admit a tree T' containing its k vertices with
    Q-adjacency == (distance in T' <= 2)?

    Every maximal clique of Q gets a center: one of its members or a
    fresh Steiner node (node k+ci). Each clique member is joined to the
    center. The star union must be acyclic and every non-adjacent
    quotient pair must be at forest distance >= 3; forest components
    can always be joined afterwards by length-3 connector paths, and
    Steiner LEAF nodes are never needed, so this search is exact.
    (The old edge-subdivision model missed Steiner branch nodes.)
    """
    cliques = _q_maximal_cliques(k, Q)
    nc = len(cliques)
    total = k + nc
    adjT = [set() for _ in range(total)]

    def acyclic_and_distances_ok():
        seen = [False] * total
        for s in range(total):
            if seen[s]:
                continue
            seen[s] = True
            stack = [(s, -1)]
            while stack:
                v, p = stack.pop()
                for u in adjT[v]:
                    if not seen[u]:
                        seen[u] = True
                        stack.append((u, v))
                    elif u != p:
                        return False
        for i in range(k):
            near = set()
            for u in adjT[i]:
                near.add(u)
                near.update(adjT[u])
            near.discard(i)
            reached = set(x for x in near if x < k)
            expect = set(j for j in range(k) if j != i and Q[i][j])
            if reached != expect:
                return False
        return True

    def assign(ci):
        if ci == nc:
            return acyclic_and_distances_ok()
        for center in cliques[ci] + [k + ci]:
            added = []
            for v in cliques[ci]:
                if v != center and center not in adjT[v]:
                    adjT[v].add(center)
                    adjT[center].add(v)
                    added.append(v)
            if assign(ci + 1):
                return True
            for v in added:
                adjT[v].discard(center)
                adjT[center].discard(v)
        return False

    return assign(0)


def is_four_leaf_power(n, edges):
    """4-leaf power 判定 (critical clique + tree subdivision)"""
    if n <= 1:
        return True

    # 1-indexed → 0-indexed
    adj = [set() for _ in range(n)]
    for u, v in edges:
        adj[u - 1].add(v - 1)
        adj[v - 1].add(u - 1)

    # Strongly chordal check (simplicial elimination for chordal first)
    remaining = set(range(n))
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

    # Critical cliques
    closed_nbr = {}
    for v in range(n):
        closed_nbr[v] = tuple(sorted(adj[v] | {v}))
    groups = defaultdict(list)
    for v in range(n):
        groups[closed_nbr[v]].append(v)
    ccs = list(groups.values())
    k = len(ccs)

    cc_id = [0] * n
    for i, cc in enumerate(ccs):
        for v in cc:
            cc_id[v] = i

    Q = [[False] * k for _ in range(k)]
    for i in range(k):
        Q[i][i] = True
    for i in range(k):
        rep = ccs[i][0]
        for u in adj[rep]:
            j = cc_id[u]
            if j != i:
                Q[i][j] = True
                Q[j][i] = True

    # Consistency check
    for i in range(k):
        rep = ccs[i][0]
        my_size = len(ccs[i])
        external = len(adj[rep]) - (my_size - 1)
        expected = sum(len(ccs[j]) for j in range(k) if j != i and Q[i][j])
        if external != expected:
            return False

    if k == 1:
        return True

    return _has_steiner_two_root(k, Q)


def main():
    if len(sys.argv) != 4:
        print("usage: check_four_leaf_power_enum.py <in> <exp> <out>")
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
        if not is_four_leaf_power(n, edges):
            print("non-four-leaf-power graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
