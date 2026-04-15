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

        graphs.append(tuple(edges))

    if len(graphs) != count:
        return None, "count mismatch: header={} body={}".format(count, len(graphs))
    if len(set(graphs)) != len(graphs):
        return None, "duplicate graphs detected"
    return graphs, None


def compute_faces(n, edges):
    """Compute face sizes of a cubic planar 3-connected graph.

    Returns list of face sizes, or None if face computation fails.
    Uses rotation system approach: try both orientations at vertex 1.
    """
    adj = [[] for _ in range(n + 1)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)
    for v in range(1, n + 1):
        adj[v].sort()

    E = len(edges)

    for flip in range(2):
        if len(adj[1]) != 3:
            return None
        a, b, c = adj[1]

        # next_cw[v][u] = next clockwise neighbor of v after u
        next_cw = [{} for _ in range(n + 1)]
        if flip == 0:
            next_cw[1] = {a: b, b: c, c: a}
        else:
            next_cw[1] = {a: c, c: b, b: a}

        determined = [False] * (n + 1)
        determined[1] = True

        visited_darts = set()
        faces = []
        valid = True

        for sv in range(1, n + 1):
            if not valid:
                break
            for su in adj[sv]:
                if not valid:
                    break
                if (sv, su) in visited_darts:
                    continue

                start_u, start_v = sv, su
                cu, cv = sv, su
                face_len = 0
                max_steps = 2 * E + 2

                while max_steps > 0:
                    max_steps -= 1
                    if (cu, cv) in visited_darts:
                        if cu != start_u or cv != start_v:
                            valid = False
                        break
                    visited_darts.add((cu, cv))
                    face_len += 1

                    if not determined[cv]:
                        others = [w for w in adj[cv] if w != cu]
                        if len(others) != 2:
                            valid = False
                            break
                        p, q = others
                        next_cw[cv] = {cu: p, p: q, q: cu}
                        determined[cv] = True

                    w = next_cw[cv].get(cu)
                    if w is None:
                        valid = False
                        break
                    cu, cv = cv, w

                if max_steps <= 0:
                    valid = False
                    break
                if valid and face_len > 0:
                    faces.append(face_len)

        if not valid:
            continue
        if not all(determined[v] for v in range(1, n + 1)):
            continue

        F = len(faces)
        if n - E + F == 2:
            return faces

    return None


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

    for edges in graphs:
        if not is_fullerene(n, edges):
            print("non-fullerene graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
