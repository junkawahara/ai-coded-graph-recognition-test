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


def _induced_cycles_5_6(n, adj):
    """Enumerate vertex sets of induced (chordless) 5- and 6-cycles."""
    cycles = set()

    def extend(start, path, on_path):
        v = path[-1]
        for w in adj[v]:
            if w == start and len(path) >= 5:
                cycles.add(frozenset(path))
            elif w > start and w not in on_path and len(path) < 6:
                path.append(w)
                on_path.add(w)
                extend(start, path, on_path)
                path.pop()
                on_path.discard(w)

    for s in range(1, n + 1):
        extend(s, [s], set([s]))

    induced = []
    for cyc in cycles:
        k = len(cyc)
        deg_in = {v: sum(1 for u in adj[v] if u in cyc) for v in cyc}
        if all(deg_in[v] == 2 for v in cyc):
            induced.append(cyc)
    return induced


def _nonseparating(n, adj, cyc):
    """True if G - V(cyc) is connected (or empty)."""
    rest = [v for v in range(1, n + 1) if v not in cyc]
    if not rest:
        return True
    seen = set([rest[0]])
    stack = [rest[0]]
    while stack:
        v = stack.pop()
        for u in adj[v]:
            if u not in cyc and u not in seen:
                seen.add(u)
                stack.append(u)
    return len(seen) == len(rest)


def _triconnected(n, adj):
    """3-connectivity by removing every vertex pair (small n)."""
    if n < 4:
        return False

    def connected_without(removed):
        start = next(v for v in range(1, n + 1) if v not in removed)
        seen = set([start])
        stack = [start]
        while stack:
            v = stack.pop()
            for u in adj[v]:
                if u not in removed and u not in seen:
                    seen.add(u)
                    stack.append(u)
        return len(seen) == n - len(removed)

    for a in range(1, n + 1):
        for b in range(a + 1, n + 1):
            if not connected_without({a, b}):
                return False
    return True


def compute_faces(n, edges):
    """Face sizes of the planar embedding, via peripheral cycles.

    By Tutte's theorem, in a 3-connected planar graph the face
    boundaries are exactly the peripheral (induced and non-separating)
    cycles. Conversely, for a cubic graph a collection of 5/6-cycles
    covering every edge exactly twice with n - m + F = 2 glues into a
    genus-0 embedding (each degree-3 vertex link is forced to be a
    triangle), so these conditions are exact -- no rotation-system
    search is needed. Returns the face size list, or None.
    """
    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)

    if not _triconnected(n, adj):
        return None

    faces = [c for c in _induced_cycles_5_6(n, adj) if _nonseparating(n, adj, c)]

    cover = {}
    for cyc in faces:
        for v in cyc:
            for u in adj[v]:
                if u in cyc and v < u:
                    cover[(v, u)] = cover.get((v, u), 0) + 1
    m = len(edges)
    if len(cover) != m or any(c != 2 for c in cover.values()):
        return None
    if n - m + len(faces) != 2:
        return None
    return [len(c) for c in faces]


def is_fullerene(n, edges):
    """Check if graph is a fullerene."""
    if n < 20 or n % 2 != 0 or n == 22:
        return False
    m = len(edges)
    if m != 3 * n // 2:
        return False

    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    # Check cubic
    for v in range(1, n + 1):
        if len(adj[v]) != 3:
            return False

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

    # Check planarity (edge count bound for planar: m <= 3n - 6)
    if m > 3 * n - 6:
        return False

    # Compute faces and check all are pentagons/hexagons
    faces = compute_faces(n, edges)
    if faces is None:
        return False

    for f in faces:
        if f != 5 and f != 6:
            return False

    # Verify face counts: 12 pentagons, n/2 - 10 hexagons
    pent = sum(1 for f in faces if f == 5)
    hexa = sum(1 for f in faces if f == 6)
    if pent != 12:
        return False
    if hexa != n // 2 - 10:
        return False

    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_fullerene_enum.py <in> <exp> <out>")
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
        if not is_fullerene(n, edges):
            print("non-fullerene graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
