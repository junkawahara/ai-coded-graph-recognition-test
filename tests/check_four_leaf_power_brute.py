#!/usr/bin/env python3
"""
4-leaf power ブルートフォース比較チェッカー。
ランダムグラフと構成的 4-leaf power で C++ 認識を検証。
"""
import itertools
import random
import subprocess
import sys
from collections import defaultdict


def prufer_decode(seq, n):
    degree = [1] * n
    for x in seq:
        degree[x] += 1
    edges = []
    for x in seq:
        for leaf in range(n):
            if degree[leaf] == 1:
                break
        edges.append((leaf, x))
        degree[leaf] -= 1
        degree[x] -= 1
    remaining = [i for i in range(n) if degree[i] == 1]
    edges.append((remaining[0], remaining[1]))
    return edges


def tree_dist_and_paths(edges, k):
    adj = defaultdict(list)
    for idx, (u, v) in enumerate(edges):
        adj[u].append((v, idx))
        adj[v].append((u, idx))
    dist = [[0] * k for _ in range(k)]
    for s in range(k):
        visited = [False] * k
        visited[s] = True
        queue = [s]
        qi = 0
        while qi < len(queue):
            u = queue[qi]
            qi += 1
            for v, _ in adj[u]:
                if not visited[v]:
                    visited[v] = True
                    dist[s][v] = dist[s][u] + 1
                    queue.append(v)
    return dist


def check_subdivision(tree_edges, k, Q):
    if k <= 1:
        return True
    adj = defaultdict(list)
    for idx, (u, v) in enumerate(tree_edges):
        adj[u].append((v, idx))
        adj[v].append((u, idx))
    num_e = len(tree_edges)
    dist = [[0] * k for _ in range(k)]
    paths = [[[] for _ in range(k)] for _ in range(k)]
    for s in range(k):
        visited = [False] * k
        visited[s] = True
        parent_edge = [-1] * k
        parent_node = [-1] * k
        queue = [s]
        qi = 0
        while qi < len(queue):
            u = queue[qi]
            qi += 1
            for v, eidx in adj[u]:
                if not visited[v]:
                    visited[v] = True
                    dist[s][v] = dist[s][u] + 1
                    parent_edge[v] = eidx
                    parent_node[v] = u
                    queue.append(v)
        for t in range(k):
            if t == s:
                continue
            path = []
            cur = t
            while cur != s:
                path.append(parent_edge[cur])
                cur = parent_node[cur]
            paths[s][t] = path

    for i in range(k):
        for j in range(i + 1, k):
            if Q[i][j] and dist[i][j] > 2:
                return False
    upper = [2] * num_e
    for i in range(k):
        for j in range(i + 1, k):
            if not Q[i][j]:
                continue
            path = paths[i][j]
            slack = 2 - dist[i][j]
            if slack == 0:
                for e in path:
                    upper[e] = 0
            else:
                for e in path:
                    upper[e] = min(upper[e], slack)
    for i in range(k):
        for j in range(i + 1, k):
            if Q[i][j]:
                continue
            path = paths[i][j]
            needed = 3 - dist[i][j]
            if needed <= 0:
                continue
            if sum(upper[e] for e in path) < needed:
                return False
    return True


