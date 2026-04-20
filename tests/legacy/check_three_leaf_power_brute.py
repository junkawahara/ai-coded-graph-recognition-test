#!/usr/bin/env python3
"""
3-leaf power ブルートフォースチェッカー。
3-leaf power ⟺ (bull, dart, gem)-free な弦グラフ。

比較テスト用: ./three_leaf_power と照合。
"""
import itertools
import random
import subprocess
import sys


def has_induced_subgraph(adj, n, pattern_edges, pattern_n):
    """adj は隣接集合の dict (1-indexed)。pattern_edges は 0-indexed の辺集合。"""
    for combo in itertools.permutations(range(1, n + 1), pattern_n):
        ok = True
        for a in range(pattern_n):
            for b in range(a + 1, pattern_n):
                edge_in_g = combo[b] in adj[combo[a]]
                edge_in_p = (a, b) in pattern_edges or (b, a) in pattern_edges
                if edge_in_g != edge_in_p:
                    ok = False
                    break
            if not ok:
                break
        if ok:
            return True
    return False


def is_chordal_brute(adj, n):
    """n <= 10 なので全 induced cycle を列挙して長さ4以上の弦なしサイクルがあるか判定。"""
    # PEO by greedy: simplicial vertex elimination
    remaining = set(range(1, n + 1))
    for _ in range(n):
        found = False
        for v in sorted(remaining):
            nbrs = [u for u in adj[v] if u in remaining and u != v]
            # Check if nbrs form a clique
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
    return True


def is_three_leaf_power_brute(n, edges):
    """(bull, dart, gem)-free chordal で判定"""
    adj = {v: set() for v in range(1, n + 1)}
    for u, v in edges:
        adj[u].add(v)
        adj[v].add(u)

    if not is_chordal_brute(adj, n):
        return False

    # Bull: 5 vertices, path 1-2-3-4-5 with 2-4 edge (actually: 1-2, 2-3, 3-4, 4-5, 2-3 triangle + pendant)
    # Bull graph: vertices {0,1,2,3,4}, edges: 0-1, 1-2, 2-3, 1-3, 3-4
    # (triangle 1-2-3 with pendants 0-1 and 3-4)
    bull_edges = {(0, 1), (1, 2), (2, 3), (1, 3), (3, 4)}
    if has_induced_subgraph(adj, n, bull_edges, 5):
        return False

    # Dart: K4 minus one edge, plus pendant on degree-3 vertex
    # vertices {0,1,2,3,4}, clique {0,1,2,3} minus edge 2-3, pendant 4-0
    # edges: 0-1, 0-2, 0-3, 1-2, 1-3, 0-4
    dart_edges = {(0, 1), (0, 2), (0, 3), (1, 2), (1, 3), (0, 4)}
    if has_induced_subgraph(adj, n, dart_edges, 5):
        return False

    # Gem (fan F_4): path + universal vertex
    # vertices {0,1,2,3,4}, path 1-2-3-4, vertex 0 adjacent to all
    # edges: 0-1, 0-2, 0-3, 0-4, 1-2, 2-3, 3-4
    gem_edges = {(0, 1), (0, 2), (0, 3), (0, 4), (1, 2), (2, 3), (3, 4)}
    if has_induced_subgraph(adj, n, gem_edges, 5):
        return False

    return True


def gen_random_graph(n, p):
    edges = []
    for i in range(1, n + 1):
        for j in range(i + 1, n + 1):
            if random.random() < p:
                edges.append((i, j))
    return edges


def gen_tree_leaf_power(n):
    """ランダムな木を生成し、その 3-leaf power を返す"""
    if n <= 1:
        return []
    # ランダムな木を Prüfer sequence で生成 (内部頂点を含む)
    total = n + random.randint(1, max(1, n // 2))  # 内部頂点も含む
    if total < 2:
        total = 2
    # Random tree on 'total' vertices
    tree_adj = {v: set() for v in range(1, total + 1)}
    for v in range(2, total + 1):
        u = random.randint(1, v - 1)
        tree_adj[u].add(v)
        tree_adj[v].add(u)

    # 葉を n 個選ぶ (次数1の頂点を優先)
    leaves_pool = [v for v in range(1, total + 1) if len(tree_adj[v]) == 1]
    if len(leaves_pool) < n:
        # 足りない場合はランダム追加
        rest = [v for v in range(1, total + 1) if v not in leaves_pool]
        random.shuffle(rest)
        leaves_pool += rest
    leaves = leaves_pool[:n]

    # BFS で全ペア距離を計算
    def bfs_dist(src):
        dist = [-1] * (total + 1)
        dist[src] = 0
        queue = [src]
        qi = 0
        while qi < len(queue):
            u = queue[qi]
            qi += 1
            for v in tree_adj[u]:
                if dist[v] == -1:
                    dist[v] = dist[u] + 1
                    queue.append(v)
        return dist

    edges = []
    for i in range(len(leaves)):
        dist_i = bfs_dist(leaves[i])
        for j in range(i + 1, len(leaves)):
            if dist_i[leaves[j]] <= 3:
                edges.append((i + 1, j + 1))
    return edges


def run_binary(bin_path, n, edges):
    lines = [f"{n} {len(edges)}"] + [f"{u} {v}" for u, v in edges]
    inp = "\n".join(lines) + "\n"
    p = subprocess.run([bin_path], input=inp.encode("ascii"),
                       stdout=subprocess.PIPE, timeout=10)
    out = p.stdout.decode("ascii").strip().split('\n')
    return out[0].strip() if out else ""


def main():
    if len(sys.argv) != 3:
        print("usage: check_three_leaf_power_brute.py <binary> <cases>")
        return 2
    bin_path = sys.argv[1]
    cases = int(sys.argv[2])
    random.seed(42)
    yes_count = 0
    no_count = 0
    for t in range(cases):
        if t % 3 == 0:
            n = random.randint(1, 8)
            edges = gen_tree_leaf_power(n)
        elif t % 3 == 1:
            n = random.randint(1, 8)
            p = random.random() * 0.6
            edges = gen_random_graph(n, p)
        else:
            n = random.randint(1, 8)
            p = random.random()
            edges = gen_random_graph(n, p)

        expected = is_three_leaf_power_brute(n, edges)
        got = run_binary(bin_path, n, edges)
        exp_str = "YES" if expected else "NO"

        if got != exp_str:
            print(f"MISMATCH trial {t}: n={n} m={len(edges)} expected={exp_str} got={got}")
            for u, v in edges:
                print(f"  {u} {v}")
            return 1

        if expected:
            yes_count += 1
        else:
            no_count += 1

        if (t + 1) % 1000 == 0:
            print(f"  ... {t + 1}/{cases} done (YES={yes_count}, NO={no_count})")

    print(f"OK: {cases} cases (YES={yes_count}, NO={no_count})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
