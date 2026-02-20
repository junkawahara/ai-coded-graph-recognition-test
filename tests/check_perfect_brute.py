#!/usr/bin/env python3
"""Brute-force perfect graph checker: enumerate all induced subgraphs
to find odd holes and odd antiholes."""
import sys
from itertools import combinations


def read_graph(path):
    with open(path) as f:
        n, m = map(int, f.readline().split())
        adj = [set() for _ in range(n + 1)]
        for _ in range(m):
            u, v = map(int, f.readline().split())
            adj[u].add(v)
            adj[v].add(u)
    return n, adj


def is_induced_cycle(vertices, adj):
    """Check if the given vertices form an induced cycle in the graph."""
    k = len(vertices)
    if k < 3:
        return False
    vset = set(vertices)
    # Check each vertex has exactly 2 neighbors in vset
    for v in vertices:
        cnt = len(adj[v] & vset)
        if cnt != 2:
            return False
    # Check connectivity: walk the cycle
    visited = set()
    start = vertices[0]
    cur = start
    prev = -1
    for _ in range(k):
        visited.add(cur)
        nexts = [w for w in adj[cur] if w in vset and w != prev]
        if len(nexts) != 1 and not (len(nexts) == 2 and prev == -1):
            return False
        if prev == -1:
            nxt = nexts[0]
        else:
            nxt = nexts[0]
        prev = cur
        cur = nxt
    return cur == start and len(visited) == k


def has_odd_hole(n, adj):
    """Check for odd induced cycle of length >= 5."""
    for k in range(5, n + 1, 2):
        for subset in combinations(range(1, n + 1), k):
            if is_induced_cycle(list(subset), adj):
                return True
    return False


def main():
    if len(sys.argv) != 4:
        print("usage: check_perfect_brute.py <in> <exp> <out>")
        return 2
    in_path, _, out_path = sys.argv[1], sys.argv[2], sys.argv[3]

    with open(out_path) as f:
        out = f.read().strip()
    if out not in ('YES', 'NO'):
        print("bad output format")
        return 1

    n, adj = read_graph(in_path)

    # Check odd hole in G
    if has_odd_hole(n, adj):
        expected = 'NO'
    else:
        # Check odd hole in complement (= odd antihole)
        comp_adj = [set() for _ in range(n + 1)]
        for u in range(1, n + 1):
            for v in range(u + 1, n + 1):
                if v not in adj[u]:
                    comp_adj[u].add(v)
                    comp_adj[v].add(u)
        if has_odd_hole(n, comp_adj):
            expected = 'NO'
        else:
            expected = 'YES'

    if out != expected:
        print(f"mismatch: brute-force says {expected}, output says {out}")
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
