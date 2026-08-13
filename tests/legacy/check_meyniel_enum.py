#!/usr/bin/env python3
import sys


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


def is_meyniel(n, edges):
    """Check if graph is a Meyniel graph.

    A Meyniel graph is one where every odd cycle of length >= 5
    has at least 2 chords. We check by searching for a Meyniel
    obstruction: an odd cycle of length >= 5 with <= 1 chord.
    """
    adj = [set() for _ in range(n + 1)]
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    if n <= 4:
        return True

    edge_list = [(u, v) for u in range(1, n + 1)
                 for v in sorted(adj[u]) if u < v]

    for eu, ev in edge_list:
        in_path = [False] * (n + 1)
        in_path[eu] = True
        found = [False]

        def dfs(cur, depth, path, chord_count):
            if found[0]:
                return
            for w in range(1, n + 1):
                if found[0]:
                    return
                if w not in adj[cur]:
                    continue
                if w == ev:
                    if depth == 0:
                        continue
                    # cycle length = depth + 2; odd iff depth is odd; >= 5 iff depth >= 3
                    if depth >= 3 and depth % 2 == 1:
                        extra = 0
                        for a in range(1, depth):
                            if path[a] in adj[ev]:
                                extra += 1
                        if chord_count + extra <= 1:
                            found[0] = True
                            return
                    continue
                if in_path[w]:
                    continue
                new_chords = 0
                for a in range(depth):
                    if path[a] in adj[w]:
                        new_chords += 1
                if chord_count + new_chords >= 2:
                    continue
                in_path[w] = True
                path.append(w)
                dfs(w, depth + 1, path, chord_count + new_chords)
                path.pop()
                in_path[w] = False

        dfs(eu, 0, [eu], 0)
        if found[0]:
            return False

    return True


def main():
    if len(sys.argv) != 4:
        print("usage: check_meyniel_enum.py <in> <exp> <out>")
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
        if not is_meyniel(n, edges):
            print("non-Meyniel graph found:", edges)
            return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
