#!/usr/bin/env python3
import sys

def read_graph(path):
    with open(path, 'r', encoding='ascii') as f:
        data = f.read().strip().split()
    if not data:
        return 0, []
    it = iter(data)
    n = int(next(it))
    m = int(next(it))
    edges = []
    for _ in range(m):
        u = int(next(it)); v = int(next(it))
        if u != v:
            edges.append((u, v))
    return n, edges

def parse_output(path, n):
    with open(path, 'r', encoding='ascii') as f:
        lines = [ln.strip() for ln in f.readlines() if ln.strip() != '']
    if not lines:
        return None, []
    head = lines[0]
    if head not in ('YES', 'NO'):
        return None, []
    if head == 'NO':
        return 'NO', []
    intervals = []
    for ln in lines[1:]:
        parts = ln.split()
        if len(parts) != 2:
            return None, []
        l = int(parts[0]); r = int(parts[1])
        intervals.append((l, r))
    if len(intervals) != n:
        return None, []
    return 'YES', intervals

def is_interval_model(n, edges, intervals):
    for l, r in intervals:
        if l > r:
            return False
    # Check adjacency matches interval intersections.
    adj = [[False]*(n+1) for _ in range(n+1)]
    for u, v in edges:
        adj[u][v] = adj[v][u] = True
    for i in range(1, n+1):
        for j in range(i+1, n+1):
            inter = not (intervals[i-1][1] < intervals[j-1][0] or intervals[j-1][1] < intervals[i-1][0])
            if inter != adj[i][j]:
                return False
    return True

def main():
    if len(sys.argv) != 4:
        print("usage: check.py <in> <exp> <out>")
        return 2
    in_path, exp_path, out_path = sys.argv[1:4]
    n, edges = read_graph(in_path)
    with open(exp_path, 'r', encoding='ascii') as f:
        exp = f.read().strip()
    kind, intervals = parse_output(out_path, n)
    if exp not in ('YES', 'NO'):
        print("bad expected file")
        return 2
    if kind is None:
        print("bad output format")
        return 1
    if exp != kind:
        print("mismatch: expected", exp, "got", kind)
        return 1
    if kind == 'YES' and not is_interval_model(n, edges, intervals):
        print("invalid interval model")
        return 1
    return 0

if __name__ == '__main__':
    sys.exit(main())
