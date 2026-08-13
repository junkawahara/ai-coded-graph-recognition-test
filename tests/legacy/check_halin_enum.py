#!/usr/bin/env python3
import sys
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


def _adj_sets(n, edges):
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)
    return adj


def _iso_invariant(n, edges):
    """Cheap isomorphism invariant for bucketing."""
    adj = _adj_sets(n, edges)
    nbr_degs = sorted(
        (len(adj[v]), tuple(sorted(len(adj[u]) for u in adj[v])))
        for v in range(1, n + 1)
    )
    return (len(edges), tuple(nbr_degs))


def _isomorphic(n, e1, e2):
    """Backtracking graph isomorphism (exact, for small n)."""
    a1 = _adj_sets(n, e1)
    a2 = _adj_sets(n, e2)
    # DFS order so each vertex (after the first per component) has a
    # previously-mapped neighbor, keeping the candidate set small.
    order = []
    anchor = {}
    seen = [False] * (n + 1)
    for s in range(1, n + 1):
        if seen[s]:
            continue
        stack = [s]
        seen[s] = True
        while stack:
            v = stack.pop()
            order.append(v)
            for u in sorted(a1[v]):
                if not seen[u]:
                    seen[u] = True
                    anchor[u] = v
                    stack.append(u)
    mapping = [0] * (n + 1)
    used = [False] * (n + 1)

    def bt(i):
        if i == n:
            return True
        v = order[i]
        if v in anchor:
            cands = a2[mapping[anchor[v]]]
        else:
            cands = range(1, n + 1)
        for w in cands:
            if used[w] or len(a2[w]) != len(a1[v]):
                continue
            ok = True
            for u in order[:i]:
                if (u in a1[v]) != (mapping[u] in a2[w]):
                    ok = False
                    break
            if ok:
                mapping[v] = w
                used[w] = True
                if bt(i + 1):
                    return True
                mapping[v] = 0
                used[w] = False
        return False

    return bt(0)


def find_isomorphic_duplicate(n, graphs):
    """Return an error message if two output graphs are isomorphic."""
    buckets = {}
    for idx, edges in enumerate(graphs):
        buckets.setdefault(_iso_invariant(n, edges), []).append(idx)
    for idxs in buckets.values():
        for i in range(len(idxs)):
            for j in range(i + 1, len(idxs)):
                if _isomorphic(n, graphs[idxs[i]], graphs[idxs[j]]):
                    return "isomorphic duplicate: graphs {} and {}".format(
                        idxs[i] + 1, idxs[j] + 1)
    return None


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


