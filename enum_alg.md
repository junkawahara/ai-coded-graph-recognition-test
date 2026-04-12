# グラフクラス列挙アルゴリズム一覧

各グラフクラスについて、非同型グラフを列挙するアルゴリズム・数え上げ公式・OEIS数列をまとめる。
`[ ]` は未実装、`[x]` は実装完了を示す。

---

## 汎用フレームワーク

### Canonical Augmentation (McKay, 1998)
- 小さいグラフに頂点/辺を追加して段階的に構築。canonical deletion の逆操作のみ受理し、同型クラスごとに唯一の構築パスを保証
- nauty/geng がこの手法を実装。`geng n` で n 頂点の全非同型グラフを生成
- **参考文献**: McKay, "Isomorph-Free Exhaustive Generation," J. Algorithms 26(2), 1998
- **PDF**: `references/mckay1998_isomorph_free_generation.pdf`

### Reverse Search (Avis & Fukuda, 1996)
- 解空間上に木構造を動的に定義し、DFS で全解を列挙。多項式空間のみ必要
- **参考文献**: Avis, Fukuda, "Reverse Search for Enumeration," Discrete Appl. Math. 65(1-3), 1996

### Orderly Generation (Colbourn & Read, 1979)
- 部分解の正規形だけを拡張し、同型な中間状態を早期に排除する古典的枠組み
- 制限付きグラフクラス向けの変種もあり、triangle-free など局所禁止条件のあるクラス生成に適用可能
- **参考文献**: Colbourn, Read, "Orderly Algorithms for Graph Generation," Int. J. Comput. Math. 7, 1979; Colbourn, Read, "Orderly algorithms for generating restricted classes of graphs," J. Graph Theory 3(2), 1979

### Proximity Search (Conte & Uno, 2022)
- 極大部分グラフの列挙に特化。解グラフの出次数を削減しつつ強連結性を維持し、多項式遅延を実現
- **参考文献**: Conte, Grossi, Marino, Uno, Versari, SIAM J. Computing, 2022 (STOC 2019)
- **PDF**: `references/conte2022_proximity_search.pdf`

### Recursive Graph Transformations (Mestre, 2009)
- 基本変形を再帰的に適用して connected / 2-edge-connected グラフを重複なく生成する枠組み
- automorphism 群の重み付けを使って生成木を管理し、連結性付き生成の別系統の手法を与える
- **参考文献**: Mestre, "Generating connected and 2-edge connected graphs," JGAA 13(2), 2009

### BDD ベース列挙 (Kawahara et al., 2024)
- O(n)-bit 文字列表現を持つ交差グラフクラスに対し、BDD 上で n に関する多項式時間で列挙
- 対象: proper interval, bipartite permutation, chain, cochain, threshold
- 最大 (bi)clique サイズや辺数に制約を付けた列挙にも拡張可能
- **参考文献**: Kawahara et al., Theoretical Computer Science 1003, 2024
- **PDF**: `references/kawahara2024_bdd_intersection_graphs.pdf`

### フィルタ方式
- nauty/geng で全非同型グラフを生成 → 認識アルゴリズムでフィルタ (`geng n | ./recognizer`)
- n ≤ 11-13 程度まで実用的。geng の組み込みフィルタ (`-C` 連結, `-T` triangle-free 等) で探索空間削減可能

### 主要ソフトウェア
| ツール | 用途 | URL |
|--------|------|-----|
| **nauty/geng** | 汎用非同型グラフ生成 | https://users.cecs.anu.edu.au/~bdm/nauty/ |
| **plantri** | 平面グラフ・三角形分割生成 | https://users.cecs.anu.edu.au/~bdm/plantri/ |
| **GENREG** | 正則グラフ生成 | https://www.mathe2.uni-bayreuth.de/markus/reggraphs.html |
| **snarkhunter** | 三次グラフ・スナーク生成 | (Brinkmann, Goedgebeur, McKay) |
| **SageMath** | nauty ラッパー + グラフ理論全般 | https://www.sagemath.org/ |

---

## 各グラフクラスの列挙

### [x] Chordal (弦グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | A058862: 1, 2, 8, 61, 822, 18154, ... |
| OEIS (unlabeled) | A048193: 1, 2, 4, 10, 27, 94, 393, ... |
| 数え上げ | O(n^7) 算術演算で labeled 数を計算 |
| 列挙 | reverse search で弦部分グラフ列挙、O(n^3)/グラフ |
| 実装 | `include/chordal_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Hebert-Johnson, Lokshtanov, Vigoda, ESA 2023 (数え上げ); Kiyomi, Kijima, Uno, WG 2006 (列挙) |
| PDF | `references/hebert-johnson2023_counting_chordal.pdf` |
| 備考 | 非ラベル付き一様ランダム生成が期待多項式時間で可能 (Hien, Patel, Sah, Sawhney, STACS 2025) |

### [x] Interval (インターバルグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A005975: 1, 2, 4, 10, 27, 92, 369, 1807, ... |
| OEIS (connected unlabeled) | A005976: 1, 1, 2, 5, 15, 56, 250, 1328, ... |
| 数え上げ | 母関数による暗黙的列挙 (Hanlon, 1982) |
| 列挙 | 多項式遅延 (Yamazaki, Saitoh, Kiyomi, Uehara, WALCOM 2018) |
| 実装 | `include/interval_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Hanlon, Trans. AMS 272, 1982; Yang, Pippenger, Proc. AMS Ser. B 4, 2017 |

### [x] Proper Interval / Unit Interval (固有インターバルグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | A005217 系列 (Hanlon の列挙) |
| 列挙 | reverse search, **O(1) amortized**/グラフ; BDD ベースで非同型 proper interval graph を n の多項式時間で列挙 |
| 実装 | `include/proper_interval_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Saitoh, Yamanaka, Kiyomi, Uehara, WALCOM 2009 / IEICE Trans. E93-D(7), 2010; Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |

### [x] Split (分割グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | A179534 |
| OEIS (unlabeled) | A048194: 1, 2, 4, 9, 21, 56, 164, 557, ... |
| 数え上げ | 閉じた公式あり (クリークサイズ k に関する和) |
| 列挙 | 逆探索 (chordal の部分木として枝刈り列挙) |
| 実装 | `include/split_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Bina, Pribil, Comment. Math. Univ. Carolin. 56(2), 2015; Collins, Trenk et al., EJC 26(2), 2019 |

### [x] Threshold (閾値グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | A005840: 1, 1, 4, 23, 166, 1437, ... |
| OEIS (unlabeled) | 2^(n-1) 個 (バイナリ文字列特性化から直接) |
| 数え上げ | 閉じた公式 (Eulerian 数を用いた表現) |
| 列挙 | バイナリ文字列 (各ステップで孤立点 or 支配点を追加) の全列挙; BDD ベースで非同型 threshold graph を n の多項式時間で列挙 |
| 参考文献 | Beissinger, Peled, Graphs and Combinatorics 3, 1987; Galvin, Wesley, Zacovic, JIS 25, 2022; Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |
| PDF | `references/galvin2022_threshold_enum.pdf` |

### [x] Trivially Perfect / Quasi-Threshold (自明完全グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | 1, 2, 8, 49, 402, ... (n=1,...,5) |
| OEIS (unlabeled) | ≈ A000081 (根付き木の数、根付き森と全単射) |
| 数え上げ | UVD 木の再帰的構築による構成的列挙 |
| 実装 | `include/trivially_perfect_enum.h` (UVD_TREE) |
| 参考文献 | Galvin, Wesley, Zacovic, JIS 25, 2022 |

### [x] Strongly Chordal (強弦グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| OEIS (labeled) | 1, 2, 8, 61, 822, 18034, ... (n=1,...,6) |
| 列挙 | 逆探索 (chordal の部分木として枝刈り列挙)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/strongly_chordal_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | n≤5 では全 chordal = strongly chordal (3-sun は 6 頂点)。n=6 で chordal (18154) との差が出現 |

