#!/usr/bin/env python3
"""Random testing for perfect graph recognition against brute-force checker."""
import random
import subprocess
import sys
import tempfile
import os
from itertools import combinations


def has_odd_induced_cycle(n, adj):
    """Brute-force check for odd induced cycle of length >= 5."""
    for k in range(5, n + 1, 2):
        for subset in combinations(range(1, n + 1), k):
            vset = set(subset)
            # Check each vertex has exactly 2 neighbors in vset
            ok = True
            for v in subset:
                cnt = len(adj[v] & vset)
                if cnt != 2:
                    ok = False
                    break
            if not ok:
                continue
            # Check connectivity
            start = subset[0]
            cur = start
            prev = -1
            visited = set()
            for _ in range(k):
                visited.add(cur)
                nexts = [w for w in adj[cur] if w in vset and w != prev]
                if prev == -1:
                    nxt = nexts[0]
                else:
                    if len(nexts) != 1:
                        break
                    nxt = nexts[0]
                prev = cur
                cur = nxt
            else:
                if cur == start and len(visited) == k:
                    return True
    return False


def is_perfect_brute(n, edges):
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)
    if has_odd_induced_cycle(n, adj):
        return False
    # Check complement
    comp_adj = [set() for _ in range(n + 1)]
    for u in range(1, n + 1):
        for v in range(u + 1, n + 1):
            if v not in adj[u]:
                comp_adj[u].add(v)
                comp_adj[v].add(u)
    if has_odd_induced_cycle(n, comp_adj):
        return False
    return True


def run_bin(bin_path, n, edges):
    lines = [f"{n} {len(edges)}"] + [f"{u} {v}" for u, v in edges]
    inp = "\n".join(lines) + "\n"
    p = subprocess.run([bin_path], input=inp.encode(), stdout=subprocess.PIPE,
                       timeout=10)
    return p.stdout.decode().strip()


def main():
    if len(sys.argv) < 2:
        print("usage: random_perfect.py <bin> [cases] [max_n]")
        return 2
    bin_path = sys.argv[1]
    cases = int(sys.argv[2]) if len(sys.argv) > 2 else 500
    max_n = int(sys.argv[3]) if len(sys.argv) > 3 else 8

    random.seed(42)
    yes_count = 0
    no_count = 0

    for t in range(cases):
        n = random.randint(1, max_n)
        p = random.random()
        edges = []
        for i in range(1, n + 1):
            for j in range(i + 1, n + 1):
                if random.random() < p:
                    edges.append((i, j))

        expected = is_perfect_brute(n, edges)
        out = run_bin(bin_path, n, edges)

        if out not in ('YES', 'NO'):
            print(f"FAIL case {t}: bad output: {out}")
            print(f"  n={n} edges={edges}")
            return 1

        got = (out == 'YES')
        if got != expected:
            exp_str = 'YES' if expected else 'NO'
            print(f"FAIL case {t}: expected {exp_str}, got {out}")
            print(f"  n={n} edges={edges}")
            return 1

        if expected:
            yes_count += 1
        else:
            no_count += 1

    print(f"OK {cases} cases (YES={yes_count}, NO={no_count})")
    return 0


if __name__ == '__main__':
    sys.exit(main())
