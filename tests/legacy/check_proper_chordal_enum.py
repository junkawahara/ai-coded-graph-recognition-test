#!/usr/bin/env python3
"""Checker for proper chordal graph enumeration.

Verifies:
1. Output format: count on first line, then edge lists
2. All graphs are proper chordal (chordal + admits indifference tree-layout)
3. Count matches expected
4. No duplicates
"""
import sys
from itertools import permutations
from collections import deque


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


def is_chordal(n, edges):
    """MCS-based chordal check."""
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    # MCS to get PEO
    order = []
    weight = [0] * (n + 1)
    chosen = [False] * (n + 1)
    for _ in range(n):
        best = -1
        for v in range(1, n + 1):
            if not chosen[v] and (best < 0 or weight[v] > weight[best]):
                best = v
        chosen[best] = True
        order.append(best)
        for u in adj[best]:
            if not chosen[u]:
                weight[u] += 1

    # Verify PEO (reverse MCS order is the PEO; "later in PEO" = earlier in MCS)
    pos = [0] * (n + 1)
    for i, v in enumerate(order):
        pos[v] = i

    for v in order:
        # "later in PEO" = picked earlier by MCS = lower pos value
        later_nbrs = [u for u in adj[v] if pos[u] < pos[v]]
        later_nbrs.sort(key=lambda u: pos[u], reverse=True)
        if len(later_nbrs) >= 2:
            first = later_nbrs[0]
            for u in later_nbrs[1:]:
                if u not in adj[first]:
                    return False
    return True


def find_components(n, adj, excluded):
    """Find connected components of graph excluding some vertices."""
    visited = [False] * (n + 1)
    for v in excluded:
        visited[v] = True
    components = []
    for v in range(1, n + 1):
        if visited[v]:
            continue
        comp = []
        q = deque([v])
        visited[v] = True
        while q:
            u = q.popleft()
            comp.append(u)
            for w in adj[u]:
                if not visited[w]:
                    visited[w] = True
                    q.append(w)
        components.append(comp)
    return components


def compute_block_tree(n, adj, root):
    """Algorithm 1: compute block tree rooted at vertex root.
    Returns (blocks, parent) or None if failed."""
    in_S = set([root])
    blocks = [[root]]
    parent = [-1]
    depth = [0]
    block_of = {}
    block_of[root] = 0

    while len(in_S) < n:
        # connected components of G - S
        comps = find_components(n, adj, in_S)
        found = False

        for comp in comps:
            comp_set = set(comp)
            # NS_C: vertices in comp adjacent to some vertex in S
            ns_c = set()
            for v in comp:
                for u in adj[v]:
                    if u in in_S:
                        ns_c.add(v)
                        break

            # S-maximal: max N(v) ∩ S by inclusion
            max_ns = frozenset()
            for v in comp:
                nv_s = frozenset(u for u in adj[v] if u in in_S)
                if max_ns < nv_s or (not (nv_s <= max_ns) and len(nv_s) > len(max_ns)):
                    # Check if nv_s is a superset of max_ns
                    if max_ns <= nv_s:
                        max_ns = nv_s

            s_maximal = []
            for v in comp:
                nv_s = frozenset(u for u in adj[v] if u in in_S)
                if nv_s == max_ns:
                    s_maximal.append(v)

            # NS_C-universal: adjacent to all of ns_c \ {self}
            block = []
            for v in s_maximal:
                universal = True
                for u in ns_c:
                    if u != v and u not in adj[v]:
                        universal = False
                        break
                if universal:
                    block.append(v)

            if not block:
                continue

            # Found S-block
            for v in block:
                in_S.add(v)

            # N(block)
            nx = set()
            for v in block:
                nx |= adj[v]

            # Find deepest ancestor block with N(block) ∩ B ≠ ∅
            best_parent = -1
            best_depth = -1
            for bi in range(len(blocks)):
                if nx & set(blocks[bi]):
                    if depth[bi] > best_depth:
                        best_depth = depth[bi]
                        best_parent = bi

            if best_parent < 0:
                return None

            new_idx = len(blocks)
            blocks.append(block)
            parent.append(best_parent)
            depth.append(best_depth + 1)
            for v in block:
                block_of[v] = new_idx

            found = True
            break

        if not found:
            return None

    return blocks, parent, depth