def is_four_leaf_power_brute(n, edges_1indexed):
    """Python ブルートフォース 4-leaf power 判定"""
    if n <= 1:
        return True

    adj = [set() for _ in range(n)]
    for u, v in edges_1indexed:
        adj[u - 1].add(v - 1)
        adj[v - 1].add(u - 1)

    # Chordal check
    remaining = set(range(n))
    for _ in range(n):
        found = False
        for v in sorted(remaining):
            nbrs = [u for u in adj[v] if u in remaining and u != v]
            is_clique = True
            for i in range(len(nbrs)):
                for j in range(i + 1, len(nbrs)):
                    if nbrs[j] not in adj[nbrs[i]]:
                        is_clique = False
                        break
                if not is_clique:
                    break
            if is_clique:
                remaining.remove(v)
                found = True
                break
        if not found:
            return False

    # Critical cliques
    closed_nbr = {}
    for v in range(n):
        closed_nbr[v] = tuple(sorted(adj[v] | {v}))
    groups = defaultdict(list)
    for v in range(n):
        groups[closed_nbr[v]].append(v)
    ccs = list(groups.values())
    k = len(ccs)

    cc_id = [0] * n
    for i, cc in enumerate(ccs):
        for v in cc:
            cc_id[v] = i

    Q = [[False] * k for _ in range(k)]
    for i in range(k):
        Q[i][i] = True
    for i in range(k):
        rep = ccs[i][0]
        for u in adj[rep]:
            j = cc_id[u]
            if j != i:
                Q[i][j] = True
                Q[j][i] = True

    for i in range(k):
        rep = ccs[i][0]
        my_size = len(ccs[i])
        external = len(adj[rep]) - (my_size - 1)
        expected = sum(len(ccs[j]) for j in range(k) if j != i and Q[i][j])
        if external != expected:
            return False

    if k == 1:
        return True
    if k == 2:
        return check_subdivision([(0, 1)], 2, Q)
    for seq in itertools.product(range(k), repeat=k - 2):
        te = prufer_decode(list(seq), k)
        if check_subdivision(te, k, Q):
            return True
    return False


def generate_random_4lp(n):
    """ランダムな Steiner 木から 4-leaf power を構成"""
    if n <= 1:
        return []
    s = random.randint(1, max(1, n - 1))
    total = n + s
    if total < 2:
        return []

    # ランダム Prufer 列で木を生成
    if total == 2:
        tree_edges = [(0, 1)]
    else:
        seq = [random.randint(0, total - 1) for _ in range(total - 2)]
        tree_edges = prufer_decode(seq, total)

    # 各頂点を Steiner ノードに割り当て
    assignment = [random.randint(n, n + s - 1) for _ in range(n)]

    # 距離計算
    dist = tree_dist_and_paths(tree_edges, total)

    edges = []
    for i in range(n):
        for j in range(i + 1, n):
            a, b = assignment[i], assignment[j]
            if a == b:
                d = 2
            else:
                d = dist[a][b] + 2
            if d <= 4:
                edges.append((i + 1, j + 1))
    return edges


def run_binary(binary, n, edges):
    """C++ バイナリを実行して結果を取得"""
    m = len(edges)
    inp = f"{n} {m}\n"
    for u, v in edges:
        inp += f"{u} {v}\n"
    proc = subprocess.run([binary], input=inp, capture_output=True, text=True, timeout=30)
    return proc.stdout.strip()


def main():
    binary = "./four_leaf_power"
    num_tests = 500
    max_n = 8

    if len(sys.argv) > 1:
        binary = sys.argv[1]
    if len(sys.argv) > 2:
        num_tests = int(sys.argv[2])

    passed = 0
    failed = 0

    for t in range(num_tests):
        n = random.randint(1, max_n)

        if random.random() < 0.5:
            # ランダム 4-leaf power
            edges = generate_random_4lp(n)
            expected = True
        else:
            # ランダムグラフ
            all_possible = [(i, j) for i in range(1, n + 1) for j in range(i + 1, n + 1)]
            m = random.randint(0, len(all_possible))
            edges = random.sample(all_possible, m)
            expected = is_four_leaf_power_brute(n, edges)

        try:
            result = run_binary(binary, n, edges)
        except Exception as e:
            print(f"Test {t + 1}: ERROR running binary: {e}")
            failed += 1
            continue

        cpp_answer = (result == "YES")
        if cpp_answer != expected:
            print(f"Test {t + 1}: MISMATCH n={n} edges={edges}")
            print(f"  Expected: {'YES' if expected else 'NO'}, Got: {result}")
            failed += 1
        else:
            passed += 1

    print(f"\n{passed}/{passed + failed} tests passed")
    if failed > 0:
        print(f"{failed} tests FAILED")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