### [x] Ptolemaic (プトレマイオスグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected labeled) | A287886: 1, 1, 4, 35, 481, 9042, ... |
| OEIS (all labeled) | 1, 2, 8, 61, 762, 13534, ... (n=1,...,6) |
| OEIS (connected unlabeled) | A287888: 1, 1, 2, 5, 14, 47, 170, 676, ... |
| 列挙 | 逆探索 (chordal の部分木として枝刈り列挙)。多項式遅延 (Nakano, Uno); split-decomposition grammar による full enumeration / random generation; vertex-incremental characterization に基づく **O(n^3)** delay 列挙 |
| 実装 | `include/ptolemaic_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Nakano, Uno, WALCOM 2020; ISAAC 2020 / Discrete Appl. Math. 2023; Bahrani, Lumbroso, Electron. J. Combin. 25(4), 2018; Yamazaki, Qian, Uehara, Discrete Appl. Math. 342, 2024 |

### [x] Block (ブロックグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | A035053: 1, 1, 2, 4, 9, 22, 59, 165, 496, ... |
| OEIS (labeled) | 1, 2, 8, 55, 562, 7739, 134808, ... |
| 列挙 | Nakano-Uno フレームワーク: chordal 逆探索 + block 性枝刈り; split-decomposition grammar による full enumeration |
| 実装 | `include/block_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Bahrani, Lumbroso, Electron. J. Combin. 25(4), 2018 |

### [x] Weakly Chordal (弱弦グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | A079457 |
| 列挙 | 逆探索 (頂点追加 + weakly chordal 判定)。遺伝的性質のため枝刈り可能 |
| 実装 | `include/weakly_chordal_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Hayward, J. Combin. Theory Ser. B 39, 1985 |

### [x] AT-Free (小惑星三つ組なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| OEIS (labeled) | 1, 2, 8, 64, 1024, 31748, ... (n=1,...,6) |
| 列挙 | 逆探索 (頂点追加 + AT-free 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/at_free_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | AT-free は遺伝的クラス (誘導部分グラフに対して閉)。n ≤ 5 では全グラフが AT-free |
| 参考文献 | Corneil, Olariu, Stewart, SIAM J. Discrete Math. 10(3), 1997 |

---

### [x] Permutation (順列グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A123448: 1, 2, 4, 11, 33, 142, 776, 5699, 50723, ... |
| 列挙 | canonical deletion (Johnston, 2020); 多項式遅延 (Yamazaki et al., TCS 2019) |
| 実装 | `include/permutation_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | canonical deletion で n=13 まで約 44 CPU 時間 |
| 参考文献 | Yamazaki, Saitoh, Kiyomi, Uehara, TCS 2019 |

### [x] Comparability (比較可能性グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 漸近的に poset 数 (A000112) の約半分 (Möhring の定理: ほぼすべての comparability graph は UPO) |
| 列挙 | reverse search (頂点追加 + 比較可能性判定)。遺伝的性質のため枝刈り可能 |
| 実装 | `include/comparability_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Möhring, "Almost all comparability graphs are UPO," Discrete Math. 1984 |

### [x] Co-Comparability
| 項目 | 内容 |
|------|------|
| 列挙 | 逆探索 (reverse search) による頂点追加 + 余比較可能性判定。補グラフが比較可能性グラフか判定して枝刈り |
| 実装 | `include/co_comparability_enum.h` — ラベル付き全列挙 (reverse search) |

### [x] Co-Chordal (余弦グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | A058862: 1, 2, 8, 61, 822, 18154, ... (弦グラフと同値: 補グラフは全単射) |
| 列挙 | 逆探索 (reverse search) による頂点追加 + 余弦グラフ判定。補グラフが弦グラフか判定して枝刈り |
| 実装 | `include/co_chordal_enum.h` — ラベル付き全列挙 (reverse search) |

### [x] Co-Interval (余インターバルグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | インターバルグラフと同値 (補グラフは全単射): 1, 2, 8, 61, 822, ... |
| 列挙 | 逆探索 (reverse search) による頂点追加 + 余インターバルグラフ判定。補グラフがインターバルグラフか判定して枝刈り |
| 実装 | `include/co_interval_enum.h` — ラベル付き全列挙 (reverse search) |

### [x] Cograph (P4-free グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000084: 1, 2, 4, 10, 24, 66, 180, 522, 1532, ... |
| OEIS (labeled) | A006351: 1, 2, 8, 52, 472, 5504, ... |
| 数え上げ | cotree (series-parallel network) と全単射。再帰公式あり |
| 列挙 | cotree の再帰的構築により直接列挙; 非同型 cotree を直接生成する **O(n)** delay アルゴリズム; proximity search で極大 cograph 部分グラフも列挙可 |
| 実装 | `include/cograph_enum.h` — ラベル付き全列挙 (cotree construction) |
| 参考文献 | Seinsche, 1974 (P4-free 特性化); Jones, Protti, Del-Vecchio, TCS 713, 2018; Conte, Kante, Kurita, Uno, Wasa, DAM 2023 (proximity search) |

### [x] Distance-Hereditary (距離遺伝的グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | A277862: 1, 1, 2, 6, 18, 73, 308, 1484, 7492, ... |
| 数え上げ | 母関数 + symbolic specification (Chauve, Fusy, Lumbroso, 2017) |
| 列挙 | 多項式遅延 (Nakano, Uno, ISAAC 2020 / DAM 2023); vertex-incremental characterization に基づく **O(n^3)** delay 非同型列挙 |
| 実装 | `include/distance_hereditary_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Yamazaki, Qian, Uehara, Discrete Appl. Math. 342, 2024 |
| PDF | `references/chauve2017_distance_hereditary_enum.pdf` |

