#!/usr/bin/env python3
"""Brute-force checker for circular-arc graph recognition.

Uses the characterization: G is circular-arc iff its clique-vertex
incidence matrix has the circular 1's property for columns (C1CP).

C1CP is tested via: complement rows with 1 in any fixed column c,
then check if the result has C1P (linear consecutive 1's property).

C1P is checked by trying all column permutations (brute-force, for small k).
"""

import sys
import random
import subprocess
from itertools import permutations


def find_maximal_cliques(n, adj):
    """Enumerate all maximal cliques (brute force via Bron-Kerbosch)."""
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
    """Check if the matrix has C1P (linear consecutive 1's for columns).
    member = list of sets of column indices.
    Tries all (k-1)! permutations (fix column 0 at position 0)."""
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
    """Check circular 1's property for columns.
    Uses complement approach: complement rows with 1 in column 0,
    then check C1P."""
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


def is_circular_arc_brute(n, edges):
    """Check if graph is circular-arc via exhaustive search.

    Tries all possible endpoint orderings on a circle with 2n positions
    and all arc orientation choices, with aggressive pruning.
    Correct for both Helly and non-Helly circular-arc graphs.
    """
    if n <= 2:
        return True

    adj_set = set()
    for u, v in edges:
        adj_set.add((min(u, v), max(u, v)))

    total = 2 * n

    # Precompute arc bitmasks: arc_cw[p][q] = mask for arc from p to q
    # (clockwise, covering p but NOT q)
    arc_cw = [[0] * total for _ in range(total)]
    for p in range(total):
        mask = 0
        for steps in range(total):
            pos = (p + steps) % total
            arc_cw[p][(p + steps + 1) % total] = mask | (1 << pos)
            mask |= (1 << pos)

    ep = [None] * (n + 1)  # ep[v] = (left_pos, right_pos)

    def solve(v, used):
        if v > n:
            # All placed — try all 2^n orientations
            for omask in range(1 << n):
                arcs = [0] * (n + 1)
                for u in range(1, n + 1):
                    p, q = ep[u]
                    arcs[u] = arc_cw[q][p] if (omask >> (u - 1)) & 1 else arc_cw[p][q]
                valid = True
                for u in range(1, n + 1):
                    if not valid:
                        break
                    for w in range(u + 1, n + 1):
                        inter = bool(arcs[u] & arcs[w])
                        if inter != ((u, w) in adj_set):
                            valid = False
                            break
                if valid:
                    return True
            return False

        avail = [p for p in range(total) if not (used >> p) & 1]
        for i in range(len(avail)):
            for j in range(i + 1, len(avail)):
                p, q = avail[i], avail[j]
                ep[v] = (p, q)

                # Pruning: for each already-placed vertex, check if ANY
                # orientation combo can match the adjacency requirement
                ok = True
                for u in range(1, v):
                    pu, qu = ep[u]
                    is_edge = (min(u, v), max(u, v)) in adj_set
                    found = False
                    for mu in range(2):
                        if found:
                            break
                        au = arc_cw[qu][pu] if mu else arc_cw[pu][qu]
                        for mv in range(2):
                            av = arc_cw[q][p] if mv else arc_cw[p][q]
                            if bool(au & av) == is_edge:
                                found = True
                                break
                    if not found:
                        ok = False
                        break
                if ok and solve(v + 1, used | (1 << p) | (1 << q)):
                    return True
        return False

    # Fix vertex 1's first endpoint at position 0 (break rotational symmetry)
    for r in range(1, total):
        ep[1] = (0, r)
        if solve(2, (1 << 0) | (1 << r)):
            return True
    return False


def run_test(binary, n, edges):
    """Run the binary on the given graph and return result."""
    m = len(edges)
    input_str = f"{n} {m}\n"
    for u, v in edges:
        input_str += f"{u} {v}\n"

    result = subprocess.run(
        [binary],
        input=input_str,
        capture_output=True,
        text=True,
        timeout=30,
    )
    return result.stdout.strip()


def main():
    binary = sys.argv[1] if len(sys.argv) > 1 else "./circular_arc"
    num_tests = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
    max_n = int(sys.argv[3]) if len(sys.argv) > 3 else 7

    passed = 0
    failed = 0
    yes_count = 0
    no_count = 0

    for t in range(num_tests):
        n = random.randint(1, max_n)
        p = random.uniform(0.1, 0.9)
        edges = []
        for u in range(1, n + 1):
            for v in range(u + 1, n + 1):
                if random.random() < p:
                    edges.append((u, v))

        expected = is_circular_arc_brute(n, edges)
        expected_str = "YES" if expected else "NO"
        if expected:
            yes_count += 1
        else:
            no_count += 1

        got = run_test(binary, n, edges)
        got_bool = got.startswith("YES")

        if got_bool == expected:
            passed += 1
        else:
            failed += 1
            m = len(edges)
            print(f"FAIL test {t}: n={n} m={m} expected={expected_str} got={got}")
            print(f"  edges: {edges}")
            if failed >= 10:
                print("Too many failures, stopping.")
                break

    print(f"\nResults: {passed} passed, {failed} failed "
          f"(YES: {yes_count}, NO: {no_count})")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
