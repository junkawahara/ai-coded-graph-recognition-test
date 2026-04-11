#!/usr/bin/env python3
"""Circle graph brute-force checker.

Generates random graphs and compares the C++ recognizer against
a Python DOW backtracking implementation.
"""
import itertools
import random
import subprocess
import sys


def is_circle_graph_brute(n, edges):
    """Check if graph is a circle graph by trying to construct a DOW."""
    if n <= 1:
        return True

    adj = [[False] * (n + 1) for _ in range(n + 1)]
    for u, v in edges:
        adj[u][v] = adj[v][u] = True

    first_pos = [-1] * (n + 1)
    second_pos = [-1] * (n + 1)
    placement = [0] * (n + 1)  # 0: none, 1: first placed, 2: both placed

    def check_second(v, pos):
        f = first_pos[v]
        lo, hi = min(f, pos), max(f, pos)
        for u in range(1, n + 1):
            if u == v or placement[u] != 2:
                continue
            a, b = first_pos[u], second_pos[u]
            if a > b:
                a, b = b, a
            a_in = lo < a < hi
            b_in = lo < b < hi
            interleave = a_in != b_in
            if interleave != adj[u][v]:
                return False
        return True

    def dfs(pos):
        if pos == 2 * n:
            return True
        # Try placing second occurrence first (more constrained)
        for v in range(1, n + 1):
            if placement[v] != 1:
                continue
            if not check_second(v, pos):
                continue
            second_pos[v] = pos
            placement[v] = 2
            if dfs(pos + 1):
                return True
            placement[v] = 1
            second_pos[v] = -1
        # Try placing first occurrence
        for v in range(1, n + 1):
            if placement[v] != 0:
                continue
            first_pos[v] = pos
            placement[v] = 1
            if dfs(pos + 1):
                return True
            placement[v] = 0
            first_pos[v] = -1
        return False

    # Fix vertex 1 at position 0
    first_pos[1] = 0
    placement[1] = 1
    return dfs(1)


def main():
    binary = sys.argv[1] if len(sys.argv) > 1 else "./circle"
    num_tests = int(sys.argv[2]) if len(sys.argv) > 2 else 500
    max_n = int(sys.argv[3]) if len(sys.argv) > 3 else 7

    passed = 0
    failed = 0
    for t in range(num_tests):
        n = random.randint(1, max_n)
        edges = []
        for u in range(1, n + 1):
            for v in range(u + 1, n + 1):
                if random.random() < 0.5:
                    edges.append((u, v))
        m = len(edges)

        inp = "{} {}\n".format(n, m)
        for u, v in edges:
            inp += "{} {}\n".format(u, v)

        proc = subprocess.run(
            [binary],
            input=inp,
            capture_output=True,
            text=True,
            timeout=30,
        )
        cpp_ans = proc.stdout.strip()

        py_ans = is_circle_graph_brute(n, edges)
        py_str = "YES" if py_ans else "NO"

        if cpp_ans != py_str:
            print("MISMATCH test {}: n={} m={} edges={}".format(t, n, m, edges))
            print("  C++: {}  Python: {}".format(cpp_ans, py_str))
            failed += 1
        else:
            passed += 1

    print("Passed: {} / {}".format(passed, passed + failed))
    if failed > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