### [x] Circular-Arc (円弧グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未確認 |
| OEIS (labeled) | 1, 2, 8, 64, 999, 27946, ... (n=1,...,6) |
| 列挙 | 逆探索 (頂点追加 + circular-arc 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/circular_arc_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | interval の超クラス。disjoint union に非閉 (非連結は全成分が interval の場合のみ circular-arc)。n ≤ 4 では全グラフが circular-arc |

### [x] Proper Circular-Arc (固有円弧グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| OEIS (labeled) | 1, 2, 8, 60, 754, ... (n=1,...,5) |
| 認識 | circular-arc ∩ claw-free (K_{1,3}-free) の交差判定。O(n+m) |
| 列挙 | 逆探索 (頂点追加 + proper circular-arc 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/proper_circular_arc_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | proper interval の超クラス、circular-arc の部分クラス。n ≤ 3 では全グラフが proper circular-arc (claw は 4 頂点必要)。n=4 で circular-arc (64) との差が出現: K_{1,3} の 4 つのラベル付き星が除外されて 60 |
| 参考文献 | Tucker (1974); Deng, Hell, Huang (1996); Lin, Soulignac, Szwarcfiter (2013) |

### [x] Trapezoid (台形グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未確認 |
| OEIS (labeled) | 1, 2, 8, 64, 1012, ... (n=1,...,5) |
| 列挙 | 逆探索 (頂点追加 + trapezoid 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/trapezoid_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | permutation の超クラス、co-comparability の部分クラス。n≤4 では全グラフが trapezoid。n=5 で permutation (1012) = trapezoid (1012) = co-comparability (1012) |

---

### [x] Bipartite (二部グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | A047864 |
| OEIS (unlabeled) | A033995: 1, 1, 2, 3, 7, 13, 35, 88, 303, ... |
| OEIS (connected unlabeled) | A005142 |
| 列挙 | nauty/**genbg** で canonical augmentation。~O(1) amortized/グラフ |
| 実装 | `include/bipartite_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | McKay, J. Algorithms 1998; Gainer-Dewar, Gessel, EJC 21(2), 2014 |

### [x] Chordal Bipartite (弦二部グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 列挙 | 逆探索 (頂点追加 + chordal bipartite 判定)。遺伝的性質のため枝刈り可能 |
| 実装 | `include/chordal_bipartite_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Kiyomi, Kanno, Otachi, Saitoh, Yamanaka, COCOON 2019 |
| PDF | `references/kiyomi2019_chordal_bipartite_enum.pdf` |

### [x] Bipartite Permutation (二部順列グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 (小さい値は計算済み) |
| 列挙 | reverse search, **O(1) amortized**/グラフ。一様ランダム生成も O(n); BDD ベースで非同型 graph を n の多項式時間で列挙 |
| 実装 | `include/bipartite_permutation_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Saitoh, Otachi, Yamanaka, Uehara, J. Discrete Algorithms 10, 2012 (ISAAC 2009); Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |

### [x] Convex Bipartite (凸二部グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 列挙 | reverse search (頂点追加 + C1P 判定) |
| 実装 | `include/convex_bipartite_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 一方の部集合に consecutive-ones 性質。遺伝的クラスのため逆探索で枝刈り可能 |

### [x] Biconvex Bipartite (双凸二部グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 列挙 | reverse search (頂点追加 + 双凸判定) |
| 実装 | `include/biconvex_bipartite_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 両方の部集合に consecutive-ones 性質。遺伝的クラスのため逆探索で枝刈り可能 |

### [x] Chain (連鎖グラフ / 差分グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A005418: 1, 2, 3, 6, 10, 20, 36, 72, 136, ... |
| 数え上げ | **閉じた公式**: a(n) = 2^(n-2) + 2^(floor(n/2)-1) (n ≥ 2) |
| 列挙 | 各部集合の近傍が包含関係で線形順序 → 直接構築可能; BDD ベースで非同型 chain graph を n の多項式時間で列挙 |
| 参考文献 | Peled, Sun, Discrete Appl. Math. 60(1-3), 1995; Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |

### [x] Cochain
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A005418 (chain グラフと同数) |
| 列挙 | chain グラフ列挙 + 補グラフ変換。補グラフ操作が同型類上の全単射であることを利用; BDD ベースで非同型 cochain graph を n の多項式時間で列挙 |
| 実装 | `include/cochain_enum.h` — chain 列挙から補グラフ構築 |
| 備考 | chain グラフの補グラフ。非同型数は chain と一致 |
| 参考文献 | Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 |

---

### [x] Planar (平面グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A005470: 1, 2, 4, 11, 33, 142, 822, 6966, ... |
| OEIS (connected) | A003094 |
| 列挙 | **plantri** (Brinkmann, McKay)。毎秒 200 万グラフ以上 |
| 実装 | `include/planar_enum.h` — 逆探索 (頂点追加 + 平面性判定) |
| 参考文献 | Brinkmann, McKay, MATCH 58, 2007; Gimenez, Noy, JAMS 2009 (漸近公式) |
| PDF | `references/brinkmann2007_plantri.pdf` |

### [x] Outerplanar (外平面グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A111564 |
| OEIS (connected) | A111563 |
| OEIS (2-connected) | A001004 |
| 数え上げ | 漸近公式: g_n ~ 0.00910 * n^(-5/2) * 7.504^n |
| 列挙 | plantri (2-connected); 根付き版は O(1)/グラフ (Wang, Nagamochi, 2010) |
| 実装 | `include/outer_planar_enum.h` — 逆探索 (頂点追加 + outerplanar 判定) |
| 参考文献 | Bodirsky, Fusy, Kang, Vigerske, EJC 14, 2007 |
| PDF | `references/bodirsky2007_outerplanar_enum.pdf` |

### [x] Series-Parallel (直並列グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled, by edges) | A000084 |
| OEIS (labeled) | A006351 |
| 列挙 | **O(1) amortized**/グラフ (Kawano, Nakano, IEICE 2005) |
| 実装 | `include/series_parallel_enum.h` — 逆探索 (頂点追加 + SP 判定) |
| 参考文献 | Kawano, Nakano, IEICE Trans. E88-A(5), 2005; Bodirsky, Gimenez, Kang, Noy, EuroComb 2005 |

### [x] Cactus (サボテングラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000083: 1, 1, 2, 4, 9, 23, 63, 188, ... |
| OEIS (labeled) | A000314 |
| 数え上げ | 母関数 + Lagrange inversion |
| 列挙 | 根付き版で O(1)/グラフ。split-decomposition tree による列挙 + ランダム生成; split-decomposition grammar による full enumeration |
| 実装 | `include/cactus_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Bahrani, Lumbroso, arXiv:1711.10647, 2017; Bahrani, Lumbroso, Electron. J. Combin. 25(4), 2018 |
| PDF | `references/bahrani2017_cactus_enum.pdf` |

### [x] Line Graph (線グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A132220: 1, 2, 4, 10, 24, 63, 166, 471, ... |
| OEIS (connected) | A003089 |
| 列挙 | reverse search (頂点追加 + Krausz 分割判定)。遺伝的性質のため枝刈り可能 |
| 実装 | `include/line_graph_enum.h` — ラベル付き全列挙 (reverse search) |

### [x] Claw-Free (爪なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A086991: 1, 2, 4, 10, 26, 85, 302, 1285, ... |
| OEIS (connected) | A022562 |
| 列挙 | reverse search (頂点追加 + claw-free 判定)。遺伝的性質のため枝刈り可能 |
| 実装 | `include/claw_free_enum.h` — ラベル付き全列挙 (reverse search) |

### [x] Diamond-Free (ダイヤモンドなしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未確認 |
| 列挙 | reverse search (頂点追加 + diamond-free 判定)。遺伝的性質のため枝刈り可能 |
| 実装 | `include/diamond_free_enum.h` — ラベル付き全列挙 (reverse search) |

### [x] Perfect (完全グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A052431: 1, 2, 4, 11, 33, 148, 906, 8887, ... |
| OEIS (connected) | A052433 |
| 列挙 | 逆探索 (頂点追加 + SPGT 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/perfect_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Kawahara et al., WALCOM 2023 / TCS 2024 (部分クラス); Chudnovsky et al., Ann. Math. 164, 2006 (SPGT) |

### [x] Three-Leaf Power (3-leaf power)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | 1, 2, 8, 61, 642, 8254, ... (n=1,...,6) |
| 列挙 | chordal 逆探索 + 3-leaf power フィルタ (critical clique graph が森か判定); vertex-incremental characterization に基づく **O(n^3)** delay 非同型列挙 |
| 実装 | `include/three_leaf_power_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Brandstädt & Le, IPL 98, 2006 (特性化: (bull, dart, gem)-free chordal); Chauve, Fusy, Lumbroso, ANALCO 2017 (解析的数え上げ); Yamazaki, Qian, Uehara, Discrete Appl. Math. 342, 2024 |
| 備考 | split-decomposition + 母関数による正確な数え上げも可能 (distance-hereditary と同フレームワーク) |

---

## 木・森系グラフクラス

### [x] Tree (木, 非ラベル付き)
| 項目 | 内容 |
|------|------|
| OEIS (unrooted unlabeled) | A000055: 1, 1, 1, 2, 3, 6, 11, 23, 47, 106, ... |
| OEIS (rooted unlabeled) | A000081: 1, 1, 2, 4, 9, 20, 48, 115, 286, ... |
| OEIS (labeled unrooted) | A000272: n^(n-2) (Cayley の公式) |
| 数え上げ | Otter の公式 (1948): 根付き木の数から非根付き木の数を導出。漸近公式あり |
| 列挙 (unrooted) | **CAT (定数償却時間)** (Wright, Richmond, Odlyzko, McKay, SIAM J. Comput. 1986) |
| 列挙 (rooted) | **CAT** (Beyer, Hedetniemi, SIAM J. Comput. 1980): level sequence による生成 |
| 列挙 (labeled) | Prüfer 列の全列挙: n^(n-2) 個を各 O(n) で復号 |
| 実装 | `include/tree_enum.h` — 非同型自由木列挙 (ボトムアップ再帰構成 + 重心分解) |
| 参考文献 | Otter, Ann. Math. 49(3), 1948; Wright et al., SIAM J. Comput. 15(2), 1986; Beyer, Hedetniemi, SIAM J. Comput. 9(4), 1980 |

### [x] Forest (森)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A005195: 1, 1, 2, 3, 6, 10, 20, 37, 76, 153, ... |
| OEIS (labeled) | A001858 (根付き森は (n+1)^(n-1) 個) |
| 数え上げ | 非ラベル付き: A000055 (非根付き木) の Euler 変換。ラベル付き: 指数母関数 exp(T(x)) |
| 列挙 | 木列挙の合成: 分割数の列挙 + 各サイズの木の列挙を組み合わせ |
| 実装 | `include/forest_enum.h` — 整数分割 + 木の合成による非同型森列挙 |
| 参考文献 | Harary, Palmer, "Graphical Enumeration," Academic Press, 1973 |

### [x] Caterpillar (キャタピラー木)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000672: 1, 1, 1, 2, 3, 6, 10, 20, 36, 72, ... |
| OEIS (labeled) | A052471 |
| 数え上げ | **閉じた公式**: Harary-Schwenk (1973) による母関数。spine + 葉の分布で特性化 |
| 列挙 | spine 長を列挙 → 各 spine 頂点への葉の配分 (組合せ) を列挙。spine の反転対称性を考慮 |
| 実装 | `include/caterpillar_enum.h` — 構成的列挙 (spine + 葉配分) |
| 備考 | pathwidth 1 のグラフと一致。全頂点が中心パスから距離 1 以内の木 |
| 参考文献 | Harary, Schwenk, "The number of caterpillars," Discrete Math. 6(4), 1973 |

### [x] k-Tree (k-木)
| 項目 | 内容 |
|------|------|
| OEIS (labeled 2-tree) | A054581 |
| OEIS (unlabeled 2-tree) | A005573: 1, 1, 1, 2, 5, 12, 39, 136, 529, ... |
| OEIS (labeled 3-tree) | A054582 |
| OEIS (labeled 一般) | A054580 (三角配列) |
| 数え上げ (labeled) | Beineke-Pippert (1969): Cayley 型公式の一般化。T_k(n) = C(n,k) · (k(n-k)+1)^(n-k-2) 型 |
| 列挙 | 再帰的 k-クリーク拡張: 既存 k-クリークに新頂点を隣接させて構築。逆探索に適した構造 |
| 実装 | `include/ktree_enum.h` — ラベル付き全列挙 (reverse search, k-clique restricted) |
| 備考 | k=1 は通常の木、k=2 は maximal outerplanar (n≥3)、k=3 は Apollonian network |
| 参考文献 | Beineke, Pippert, J. Combin. Theory 6(2), 1969; Harary, Palmer, "Graphical Enumeration," 1973 |

---

## 構造的性質によるグラフクラス

### [x] Unicyclic (単閉路グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | A001429: 1, 2, 5, 13, 33, 89, 240, 657, 1806, ... |
| 数え上げ | 母関数による導出。n 頂点 n 辺の連結グラフ |
| 列挙 | 閉路長を固定 → 各閉路頂点に根付き木を付与する構成的列挙。bracelet 正規形で二面体対称性を排除 |
| 実装 | `include/unicyclic_enum.h` — 構成的列挙 (閉路 + 根付き木付与) |
| 備考 | 連結グラフで辺数 = 頂点数のもの。ちょうど 1 つの閉路を含む |

### [x] Biconnected (2-連結グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A002218: 0, 0, 1, 3, 10, 56, 468, 7123, 194066, ... |
| OEIS (labeled) | 0, 0, 1, 10, 238, 11366, ... (n=1,...,6) |
| 認識 | O(n+m) Tarjan の切断点検出 |
| 列挙 | 逆探索 (頂点追加 + 連結性枝刈り + 最終ステップで 2-連結判定)。非遺伝的クラスのため中間枝刈りは連結性ベースのみ |
| 実装 | `include/biconnected_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 基本的な構造的性質。切断点を持たない連結グラフ (n ≥ 3)。遺伝的でない (頂点除去で性質が壊れうる) |

### [x] Maximal Planar / Triangulation (極大平面グラフ / 三角形分割)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000109: 1, 1, 1, 2, 5, 14, 50, 233, 1249, 7595, 49566, ... |
| OEIS (rooted) | A000260 |
| 列挙 | **plantri** (Brinkmann, McKay): 毎秒 500 万グラフ以上。canonical construction path 法。償却 O(n^2)/グラフ |
| 実装 | `include/maximal_planar_enum.h` — ラベル付き全列挙 (reverse search + 平面性枝刈り + 辺数制約) |
| 備考 | 全面が三角形の平面グラフ。3-連結平面グラフの双対は三角形分割 |
| 参考文献 | Brinkmann, McKay, MATCH 58, 2007 |

### [x] Triangle-Free (三角形なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A006785: 1, 2, 3, 7, 14, 38, 107, 410, 1897, 12172, ... |
| OEIS (connected unlabeled) | A024607: 1, 1, 1, 3, 6, 19, 59, 267, ... |
| OEIS (labeled) | A345249 |
| 列挙 | **geng -t** (nauty): canonical augmentation + 三角形禁止枝刈り。辺追加時に三角形が生じるかチェック |
| 実装 | `include/triangle_free_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 遺伝的性質。Ramsey 理論と密接に関連 |
| 参考文献 | McKay, J. Algorithms 26, 1998; Colbourn, Read, J. Graph Theory 3, 1979 |

### [x] Eulerian (オイラーグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A002854 |
| OEIS (connected unlabeled) | A003049: 1, 0, 1, 1, 4, 8, 37, 184, 1782, 31026, ... |
| 数え上げ (labeled) | **閉じた公式**: 2^((n-1)(n-2)/2) (全頂点の次数が偶数の条件は GF(2) 上の n-1 個の独立な線形制約) |
| 列挙 | サイクル空間基底列挙: K_n の全域木の基本サイクルの全部分集合の対称差を列挙。geng + 偶数次数フィルタも可。非ラベル付きは Polya/Burnside 法 |
| 実装 | `include/eulerian_enum.h` — ラベル付き全列挙 (サイクル空間基底列挙) |
| 備考 | 全頂点の次数が偶数のグラフ。連結なら Euler 回路を持つ |
| 参考文献 | Harary, Palmer, "Graphical Enumeration," Academic Press, 1973 |

### [x] Self-Complementary (自己補的グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000171: 1, 0, 0, 1, 2, 0, 0, 10, 36, 0, 0, 720, 5600, ... |
| OEIS (labeled) | 1, 0, 0, 12, 24, 0, 0, 98280, ... (n=1,...,8) |
| 数え上げ | **閉じた公式** (Read, 1963): Polya 型巡回指標を用いた列挙。a(n)=0 when n mod 4 ∈ {2,3} |
| 列挙 | 補置換 (complementing permutation) σ の構造を利用した構成的列挙。有効なサイクル型 (2の冪 ≥4) の全置換について辺対の軌道を計算し、各軌道の 2 通りの選択肢を全列挙 |
| 実装 | `include/self_complementary_enum.h` — ラベル付き全列挙 (complementing permutation) |
| 備考 | n(n-1)/4 が整数でないと存在しない (n ≡ 0,1 mod 4 のみ) |
| 参考文献 | Read, J. London Math. Soc. 38, 1963; Farrugia, Ph.D. thesis, Univ. Malta, 1999 |

---

## 正則グラフクラス

### [x] k-Regular (k-正則グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (三角配列) | A051031: (n 頂点, 次数 k) の非同型 k-正則グラフ数 |
| OEIS (全次数合計) | A005176 |
| OEIS (4-regular) | A033301 |
| OEIS (5-regular) | A165626 |
| 列挙 | **GENREG** (Meringer, 1999): orderly generation + 高速 canonicity テスト。任意の k, n に対応 |
| 実装 | `include/kregular_enum.h` — ラベル付き全列挙 (次数制約付き逆探索) |
| 備考 | 並列化可能 (Rouyer et al., 2019: 4-正則 23 頂点まで列挙) |
| 参考文献 | Meringer, J. Graph Theory 30, 1999, pp. 137-146 |

### [x] Cubic (三次グラフ / 3-正則グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A005638: 0, 1, 2, 6, 21, 94, 540, 4207, ... |
| OEIS (connected unlabeled) | A002851: 0, 1, 2, 5, 19, 85, 509, 4060, ... |
| OEIS (labeled) | A004109 |
| 列挙 | **snarkhunter** (Brinkmann, Goedgebeur, McKay): 三次グラフ専用生成器。canonical deletion 使用。girth 制約付きで geng の 30 倍以上高速 |
| 実装 | `include/cubic_enum.h` — ラベル付き全列挙 (reverse search, 次数 3 制約付き) |
| 備考 | snark (橋なし三次グラフで 3-辺彩色不可) の列挙にも使用 |
| 参考文献 | Brinkmann, Goedgebeur, McKay, J. Graph Theory 86, 2017; Brinkmann, J. Graph Theory 23(2), 1996 |

### [x] Strongly Regular (強正則グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | A088741 (パラメータ依存、単一数列なし) |
| OEIS (labeled) | 0, 0, 0, 6, 12, 50, 0, 280, 5600, ... (n=1,...,9) |
| 数え上げ | パラメータ (n,k,λ,μ) ごとに個別に列挙。小さい n では完全分類済み |
| 列挙 | パラメータ固定の網羅的探索 + 固有値実現可能性テストで枝刈り (McKay, Spence) |
| 実装 | `include/strongly_regular_enum.h` — ラベル付き全列挙 (パラメータ制約付きバックトラッキング) |
| 備考 | Spence のデータベースで 64 頂点以下の全実現可能パラメータの完全リストあり |
| 参考文献 | McKay, Spence, Australas. J. Combin. 24, 2001; Brouwer パラメータ表 |

---

## 交差グラフ・幾何的グラフクラス (追加)

### [x] Circle (円グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A156809: 1, 2, 4, 11, 34, 154, 978, 9497, 127954, ... |
| OEIS (labeled) | 1, 2, 8, 64, 1024, 32636, ... (n=1,...,6) |
| 認識 | O(n+m) 線形時間 (Paul, Rutter, STACS 2026); O(n^2) (Spinrad, 1994) |
| 列挙 | canonical deletion 法 (Johnston, 2020): n=13 まで計算済み (22,576,188,846 個) |
| 実装 | `include/circle_enum.h` — ラベル付き全列挙 (reverse search + DOW backtracking 認識) |
| 備考 | 円の弦の交差グラフ。permutation の超クラス。n≤5 では全グラフが circle |
| 参考文献 | Spinrad, Discrete Math. 128, 1994; Paul, Rutter, arXiv:2512.23492, 2025; Johnston, 2020 |

### [x] Halin (ハリングラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A346779: 0, 0, 0, 1, 1, 2, 2, 4, 6, 13, 22, 50, 106, 252, ... |
| 認識 | O(n+m) 線形時間: 平面的 + 3-連結 + 外面の頂点数条件 |
| 列挙 | 構成的列挙: 非同型 HI-tree の全平面埋め込みを列挙し、平面木カノニカルコードで重複除去 |
| 実装 | `include/halin_enum.h` — 非同型全列挙 (HI-tree + 平面埋め込み + ブラケットコード正規化) |
| 備考 | 葉のない木に外面閉路を追加。cubic polyhedral グラフと関連 |
| 参考文献 | Halin, Combinatorial Mathematics and its Applications, 1971 |

### [x] Apex (頂点グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A215620: 1, 2, 4, 11, 34, 155, 1026, 11666, ... |
| OEIS (nonplanar apex) | A215621 |
| OEIS (labeled) | 1, 2, 8, 64, 1024, 32767, ... (n=1,...,6) |
| 認識 | O(n(n+m)): 各頂点を除去して平面性テスト |
| 列挙 | 逆探索 (頂点追加 + apex 判定)。マイナー閉 (遺伝的) クラスのため枝刈り可能 |
| 実装 | `include/apex_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 1 頂点除去で平面になるグラフ。マイナー閉。n ≤ 5 では全グラフが apex (除去後 ≤ 4 頂点で常に平面)。n=6 で K_6 のみ非 apex |

---

## 禁止部分グラフ系クラス (追加)

### [x] Cluster (P3-free / クリーク非交和)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | A000110 (Bell 数): 1, 1, 2, 5, 15, 52, 203, 877, 4140, ... |
| OEIS (unlabeled) | A000041 (整数分割数): 1, 1, 2, 3, 5, 7, 11, 15, 22, ... |
| 認識 | O(n+m): 各連結成分がクリークか検査 |
| 列挙 | 集合分割の再帰的列挙 → 各ブロックをクリークとしてグラフ構築。構成的列挙、フィルタなし |
| 実装 | `include/cluster_enum.h` — ラベル付き全列挙 (set partition construction) |
| 備考 | P3-free ⟺ クリークの非交和。cograph (P4-free) の部分クラス。threshold の部分クラス |
| 参考文献 | Knuth, "The Art of Computer Programming" Vol. 4A (集合分割列挙); OEIS A000110 |

### [x] Bull-Free (ブルなしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | A079575: 1, 1, 2, 6, 20, 93, 480, 2960, 19475, ... |
| OEIS (labeled) | 1, 2, 8, 64, 964, ... (n=1,...,5) |
| 認識 | O(m*Δ²) (三角形列挙 + ペンダント探索); O(n^5) ブルートフォース |
| 列挙 | 逆探索 (頂点追加 + bull-free 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/bull_free_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | bull は三角形 + 2 本のペンダント辺 (5 頂点 5 辺)。n ≤ 4 では全グラフが bull-free |
| 参考文献 | Chudnovsky, "The structure of bull-free graphs I-III," JCTB, 2012 |

### [x] Parity (パリティグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| OEIS (labeled) | 1, 2, 8, 64, 892, 18584, 521096, ... (n=1,...,7) |
| 認識 | 直接定義検査: 全頂点ペアの BFS 距離と誘導パス偶奇性を DFS バックトラッキングで検証。O(n+m) 線形時間も可 (split decomposition) |
| 列挙 | 逆探索 (reverse search): 遺伝的クラスの性質を利用した枝刈り列挙 |
| 実装 | `include/parity_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 同じ 2 頂点間の任意の誘導パスが同じ偶奇性を持つグラフ。n ≤ 4 では全グラフがパリティグラフ (C₅ が最小の非パリティグラフ) |
| 参考文献 | Burlet, Uhry, Annals of Discrete Math., 1984; Bouchet, Combinatorica, 1987 |

### [x] Meyniel (メイニエルグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| OEIS (labeled) | 1, 2, 8, 64, 952, 23744, ... (n=1,...,6) |
| 認識 | O(n^2) (Lévêque, Lin, Maffray, Trotignon, TCS 2009) |
| 列挙 | 逆探索 (頂点追加 + Meyniel 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/meyniel_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 長さ 5 以上の奇閉路が全て 2 本以上の弦を持つグラフ。n ≤ 4 では全グラフが Meyniel |
| 参考文献 | Burlet, Fonlupt, Annals of Discrete Math., 1984; Lévêque et al., TCS, 2009 |

### [x] Even-Hole-Free (偶数穴なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| OEIS (labeled) | 1, 2, 8, 61, 834, ... (n=1,...,5) |
| 認識 | O(n^9) (Lai, Lu, Thorup, STOC 2020)。歴史: O(n^40) → O(n^31) → O(n^19) → O(n^11) → O(n^9) |
| 列挙 | 逆探索 (頂点追加 + even-hole-free 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/even_hole_free_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 最小の偶���穴は C4 (4 頂点)。n ≤ 3 では全グラフが even-hole-free |
| 参考文献 | Conforti et al., JCTB, 2002; Lai, Lu, Thorup, STOC 2020 |

### [x] Odd-Hole-Free (奇数穴なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| OEIS (labeled) | 1, 2, 8, 64, 1012, ... (n=1,...,5) |
| 認識 | O(n^9) (Chudnovsky, Scott, Seymour, Spirkl, JACM 2020)。数十年間のオープン問題を解決 |
| 列挙 | 逆探索 (頂点追加 + odd-hole-free 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/odd_hole_free_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | perfect = odd-hole-free ∩ odd-antihole-free (SPGT)。n ≤ 4 では全グラフが odd-hole-free (C₅ が最小の odd hole) |
| 参考文献 | Chudnovsky, Scott, Seymour, Spirkl, JACM 67(1), 2020 |

### [x] 4-Leaf Power (4-leaf power)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| OEIS (labeled) | 1, 2, 8, 61, 822, 17194, ... (n=1,...,6) |
| 認識 | O(n+m) 線形時間 (Brandstädt, Le, Sritharan, ACM Trans. Algorithms, 2008) |
| 列挙 | chordal 逆探索 + 4-leaf power フィルタ (3-leaf power と同様のアプローチ) |
| 実装 | `include/four_leaf_power_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | strongly chordal の部分クラス。critical clique graph の tree subdivision 可能性で判定。n≤5 では全弦グラフが 4-leaf power。n=6 で 4 個の最小禁止誘導部分グラフが出現 |
| 参考文献 | Brandstädt, Le, Sritharan, ACM Trans. Algorithms, 2008 |

### [x] 5-Leaf Power (5-leaf power)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| OEIS (labeled) | 1, 2, 8, 61, 822, 18034, ... (n=1,...,6) |
| 認識 | O(n+m) 線形時間 (Chang, Ko, 2007) |
| 列挙 | chordal 逆探索 + 5-leaf power フィルタ (CC + tree subdivision, 閾値 ≤3/≥4) |
| 実装 | `include/five_leaf_power_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | k-leaf power と (k+1)-leaf power は k≥4 で比較不能 (Fellows et al.)。n≤5 では全弦グラフが 5-leaf power。n=6 で 4-leaf power (17194) と差が出る |
| 参考文献 | Chang, Ko, 2007; Lafond, ACM Trans. Algorithms, 2023 (一般 k の多項式時間認識) |

### [x] Gem-Free (gem なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | 1, 2, 8, 64, 964, 25376, ... (n=1,...,6) |
| 認識 | O(n*m*Δ) (近傍 P4 探索); O(n^5) ブルートフォース |
| 列挙 | 逆探索 (頂点追加 + gem-free 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/gem_free_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | gem (fan F_{1,3}) は P4 + universal vertex (5 頂点 7 辺)。n ≤ 4 では全グラフが gem-free |
| 参考文献 | Brandstädt, Le, Spinrad, "Graph Classes: A Survey," SIAM, 1999 |

### [x] P5-Free (P5 なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | 1, 2, 8, 64, 964, 24968, ... (n=1,...,6) |
| 認識 | パス伸長探索 (デフォルト); O(n^5) ブルートフォース |
| 列挙 | 逆探索 (頂点追加 + P5-free 判定)。遺伝的クラスのため枝刈り可能 |
| 実装 | `include/p5_free_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | P5 は長さ 4 の誘導パス (5 頂点 4 辺)。P3-free = cluster, P4-free = cograph に続く Pk-free 系列。n ≤ 4 では全グラフが P5-free |
| 参考文献 | Brandstädt, Le, Spinrad, "Graph Classes: A Survey," SIAM, 1999; Bacsó, Tuza, Dominating cliques in P5-free graphs, Period. Math. Hungar. 21, 1990 |

---

## 平面グラフの特殊クラス (追加)

### [x] Polyhedral / 3-Connected Planar (多面体グラフ / 3-連結平面グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000944: 0, 0, 0, 1, 2, 7, 34, 257, 2606, 32300, 440564, 6384634, ... |
| OEIS (labeled) | 0, 0, 0, 1, 25, 1227, 131412, ... (n=1,...,7) |
| 定義 | 3-頂点連結平面グラフ。Steinitz の定理により凸多面体の辺骨格と一致 |
| 列挙 | **plantri** (Brinkmann, McKay): canonical construction path 法。毎秒 500 万グラフ以上。n ≤ 18 まで計算済み |
| 実装 | `include/polyhedral_enum.h` — ラベル付き全列挙 (reverse search + 平面性枝刈り + 3-連結判定) |
| 参考文献 | Duijvestijn, Federico, "The Number of Polyhedral (3-Connected Planar) Graphs," Math. Comp. 37, 1981; Brinkmann, McKay, MATCH 58, 2007 |
| PDF | `references/brinkmann2007_plantri.pdf` |
| 備考 | plantri の `-p` オプションで直接生成可能。maximal planar (三角形分割) と dual 関係にある。3-連結は遺伝的でないため、中間ステップでは平面性 (遺伝的) + 連結性 + 次数の枝刈りを行い、最終ステップで 3-連結判定 |

### [ ] Simple Quadrangulation (単純四角形分割)
| 項目 | 内容 |
|------|------|
| OEIS (3-connected, min degree 3) | A078666 |
| 定義 | 球面の単純四角形分割 (全面が 4-gon)。辺の共有以外の交差なし |
| 列挙 | **plantri** (Brinkmann, McKay): 基本グラフ (八面体等) から局所変形 ({C4}; P0, P1) で生成。毎秒 27 万グラフ。3-連結 / 最小次数 3 / non-facial 4-cycle 禁止 等のフィルタ可能 |
| 参考文献 | Brinkmann, McKay, "Generation of simple quadrangulations of the sphere," Discrete Math. 305, 2005 |
| PDF | `references/brinkmann2005_quadrangulation.pdf` |
| 備考 | 双対は 4-正則平面グラフ。3-連結四角形分割はアルキメデス立体の骨格等を含む |

### [ ] Cubic Planar (三次平面グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | 三角形分割の dual として A000109 に対応。直接: plantri `-b` オプション |
| 定義 | 全頂点の次数が 3 の平面グラフ (三角形分割の dual) |
| 列挙 | **plantri** (Brinkmann, McKay): 三角形分割を生成し dual を取るか、直接 cubic planar を生成。2-連結 / 3-連結版も対応 |
| 参考文献 | Brinkmann, McKay, MATCH 58, 2007 |
| PDF | `references/brinkmann2007_plantri.pdf` |
| 備考 | fullerene, snark, Halin グラフ等の上位クラス |

### [ ] Fullerene (フラーレングラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A007894: 1, 0, 1, 1, 2, 3, 6, 6, 15, 17, 40, 45, 89, ... (n=20, 22, 24, ...) |
| OEIS (with enantiomers) | A057210 |
| 定義 | 三次平面グラフで全面が五角形 (12 個) または六角形のもの。頂点数は必ず偶数 ≥ 20 |
| 数え上げ | **正確な公式**: Engel, Smillie (Duke Math. J. 2025) が modular form を用いた正確な列挙公式を導出。漸近公式 a(n) ~ c · n^9 |
| 列挙 | **fullgen** (Brinkmann): 三角形分割から構築。**buckygen** (Brinkmann, Goedgebeur, McKay): fullgen の 3.5 倍高速。IPR (isolated pentagon rule) フィルタ対応 |
| 参考文献 | Brinkmann, Goedgebeur, McKay, J. Chem. Inf. Model. 52, 2012 (buckygen); Engel, Smillie, Duke Math. J. 174(3), 2025 (exact enumeration) |
| PDF | `references/goedgebeur2013_fullerene_generation.pdf`, `references/engel2023_fullerene_enum.pdf` |
| 備考 | C60 (サッカーボール) が代表例。化学・材料科学で重要。n=20 のみ正十二面体 |

---

## スナーク・ハミルトン関連

### [ ] Snark (スナーク)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A130315: 1, 0, 0, 0, 2, 6, 20, 38, 280, 2900, 28399, 293059, ... (n=10, 12, 14, ...) |
| 定義 | 橋なし三次グラフで辺彩色数 (chromatic index) が 4 のもの。cyclically 4-edge-connected かつ girth ≥ 5 の定義も使われる |
| 列挙 | **snarkhunter** (Brinkmann, Goedgebeur): 三次グラフ専用生成器。look-ahead による 3-辺彩色可能性判定を組み込み、girth ≥ k (k=4,5,6,7) のフィルタに対応。girth ≥ 6 のスナークは 38 頂点まで、girth ≥ 7 は 42 頂点まで全列挙済み |
| 参考文献 | Brinkmann, Goedgebeur, J. Combin. Theory Ser. B 103, 2013 (generation and properties); Brinkmann, Goedgebeur, J. Graph Theory 86, 2017 (large girth); Brinkmann, Goedgebeur, Mattiolo, arXiv:2603.17789, 2026 (new algorithms) |
| PDF | `references/goedgebeur2013_snarks_properties.pdf`, `references/brinkmann2017_cubic_snarks.pdf` |
| 備考 | 四色定理の反例候補として歴史的に重要。Petersen グラフ (10 頂点) が最小のスナーク。頂点数は必ず偶数 |

### [ ] Hypohamiltonian (準ハミルトングラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A141150: 1, 0, 0, 1, 0, 1, 0, 14, 34, ... (n=10, 11, ..., 19) |
| 定義 | 非ハミルトンだが任意の 1 頂点を除去するとハミルトンになるグラフ |
| 列挙 | Goedgebeur, Zamfirescu (2017): 専用生成アルゴリズムにより全非同型 hypohamiltonian グラフを列挙。17 頂点以下の完全リスト確立。n=18 で 14 個、n=19 で 34 個 |
| 参考文献 | Goedgebeur, Zamfirescu, Ars Math. Contemp. 13, 2017 (improved bounds); Goedgebeur, Zamfirescu, Discrete Math. 347, 2024 (K₂-hypohamiltonian) |
| PDF | `references/goedgebeur2017_hypohamiltonian.pdf`, `references/goedgebeur2024_k2_hypohamiltonian.pdf` |
| 備考 | Petersen グラフ (10 頂点) が最小の hypohamiltonian グラフかつ唯一の 10 頂点例。11, 12, 14, 17 頂点の hypohamiltonian グラフは存在しない。平面 hypohamiltonian の最小頂点数は 23 以上 |

---

## 剛性理論

### [x] Laman Graph (ラマングラフ / 最小剛性グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A227117: 1, 1, 1, 1, 3, 13, 70, 608, 7222, 110132, 2039273, 44176717, ... |
| OEIS (labeled) | 1, 1, 1, 6, 100, 3355, 190491, ... (n=1,...,7) |
| 定義 | n 頂点 2n-3 辺のグラフで、任意の k 頂点部分グラフの辺数が 2k-3 以下 ((2,3)-tight graph)。2 次元最小剛性グラフと一致 |
| 列挙 | **nauty-laman-plugin** (Larsson): geng のプラグインとして (2,3)-sparse / tight グラフを高速生成。Henneberg 構成 (頂点追加 + 辺分割) による構成的列挙も可能 |
| 実装 | `include/laman_enum.h` — ラベル��き全列挙 (reverse search + (2,3)-sparsity 増分部分集合検査) |
| 参考文献 | Laman, J. Engrg. Math. 4, 1970 (特性化); Larsson, GitHub: nauty-laman-plugin; Capco, Gallet, Grasegger, Koutschan, Lubbes, Schicho, SIAM J. Appl. Algebra Geom., 2018 (実現数) |
| 備考 | 剛性マトロイドの基。2 次元の棒と関節の最小剛性系を記述。平面 Laman グラフ (non-crossing) は reverse search で列挙可能 |

---

## 有向グラフ・半順序

### [x] Digraph (有向グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000273: 1, 1, 3, 16, 218, 9608, 1540944, 882033440, ... |
| OEIS (connected) | A003085 |
| OEIS (labeled) | A000088 の有向版 (2^(n(n-1)) ラベル付き有向グラフ、うち非同型は A000273) |
| 列挙 | nauty/**directg**: 無向グラフの辺を全方向に向き付け、同型な有向グラフを抑制。geng と組み合わせて `geng n \| directg` で全非同型有向グラフを生成 |
| 数え上げ | Burnside の補題 + 巡回指標 (Pólya 型) |
| 実装 | `include/digraph_enum.h` — ラベル付き全列挙 (構成的 DFS, 各ペア 4 分岐) |
| 参考文献 | Harary, Palmer, "Graphical Enumeration," Academic Press, 1973; McKay, nauty User's Guide |
| 備考 | self-loop なし・多重辺なしの単純有向グラフ。各辺は一方向のみ (双方向は 2 本の有向辺) |

### [x] Tournament (トーナメント)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000568: 1, 1, 1, 2, 4, 12, 56, 456, 6880, 191536, 9733056, ... |
| OEIS (connected / strong) | A000571 (強連結トーナメント) |
| 定義 | 完全グラフの全辺に向きを付けた有向グラフ (全頂点ペアが比較可能) |
| 列挙 | nauty/**gentourng**: トーナメント専用の非同型生成器。出次数制約オプション対応 |
| 実装 | `include/tournament_enum.h` — ラベル付き全列挙 (構成的 K_n 全方向付け DFS) |
| 数え上げ | Burnside の補題 + 対称群の巡回指標 |
| 参考文献 | Harary, Palmer, "Graphical Enumeration," Academic Press, 1973; Moon, "Topics on Tournaments," Holt, Rinehart & Winston, 1968 |
| 備考 | ラウンドロビン戦の結果と全単射。n! 個のラベル付きトーナメントから非同型クラスを抽出 |

### [x] Poset / Partial Order (半順序集合)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000112: 1, 1, 2, 5, 16, 63, 318, 2045, 16999, 183231, 2567284, ... |
| OEIS (labeled) | A001035: 1, 1, 3, 19, 219, 4231, 130023, ... |
| 定義 | 反射的・反対称的・推移的な二項関係を持つ有限集合。Hasse 図は DAG (有向非巡回グラフ) |
| 列挙 | nauty/**genposetg** (Brinkmann): Hasse 図 (推移的簡約 DAG) の非同型生成。16 頂点まで列挙済み (Brinkmann, McKay, 2002) |
| 実装 | `include/poset_enum.h` — ラベル付き全列挙 (ペアワイズ DFS + 増分推移閉包) |
| 数え上げ | 閉じた公式は未知。計算困難 (#P-hard と予想) |
| 参考文献 | Brinkmann, McKay, "Posets on up to 16 Points," Order 19(2), 2002; Heitzig, Reinhold, "Counting Finite Lattices," Algebra Universalis 48, 2002 |
| 備考 | T₀ 位相空間の数と一致 (A000112)。比較可能性グラフは poset から構成される無向グラフ |

### [ ] Lattice (束)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A006966: 1, 1, 1, 1, 2, 5, 15, 53, 222, 1078, 5994, 37622, ... |
| 定義 | 任意の 2 元が上限 (join) と下限 (meet) を持つ半順序集合 |
| 列挙 | poset 列挙 + 束条件フィルタ。専用アルゴリズムも存在 (Heitzig, Reinhold, 2002) |
| 参考文献 | Heitzig, Reinhold, Algebra Universalis 48, 2002 |
| 備考 | poset の重要な部分クラス。n=18 まで計算済み |

---

## 交差グラフの数え上げ理論 (追加)

### [ ] x-Monotone Curve Intersection Graph (x-単調曲線交差グラフ)
| 項目 | 内容 |
|------|------|
| 定義 | x-単調曲線 (任意の垂直線と高々 1 点で交わる曲線) の交差グラフ。pseudo-segment の場合、任意のペアが高々 1 点で交差 |
| 数え上げ | ラベル付き: 2^Ω(n^{4/3}) 個の異なる交差グラフ (下界)、2^O(n^{4/3} log² n) (上界)。Fox, Pach, Suk (GD 2024) |
| 参考文献 | Fox, Pach, Suk, Proc. GD 2024, LIPIcs; Kynčl, Discrete Comput. Geom. 50, 2013 |
| PDF | `references/fox2024_xmonotone_curves.pdf` |
| 備考 | string graph (一般曲線交差) の部分クラス。VC-dimension の新しい上界を利用。彩色数制約付きのより良い上界も得られる |

---

## 主要ソフトウェア (追加)

| ツール | 用途 | URL / 備考 |
|--------|------|------------|
| **fullgen** | フラーレングラフ生成 | plantri パッケージに同梱 |
| **buckygen** | フラーレングラフ高速生成 (fullgen の 3.5 倍) | Brinkmann, Goedgebeur, McKay |
| **snarkhunter** | スナーク・三次グラフ生成 (girth 制約付き) | Brinkmann, Goedgebeur |
| **directg** | 無向→有向グラフ全方向付け生成 | nauty パッケージに同梱 |
| **gentourng** | トーナメント (完全有向グラフ) 生成 | nauty パッケージに同梱 |
| **genposetg** | 半順序集合 (Hasse 図) 生成 | nauty パッケージに同梱 (Brinkmann) |
| **nauty-laman-plugin** | Laman グラフ (最小剛性グラフ) 生成 | geng プラグイン (Larsson) |
| **House of Graphs** | グラフデータベース・カタログ | https://houseofgraphs.org/ (Coolsaet, D'hondt, Goedgebeur) |

---

## 参考文献 PDF 一覧 (`references/`)

| ファイル名 | 内容 |
|-----------|------|
| `mckay1998_isomorph_free_generation.pdf` | McKay, Canonical augmentation 汎用フレームワーク |
| `hebert-johnson2023_counting_chordal.pdf` | Chordal グラフの多項式時間数え上げ |
| `galvin2022_threshold_enum.pdf` | Threshold, quasi-threshold グラフの列挙 |
| `chauve2017_distance_hereditary_enum.pdf` | Distance-hereditary グラフの正確な数え上げ |
| `kawahara2024_bdd_intersection_graphs.pdf` | BDD ベース交差グラフ列挙 (proper interval, bipartite permutation, chain, cochain, threshold) |
| `kiyomi2019_chordal_bipartite_enum.pdf` | Chordal bipartite 誘導部分グラフ列挙 |
| `conte2022_proximity_search.pdf` | Proximity search 汎用フレームワーク |
| `brinkmann2007_plantri.pdf` | plantri: 平面グラフ高速生成 |
| `bodirsky2007_outerplanar_enum.pdf` | Outerplanar グラフの数え上げ・漸近解析 |
| `bahrani2017_cactus_enum.pdf` | Cactus グラフの列挙・ランダム生成 |
| `brinkmann2005_quadrangulation.pdf` | 球面の単純四角形分割の生成 |
| `brinkmann2017_cubic_snarks.pdf` | 三次グラフ・スナーク (large girth) の生成 |
| `goedgebeur2013_fullerene_generation.pdf` | フラーレングラフの高速生成 (buckygen) |
| `goedgebeur2013_snarks_properties.pdf` | スナークの生成と性質 |
| `goedgebeur2017_hypohamiltonian.pdf` | 準ハミルトングラフの生成と上下界 |
| `goedgebeur2024_k2_hypohamiltonian.pdf` | K₂-準ハミルトングラフの生成と無限族 |
| `engel2023_fullerene_enum.pdf` | フラーレンの正確な数え上げ (modular form) |
| `fox2024_xmonotone_curves.pdf` | x-単調曲線交差グラフの数え上げ |

### 追加参考文献 (PDF 未収録)

| 文献 | 内容 |
|------|------|
| Wright, Richmond, Odlyzko, McKay, SIAM J. Comput. 15(2), 1986 | 非ラベル付き木の CAT 生成 |
| Beyer, Hedetniemi, SIAM J. Comput. 9(4), 1980 | 根付き木の CAT 生成 |
| Otter, Ann. Math. 49(3), 1948 | 木の数え上げ公式 |
| Harary, Palmer, Academic Press, 1973 | "Graphical Enumeration" 教科書 |
| Harary, Schwenk, Discrete Math. 6(4), 1973 | Caterpillar の数え上げ |
| Beineke, Pippert, J. Combin. Theory 6(2), 1969 | ラベル付き k-tree の数え上げ |
| Meringer, J. Graph Theory 30, 1999 | GENREG: k-正則グラフの高速生成 |
| Brinkmann, Goedgebeur, McKay, J. Graph Theory 86, 2017 | snarkhunter: 三次グラフ生成 |
| Read, J. London Math. Soc. 38, 1963 | 自己補的グラフの数え上げ |
| Colbourn, Read, Int. J. Comput. Math. 7, 1979 | orderly generation の古典的枠組み |
| Colbourn, Read, J. Graph Theory 3(2), 1979 | restricted graph classes 向け orderly generation |
| Mestre, JGAA 13(2), 2009 | connected / 2-edge-connected グラフの再帰生成 |
| Johnston, 2020 | 円グラフの canonical deletion 列挙 |
| Jones, Protti, Del-Vecchio, TCS 713, 2018 | cograph の linear-delay 非同型列挙 |
| Bahrani, Lumbroso, Electron. J. Combin. 25(4), 2018 | block / ptolemaic / cactus variants の split-decomposition ベース full enumeration |
| Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024 | proper interval / bipartite permutation / chain / cochain / threshold の BDD 列挙 |
| Yamazaki, Qian, Uehara, Discrete Appl. Math. 342, 2024 | distance-hereditary, cograph, ptolemaic, 3-leaf power などの O(n^3) delay 非同型列挙 |
| Chudnovsky, Scott, Seymour, Spirkl, JACM 67(1), 2020 | 奇数穴検出 O(n^9) |
| Lai, Lu, Thorup, STOC 2020 | 偶数穴検出 O(n^9) |
| Brandstädt, Le, Sritharan, ACM Trans. Algorithms, 2008 | 4-leaf power の線形時間認識 |
| Lafond, ACM Trans. Algorithms, 2023 | 一般 k-leaf power の多項式時間認識 |
| Duijvestijn, Federico, Math. Comp. 37, 1981 | 多面体グラフ (3-connected planar) の数え上げ |
| Brinkmann, McKay, Discrete Math. 305, 2005 | 球面の単純四角形分割の生成 |
| Brinkmann, Goedgebeur, McKay, J. Chem. Inf. Model. 52, 2012 | buckygen: フラーレングラフの高速生成 |
| Engel, Smillie, Duke Math. J. 174(3), 2025 | フラーレンの正確な数え上げ (modular form) |
| Brinkmann, Goedgebeur, J. Combin. Theory Ser. B 103, 2013 | スナークの生成と性質 |
| Brinkmann, Goedgebeur, J. Graph Theory 86, 2017 | large girth 三次グラフ・スナークの生成 |
| Brinkmann, Goedgebeur, Mattiolo, arXiv:2603.17789, 2026 | スナーク生成の新アルゴリズム |
| Goedgebeur, Zamfirescu, Ars Math. Contemp. 13, 2017 | 準ハミルトングラフの生成と上下界 |
| Goedgebeur, Zamfirescu, Discrete Math. 347, 2024 | K₂-準ハミルトングラフの生成と無限族 |
| Laman, J. Engrg. Math. 4, 1970 | 2 次元最小剛性グラフの特性化 |
| Larsson, GitHub: nauty-laman-plugin | geng プラグインによる Laman グラフ生成 |
| Brinkmann, McKay, Order 19(2), 2002 | 16 頂点以下の半順序集合の列挙 |
| Heitzig, Reinhold, Algebra Universalis 48, 2002 | 有限束の数え上げ |
| Moon, "Topics on Tournaments," 1968 | トーナメント理論の古典的教科書 |
| Fox, Pach, Suk, Proc. GD 2024, LIPIcs | x-単調曲線交差グラフの数え上げ |
| Coolsaet, D'hondt, Goedgebeur, Discrete Appl. Math. 319, 2022 | House of Graphs 2.0 データベース |

---

## 実装優先度の提案

### 実装済みクラス
1. ~~**Threshold**~~ - バイナリ文字列からの直接構築、最も単純
2. ~~**Chain**~~ - 閉じた公式あり、直接構築可能
3. ~~**Proper Interval**~~ - reverse search, O(1)/グラフ
4. ~~**Bipartite Permutation**~~ - reverse search, O(1)/グラフ
5. ~~**Series-Parallel**~~ - O(1)/グラフ (Kawano-Nakano)
6. ~~**Cograph**~~ - cotree の列挙に帰着
7. ~~**Interval**~~ - BDD ベースまたは多項式遅延
8. ~~**Permutation**~~ - BDD ベースまたは canonical deletion
9. ~~**Planar**~~ - plantri 連携
10. ~~**Outerplanar**~~ - plantri 連携

### 未実装クラス (効率的な専用アルゴリズムが存在するもの)

| 優先度 | クラス | 理由 |
|--------|--------|------|
| ~~高~~ | ~~**Tree (木)**~~ | ~~CAT (定数償却時間) アルゴリズムあり (Wright et al. 1986)~~ |
| ~~高~~ | ~~**Caterpillar**~~ | ~~閉じた数え上げ公式 + spine 構造からの直接構築~~ |
| ~~高~~ | ~~**k-Tree (2-tree)**~~ | ~~Beineke-Pippert 公式 + 再帰的クリーク拡張~~ |
| ~~高~~ | ~~**Maximal Planar**~~ | ~~plantri で毎秒 500 万グラフ以上~~ |
| ~~高~~ | ~~**4-Leaf Power**~~ | ~~線形時間認識、chordal 逆探索 + フィルタ (3-leaf power と同アプローチ)~~ |
| ~~中~~ | ~~**Cubic (3-正則)**~~ | ~~snarkhunter で高速生成~~ |
| ~~中~~ | ~~**k-Regular**~~ | ~~GENREG で任意の k に対応~~ |
| ~~中~~ | ~~**Circle**~~ | ~~canonical deletion で n=13 まで計算済み~~ |
| ~~中~~ | ~~**Triangle-Free**~~ | ~~geng -t で効率的な枝刈り~~ |
| ~~中~~ | ~~**Unicyclic**~~ | ~~閉路 + 根付き木の構成的列挙~~ |
| ~~中~~ | ~~**5-Leaf Power**~~ | ~~線形時間認識、chordal 逆探索 + フィルタ~~ |
| ~~低~~ | ~~**Parity**~~ | ~~逆探索 (遺伝的クラス + 直接定義検査)~~ |
| ~~低~~ | ~~**Meyniel**~~ | ~~逆探索 (遺伝的クラス + 直接定義検査)~~ |
| ~~低~~ | ~~**Self-Complementary**~~ | ~~閉じた公式あり (Read 1963)~~ |
| ~~低~~ | ~~**Cluster (P3-free)**~~ | ~~集合分割の構成的列挙 (Bell 数)~~ |

### 追加実装済みクラス
| 優先度 | クラス | 理由 |
|--------|--------|------|
| ~~中~~ | ~~**P5-Free**~~ | ~~逆探索 (遺伝的クラス + パス伸長探索認識)。Pk-free 系列の自然な拡張~~ |

フィルタ方式 (`geng n | ./recognizer`) は全クラスに適用可能で、n ≤ 12 程度まで実用的。
