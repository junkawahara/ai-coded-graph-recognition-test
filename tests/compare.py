#!/usr/bin/env python3
import random
import subprocess
import sys
from typing import List, Tuple

def run(bin_path: str, n: int, edges: List[Tuple[int,int]]) -> str:
    lines = [f"{n} {len(edges)}"] + [f"{u} {v}" for u,v in edges]
    inp = "\n".join(lines) + "\n"
    p = subprocess.run([bin_path], input=inp.encode("ascii"), stdout=subprocess.PIPE)
    return p.stdout.decode("ascii", errors="ignore")

def parse_output(out: str, n: int):
    lines = [ln.strip() for ln in out.splitlines() if ln.strip()]
    if not lines:
        return None, []
    head = lines[0]
    if head not in ("YES", "NO"):
        return None, []
    if head == "NO":
        return "NO", []
    ints = []
    for ln in lines[1:]:
        parts = ln.split()
        if len(parts) != 2:
            return None, []
        ints.append((int(parts[0]), int(parts[1])))
    if len(ints) != n:
        return None, []
    return "YES", ints

def is_interval_model(n: int, edges: List[Tuple[int,int]], intervals: List[Tuple[int,int]]) -> bool:
    for l, r in intervals:
        if l > r:
            return False
    adj = [[False]*(n+1) for _ in range(n+1)]
    for u, v in edges:
        adj[u][v] = adj[v][u] = True
    for i in range(1, n+1):
        for j in range(i+1, n+1):
            inter = not (intervals[i-1][1] < intervals[j-1][0] or intervals[j-1][1] < intervals[i-1][0])
            if inter != adj[i][j]:
                return False
    return True

def gen_random_graph(n: int, p: float):
    edges = []
    for i in range(1, n+1):
        for j in range(i+1, n+1):
            if random.random() < p:
                edges.append((i, j))
    return edges

def gen_interval_graph(n: int):
    pts = []
    for _ in range(n):
        a = random.randint(1, 50)
        b = random.randint(1, 50)
        if a > b:
            a, b = b, a
        pts.append((a, b))
    edges = []
    for i in range(n):
        for j in range(i+1, n):
            if not (pts[i][1] < pts[j][0] or pts[j][1] < pts[i][0]):
                edges.append((i+1, j+1))
    return edges

def main():
    if len(sys.argv) != 4:
        print("usage: compare.py <bin1> <bin2> <cases>")
        return 2
    bin1, bin2, cases = sys.argv[1], sys.argv[2], int(sys.argv[3])
    random.seed(0)
    yes_count = 0
    no_count = 0
    for t in range(cases):
        if t % 2 == 0:
            n = random.randint(1, 10)
            edges = gen_interval_graph(n)
        else:
            n = random.randint(1, 10)
            p = random.random() * 0.6
            edges = gen_random_graph(n, p)
        out1 = run(bin1, n, edges)
        out2 = run(bin2, n, edges)
        k1, ints1 = parse_output(out1, n)
        k2, ints2 = parse_output(out2, n)
        if k1 is None or k2 is None:
            print("bad output format")
            return 1
        if k1 != k2:
            print("mismatch YES/NO")
            return 1
        if k1 == "YES":
            yes_count += 1
            if not is_interval_model(n, edges, ints1):
                print("bin1 invalid model")
                return 1
            if not is_interval_model(n, edges, ints2):
                print("bin2 invalid model")
                return 1
        else:
            no_count += 1
    print("OK")
    print("YES", yes_count)
    print("NO", no_count)
    return 0

if __name__ == "__main__":
    sys.exit(main())