def verify_block_tree(n, adj, blocks, parent_arr, depth_arr):
    """Algorithm 2: verify nested-convex condition for block tree."""
    for bi in range(len(blocks)):
        B = blocks[bi]
        B_set = set(B)

        # Ancestor vertices
        ancestors = set()
        cur = parent_arr[bi]
        while cur >= 0:
            ancestors |= set(blocks[cur])
            cur = parent_arr[cur]

        # C_B: component of G - ancestors containing B
        comps = find_components(n, adj, ancestors)
        cone = set()
        for comp in comps:
            if B[0] in comp:
                cone = set(comp)
                break

        # Components of G[cone] - B
        cone_minus_B = cone - B_set
        sub_comps = find_components(n, adj, set(range(1, n + 1)) - cone_minus_B)
        # Filter to only those in cone_minus_B
        sub_comps = [c for c in sub_comps if set(c) <= cone_minus_B]

        if not sub_comps:
            continue

        # For each component, compute N_i = {N(y) ∩ B | y ∈ C_i}
        component_sets = []
        for comp in sub_comps:
            ni_sets = set()
            for y in comp:
                ny_cap_B = frozenset(u for u in adj[y] if u in B_set)
                if ny_cap_B:
                    ni_sets.add(ny_cap_B)
            ni_list = list(ni_sets)

            # Check nested
            for a in range(len(ni_list)):
                for b in range(a + 1, len(ni_list)):
                    if not (ni_list[a] <= ni_list[b] or ni_list[b] <= ni_list[a]):
                        return False
            component_sets.append(ni_list)

        # Nested-Convex check: try all permutations of B
        all_sets = []
        set_comp_idx = []
        for ci, ni in enumerate(component_sets):
            for s in ni:
                all_sets.append(s)
                set_comp_idx.append(ci)

        found_perm = False
        for perm in permutations(B):
            pos = {v: i for i, v in enumerate(perm)}

            # Check all sets are consecutive
            ok = True
            for s in all_sets:
                if not s:
                    continue
                positions = sorted(pos[v] for v in s)
                if positions[-1] - positions[0] + 1 != len(positions):
                    ok = False
                    break
            if not ok:
                continue

            # Check C-nested condition
            c_ok = True
            for ci, ni in enumerate(component_sets):
                for a in range(len(ni)):
                    for b in range(len(ni)):
                        if a == b:
                            continue
                        # Check if ni[a] ⊂ ni[b] (strict)
                        if ni[a] < ni[b]:
                            # Z = ni[a], Y = ni[b]
                            # Y\Z must come before Z
                            diff = ni[b] - ni[a]
                            last_diff = max(pos[v] for v in diff)
                            first_z = min(pos[v] for v in ni[a])
                            if last_diff >= first_z:
                                c_ok = False
                                break
                    if not c_ok:
                        break
                if not c_ok:
                    break

            if c_ok:
                found_perm = True
                break

        if not found_perm:
            return False

    return True


def is_proper_chordal(n, edges):
    """Check if graph is proper chordal using Algorithm 1 + 2."""
    if n <= 2:
        return True

    if not is_chordal(n, edges):
        return False

    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    # Check each connected component
    visited = [False] * (n + 1)
    for start in range(1, n + 1):
        if visited[start]:
            continue
        comp = []
        q = deque([start])
        visited[start] = True
        while q:
            u = q.popleft()
            comp.append(u)
            for w in adj[u]:
                if not visited[w]:
                    visited[w] = True
                    q.append(w)

        if len(comp) <= 2:
            continue

        # Build subgraph
        remap = {}
        for i, v in enumerate(comp):
            remap[v] = i + 1
        cn = len(comp)
        sub_adj = [set() for _ in range(cn + 1)]
        for v in comp:
            for u in adj[v]:
                if u in remap:
                    sub_adj[remap[v]].add(remap[u])

        # Try each vertex as root
        found = False
        for x in range(1, cn + 1):
            result = compute_block_tree(cn, sub_adj, x)
            if result is None:
                continue
            blocks, parent_arr, depth_arr = result
            if verify_block_tree(cn, sub_adj, blocks, parent_arr, depth_arr):
                found = True
                break

        if not found:
            return False

    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_proper_chordal_enum.py <in> <exp> <out>")
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
        if not is_proper_chordal(n, edges):
            print("non-proper-chordal graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