def is_halin(n, edges):
    """Check if graph is a Halin graph (tree + leaf cycle decomposition,
    and the union must be planar)."""
    if not minor_planar(edges):
        return False
    if n < 4:
        return False
    m = len(edges)
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    # Check connectivity
    visited = [False] * (n + 1)
    queue = deque([1])
    visited[1] = True
    cnt = 1
    while queue:
        u = queue.popleft()
        for v in adj[u]:
            if not visited[v]:
                visited[v] = True
                cnt += 1
                queue.append(v)
    if cnt != n:
        return False

    # Check minimum degree >= 3
    for v in range(1, n + 1):
        if len(adj[v]) < 3:
            return False

    # Try to find a Halin decomposition:
    # A spanning tree T with no degree-2 vertices such that
    # non-tree edges form a Hamiltonian cycle on T's leaves.
    #
    # Key observation: vertices of degree > 3 must be tree-internal
    # (all their edges are tree edges). Degree-3 vertices are either
    # tree-internal (3 tree edges) or tree-leaves (1 tree + 2 cycle edges).

    edge_list = list(edges)
    edge_indices = {e: i for i, e in enumerate(edge_list)}
    num_leaves = m - (n - 1)  # cycle edges = total - tree edges

    if num_leaves < 3:
        return False

    # Vertices that MUST be tree-internal (degree > 3)
    must_internal = set()
    for v in range(1, n + 1):
        if len(adj[v]) > 3:
            must_internal.add(v)

    # Degree-3 vertices: could be tree-leaf or tree-internal
    deg3 = [v for v in range(1, n + 1) if len(adj[v]) == 3]

    # We need exactly num_leaves of the degree-3 vertices to be tree-leaves
    # Try backtracking over assignments of degree-3 vertices
    if len(deg3) < num_leaves:
        return False

    from itertools import combinations

    for leaf_combo in combinations(deg3, num_leaves):
        leaf_set = set(leaf_combo)
        internal_set = must_internal | (set(deg3) - leaf_set)

        # Each leaf has 1 tree edge and 2 cycle edges
        # Each internal vertex has all tree edges
        tree_edges_set = set()
        cycle_edges_set = set()
        valid = True

        # Internal vertices: all edges are tree edges
        for v in internal_set:
            for u in adj[v]:
                e = (min(u, v), max(u, v))
                tree_edges_set.add(e)

        # Check: no cycle edge should be forced as tree edge
        # (an edge between two internal vertices is always a tree edge)
        # An edge between a leaf and internal vertex: the leaf needs 1 tree edge
        # An edge between two leaves: could be tree or cycle

        # For each leaf, exactly 1 of its 3 edges is a tree edge
        for v in leaf_set:
            nbrs = list(adj[v])
            tree_nbrs = []
            possible_cycle = []
            for u in nbrs:
                e = (min(u, v), max(u, v))
                if u in internal_set:
                    # Edge to internal vertex: must be tree edge
                    tree_nbrs.append(e)
                elif u in leaf_set:
                    # Edge to another leaf: could be tree or cycle
                    possible_cycle.append(e)
                else:
                    tree_nbrs.append(e)

            if len(tree_nbrs) > 1:
                valid = False
                break
            if len(tree_nbrs) == 1:
                tree_edges_set.add(tree_nbrs[0])
                for e in possible_cycle:
                    cycle_edges_set.add(e)
            elif len(tree_nbrs) == 0:
                # All 3 neighbors are leaves; need exactly 1 as tree edge
                # Try each option - too complex for simple iteration
                # Skip this combo and try later
                valid = False
                break

        if not valid:
            continue

        # Check tree edges form a spanning tree
        if len(tree_edges_set) != n - 1:
            continue

        # Check tree connectivity
        tree_adj_local = [set() for _ in range(n + 1)]
        for u, v in tree_edges_set:
            tree_adj_local[u].add(v)
            tree_adj_local[v].add(u)

        visited2 = [False] * (n + 1)
        queue2 = deque([1])
        visited2[1] = True
        cnt2 = 1
        while queue2:
            u = queue2.popleft()
            for v in tree_adj_local[u]:
                if not visited2[v]:
                    visited2[v] = True
                    cnt2 += 1
                    queue2.append(v)
        if cnt2 != n:
            continue

        # Check no degree-2 in tree
        tree_ok = True
        for v in range(1, n + 1):
            d = len(tree_adj_local[v])
            if d == 2:
                tree_ok = False
                break
        if not tree_ok:
            continue

        # Check tree leaves match our leaf set
        actual_leaves = set(v for v in range(1, n + 1)
                           if len(tree_adj_local[v]) == 1)
        if actual_leaves != leaf_set:
            continue

        # Check cycle edges form a Hamiltonian cycle on leaves
        cycle_edges_actual = set()
        for e in edge_list:
            if e not in tree_edges_set:
                cycle_edges_actual.add(e)

        cycle_adj = [[] for _ in range(n + 1)]
        for u, v in cycle_edges_actual:
            cycle_adj[u].append(v)
            cycle_adj[v].append(u)

        # Each leaf should have exactly 2 cycle neighbors
        cycle_ok = True
        for v in leaf_set:
            if len(cycle_adj[v]) != 2:
                cycle_ok = False
                break
            for u in cycle_adj[v]:
                if u not in leaf_set:
                    cycle_ok = False
                    break
        if not cycle_ok:
            continue

        # Traverse the cycle
        start = next(iter(leaf_set))
        path = [start]
        prev = -1
        cur = start
        for _ in range(num_leaves - 1):
            nxt = cycle_adj[cur][0] if cycle_adj[cur][0] != prev else cycle_adj[cur][1]
            prev = cur
            cur = nxt
            path.append(cur)

        if len(set(path)) == num_leaves and cur in leaf_set:
            # Check cycle closes
            if start in cycle_adj[cur]:
                return True

    return False


def main():
    if len(sys.argv) != 4:
        print("usage: check_halin_enum.py <in> <exp> <out>")
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

    dup = find_isomorphic_duplicate(n, graphs)
    if dup is not None:
        print(dup)
        return 1

    for edges in graphs:
        if not is_halin(n, edges):
            print("non-Halin graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
