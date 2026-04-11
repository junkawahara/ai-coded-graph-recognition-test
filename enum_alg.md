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

### Proximity Search (Conte & Uno, 2022)
- 極大部分グラフの列挙に特化。解グラフの出次数を削減しつつ強連結性を維持し、多項式遅延を実現
- **参考文献**: Conte, Grossi, Marino, Uno, Versari, SIAM J. Computing, 2022 (STOC 2019)
- **PDF**: `references/conte2022_proximity_search.pdf`

### BDD ベース列挙 (Kawahara et al., 2024)
- O(n)-bit 文字列表現を持つ交差グラフクラスに対し、BDD 上で n に関する多項式時間で列挙
- 対象: interval, permutation, bipartite permutation, convex bipartite, biconvex bipartite 等
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
| 列挙 | 多項式遅延 (Yamazaki, Saitoh, Kiyomi, Uehara, WALCOM 2018); BDD ベース (Kawahara et al., 2024) |
| 実装 | `include/interval_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Hanlon, Trans. AMS 272, 1982; Yang, Pippenger, Proc. AMS Ser. B 4, 2017 |

### [x] Proper Interval / Unit Interval (固有インターバルグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | A005217 系列 (Hanlon の列挙) |
| 列挙 | reverse search, **O(1) amortized**/グラフ |
| 実装 | `include/proper_interval_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Saitoh, Yamanaka, Kiyomi, Uehara, WALCOM 2009 / IEICE Trans. E93-D(7), 2010 |

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
| 列挙 | バイナリ文字列 (各ステップで孤立点 or 支配点を追加) の全列挙 |
| 参考文献 | Beissinger, Peled, Graphs and Combinatorics 3, 1987; Galvin, Wesley, Zacovic, JIS 25, 2022 |
| PDF | `references/galvin2022_threshold_enum.pdf` |

### [x] Trivially Perfect / Quasi-Threshold (自明完全グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | 1, 2, 8, 49, 402, ... (n=1,...,5) |
| OEIS (unlabeled) | ≈ A000081 (根付き木の数、根付き森と全単射) |
| 数え上げ | UVD 木の再帰的構築による構成的列挙 |
| 実装 | `include/trivially_perfect_enum.h` (UVD_TREE) |
| 参考文献 | Galvin, Wesley, Zacovic, JIS 25, 2022 |

### [ ] Strongly Chordal (強弦グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 列挙 | 専用アルゴリズムなし (オープン問題) |
| 備考 | chordal グラフ生成後に strong elimination ordering の存在を確認するフィルタ方式が実用的 |

### [x] Ptolemaic (プトレマイオスグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected labeled) | A287886: 1, 1, 4, 35, 481, 9042, ... |
| OEIS (all labeled) | 1, 2, 8, 61, 762, 13534, ... (n=1,...,6) |
| OEIS (connected unlabeled) | A287888: 1, 1, 2, 5, 14, 47, 170, 676, ... |
| 列挙 | 逆探索 (chordal の部分木として枝刈り列挙)。多項式遅延 (Nakano, Uno) |
| 実装 | `include/ptolemaic_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Nakano, Uno, WALCOM 2020; ISAAC 2020 / Discrete Appl. Math. 2023 |

### [x] Block (ブロックグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | A035053: 1, 1, 2, 4, 9, 22, 59, 165, 496, ... |
| OEIS (labeled) | 1, 2, 8, 55, 562, 7739, 134808, ... |
| 列挙 | Nakano-Uno フレームワーク: chordal 逆探索 + block 性枝刈り |
| 実装 | `include/block_enum.h` — ラベル付き全列挙 (reverse search) |

### [ ] Weakly Chordal (弱弦グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | A079457 |
| 列挙 | 専用アルゴリズムなし。geng + フィルタ (G と complement(G) に chordless cycle ≥ 5 がないか検査) |
| 参考文献 | Hayward, J. Combin. Theory Ser. B 39, 1985 |

### [ ] AT-Free (小惑星三つ組なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 列挙 | 専用アルゴリズムなし。geng + O(n^3) 認識でフィルタ |
| 備考 | AT-free は遺伝的でないため canonical augmentation + pruning が困難 |
| 参考文献 | Corneil, Olariu, Stewart, SIAM J. Discrete Math. 10(3), 1997 |

---

### [x] Permutation (順列グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A123448: 1, 2, 4, 11, 33, 142, 776, 5699, 50723, ... |
| 列挙 | canonical deletion (Johnston, 2020); 多項式遅延 (Yamazaki et al., TCS 2019); BDD ベース (Kawahara et al., 2024) |
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

### [x] Cograph (P4-free グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000084: 1, 2, 4, 10, 24, 66, 180, 522, 1532, ... |
| OEIS (labeled) | A006351: 1, 2, 8, 52, 472, 5504, ... |
| 数え上げ | cotree (series-parallel network) と全単射。再帰公式あり |
| 列挙 | cotree の再帰的構築により直接列挙。proximity search で極大 cograph 部分グラフも列挙可 |
| 実装 | `include/cograph_enum.h` — ラベル付き全列挙 (cotree construction) |
| 参考文献 | Seinsche, 1974 (P4-free 特性化); Conte, Kante, Kurita, Uno, Wasa, DAM 2023 (proximity search) |

### [x] Distance-Hereditary (距離遺伝的グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | A277862: 1, 1, 2, 6, 18, 73, 308, 1484, 7492, ... |
| 数え上げ | 母関数 + symbolic specification (Chauve, Fusy, Lumbroso, 2017) |
| 列挙 | 多項式遅延 (Nakano, Uno, ISAAC 2020 / DAM 2023) |
| 実装 | `include/distance_hereditary_enum.h` — ラベル付き全列挙 (reverse search) |
| PDF | `references/chauve2017_distance_hereditary_enum.pdf` |

### [ ] Circular-Arc (円弧グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未確認 |
| 列挙 | 専用アルゴリズムなし。BDD ベース列挙の拡張可能性あり |
| 備考 | interval の超クラス。フィルタ方式が実用的 |

### [ ] Trapezoid (台形グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未確認 |
| 列挙 | 専用アルゴリズムなし |
| 備考 | permutation の超クラス。フィルタ方式で geng + 認識 |

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
| 列挙 | reverse search, **O(1) amortized**/グラフ。一様ランダム生成も O(n) |
| 実装 | `include/bipartite_permutation_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Saitoh, Otachi, Yamanaka, Uehara, J. Discrete Algorithms 10, 2012 (ISAAC 2009) |

### [x] Convex Bipartite (凸二部グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 列挙 | reverse search (頂点追加 + C1P 判定); BDD ベース (Kawahara et al., 2024) |
| 実装 | `include/convex_bipartite_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 一方の部集合に consecutive-ones 性質。遺伝的クラスのため逆探索で枝刈り可能 |

### [x] Biconvex Bipartite (双凸二部グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 列挙 | reverse search (頂点追加 + 双凸判定); BDD ベース (Kawahara et al., 2024) |
| 実装 | `include/biconvex_bipartite_enum.h` — ラベル付き全列挙 (reverse search) |
| 備考 | 両方の部集合に consecutive-ones 性質。遺伝的クラスのため逆探索で枝刈り可能 |

### [x] Chain (連鎖グラフ / 差分グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A005418: 1, 2, 3, 6, 10, 20, 36, 72, 136, ... |
| 数え上げ | **閉じた公式**: a(n) = 2^(n-2) + 2^(floor(n/2)-1) (n ≥ 2) |
| 列挙 | 各部集合の近傍が包含関係で線形順序 → 直接構築可能 |
| 参考文献 | Peled, Sun, Discrete Appl. Math. 60(1-3), 1995 |

### [x] Cochain
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A005418 (chain グラフと同数) |
| 列挙 | chain グラフ列挙 + 補グラフ変換。補グラフ操作が同型類上の全単射であることを利用 |
| 実装 | `include/cochain_enum.h` — chain 列挙から補グラフ構築 |
| 備考 | chain グラフの補グラフ。非同型数は chain と一致 |

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
| 列挙 | 根付き版で O(1)/グラフ。split-decomposition tree による列挙 + ランダム生成 |
| 実装 | `include/cactus_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Bahrani, Lumbroso, arXiv:1711.10647, 2017 |
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

### [ ] Perfect (完全グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A052431: 1, 2, 4, 11, 33, 148, 906, 8887, ... |
| OEIS (connected) | A052433 |
| 列挙 | 専用アルゴリズムなし (部分クラスのみ BDD ベース)。geng + 認識でフィルタ |
| 参考文献 | Kawahara et al., WALCOM 2023 / TCS 2024 (部分クラス); Chudnovsky et al., Ann. Math. 164, 2006 (SPGT) |

### [x] Three-Leaf Power (3-leaf power)
| 項目 | 内容 |
|------|------|
| OEIS (labeled) | 1, 2, 8, 61, 642, 8254, ... (n=1,...,6) |
| 列挙 | chordal 逆探索 + 3-leaf power フィルタ (critical clique graph が森か判定) |
| 実装 | `include/three_leaf_power_enum.h` — ラベル付き全列挙 (reverse search) |
| 参考文献 | Brandstädt & Le, IPL 98, 2006 (特性化: (bull, dart, gem)-free chordal); Chauve, Fusy, Lumbroso, ANALCO 2017 (解析的数え上げ) |
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

### [ ] Biconnected (2-連結グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A002218: 0, 0, 1, 3, 10, 56, 468, 7123, 194066, ... |
| 列挙 | geng + 2-連結判定でフィルタ。geng の `-C` (連結) フラグと組み合わせ |
| 備考 | 基本的な構造的性質。切断点を持たない連結グラフ |

### [ ] Maximal Planar / Triangulation (極大平面グラフ / 三角形分割)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000109: 1, 1, 1, 2, 5, 14, 50, 233, 1249, 7595, 49566, ... |
| OEIS (rooted) | A000260 |
| 列挙 | **plantri** (Brinkmann, McKay): 毎秒 500 万グラフ以上。canonical construction path 法。償却 O(n^2)/グラフ |
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

### [ ] Eulerian (オイラーグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A002854 |
| OEIS (connected unlabeled) | A003049: 1, 0, 1, 1, 4, 8, 37, 184, 1782, 31026, ... |
| 数え上げ (labeled) | **閉じた公式**: 2^((n-1)(n-2)/2) (全頂点の次数が偶数の条件は GF(2) 上の n-1 個の独立な線形制約) |
| 列挙 | geng + 偶数次数フィルタ。非ラベル付きは Polya/Burnside 法 |
| 備考 | 全頂点の次数が偶数のグラフ。連結なら Euler 回路を持つ |
| 参考文献 | Harary, Palmer, "Graphical Enumeration," Academic Press, 1973 |

### [ ] Self-Complementary (自己補的グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A000171: 1, 0, 0, 1, 2, 0, 0, 10, 36, 0, 0, 720, 5600, ... |
| 数え上げ | **閉じた公式** (Read, 1963): Polya 型巡回指標を用いた列挙。a(n)=0 when n mod 4 ∈ {2,3} |
| 列挙 | 反同型 (antimorphism) σ の構造を利用した orderly generation。σ(G) = complement(G) となる σ を列挙 |
| 備考 | n(n-1)/4 が整数でないと存在しない (n ≡ 0,1 mod 4 のみ) |
| 参考文献 | Read, J. London Math. Soc. 38, 1963; Farrugia, Ph.D. thesis, Univ. Malta, 1999 |

---

## 正則グラフクラス

### [ ] k-Regular (k-正則グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (三角配列) | A051031: (n 頂点, 次数 k) の非同型 k-正則グラフ数 |
| OEIS (全次数合計) | A005176 |
| OEIS (4-regular) | A033301 |
| OEIS (5-regular) | A165626 |
| 列挙 | **GENREG** (Meringer, 1999): orderly generation + 高速 canonicity テスト。任意の k, n に対応 |
| 備考 | 並列化可能 (Rouyer et al., 2019: 4-正則 23 頂点まで列挙) |
| 参考文献 | Meringer, J. Graph Theory 30, 1999, pp. 137-146 |

### [ ] Cubic (三次グラフ / 3-正則グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A005638: 0, 1, 2, 6, 21, 94, 540, 4207, ... |
| OEIS (connected unlabeled) | A002851: 0, 1, 2, 5, 19, 85, 509, 4060, ... |
| OEIS (labeled) | A004109 |
| 列挙 | **snarkhunter** (Brinkmann, Goedgebeur, McKay): 三次グラフ専用生成器。canonical deletion 使用。girth 制約付きで geng の 30 倍以上高速 |
| 備考 | snark (橋なし三次グラフで 3-辺彩色不可) の列挙にも使用 |
| 参考文献 | Brinkmann, Goedgebeur, McKay, J. Graph Theory 86, 2017; Brinkmann, J. Graph Theory 23(2), 1996 |

### [ ] Strongly Regular (強正則グラフ)
| 項目 | 内容 |
|------|------|
| OEIS | A088741 (パラメータ依存、単一数列なし) |
| 数え上げ | パラメータ (n,k,λ,μ) ごとに個別に列挙。小さい n では完全分類済み |
| 列挙 | パラメータ固定の網羅的探索 + 固有値実現可能性テストで枝刈り (McKay, Spence) |
| 備考 | Spence のデータベースで 64 頂点以下の全実現可能パラメータの完全リストあり |
| 参考文献 | McKay, Spence, Australas. J. Combin. 24, 2001; Brouwer パラメータ表 |

---

## 交差グラフ・幾何的グラフクラス (追加)

### [ ] Circle (円グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A156809: 1, 2, 4, 11, 34, 154, 978, 9497, 127954, ... |
| 認識 | O(n+m) 線形時間 (Paul, Rutter, STACS 2026); O(n^2) (Spinrad, 1994) |
| 列挙 | canonical deletion 法 (Johnston, 2020): n=13 まで計算済み (22,576,188,846 個) |
| 備考 | 円の弦の交差グラフ。permutation の超クラス |
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

### [ ] Apex (頂点グラフ)
| 項目 | 内容 |
|------|------|
| OEIS (unlabeled) | A215620: 1, 2, 4, 11, 34, 155, 1026, 11666, ... |
| OEIS (nonplanar apex) | A215621 |
| 認識 | O(n(n+m)): 各頂点を除去して平面性テスト |
| 列挙 | 網羅的手法。閉じた公式なし |
| 備考 | 1 頂点除去で平面になるグラフ。マイナー閉 |

---

## 禁止部分グラフ系クラス (追加)

### [ ] Bull-Free (ブルなしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS (connected unlabeled) | A079575: 1, 1, 2, 6, 20, 93, 480, 2960, 19475, ... |
| 認識 | O(n^5) (bull は 5 頂点、全 5-部分集合を検査) |
| 列挙 | geng + フィルタのみ。専用アルゴリズムなし |
| 参考文献 | Chudnovsky, "The structure of bull-free graphs I-III," JCTB, 2012 |

### [ ] Parity (パリティグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 認識 | O(n+m) 線形時間: split decomposition で素成分が完全グラフまたは二部グラフか判定 |
| 列挙 | 専用アルゴリズムなし。split-decomposition ベースの解析的列挙の可能性あり (distance-hereditary と類似構造) |
| 備考 | 同じ 2 頂点間の任意の誘導パスが同じ偶奇性を持つグラフ |
| 参考文献 | Burlet, Uhry, Annals of Discrete Math., 1984; Bouchet, Combinatorica, 1987 |

### [ ] Meyniel (メイニエルグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 認識 | O(n^2) (Lévêque, Lin, Maffray, Trotignon, TCS 2009) |
| 列挙 | geng + フィルタ。chordal の超クラス、perfect の部分クラス |
| 備考 | 長さ 5 以上の奇閉路が全て 2 本以上の弦を持つグラフ |
| 参考文献 | Burlet, Fonlupt, Annals of Discrete Math., 1984; Lévêque et al., TCS, 2009 |

### [ ] Even-Hole-Free (偶数穴なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 認識 | O(n^9) (Lai, Lu, Thorup, STOC 2020)。歴史: O(n^40) → O(n^31) → O(n^19) → O(n^11) → O(n^9) |
| 列挙 | geng + フィルタのみ。認識の高計算量のため小さい n でのみ実用的 |
| 参考文献 | Conforti et al., JCTB, 2002; Lai, Lu, Thorup, STOC 2020 |

### [ ] Odd-Hole-Free (奇数穴なしグラフ)
| 項目 | 内容 |
|------|------|
| OEIS | 未登録 |
| 認識 | O(n^9) (Chudnovsky, Scott, Seymour, Spirkl, JACM 2020)。数十年間のオープン問題を解決 |
| 列挙 | geng + フィルタのみ。認識の高計算量のため小さい n でのみ実用的 |
| 備考 | perfect = odd-hole-free ∩ odd-antihole-free (SPGT) |
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

---

## 参考文献 PDF 一覧 (`references/`)

| ファイル名 | 内容 |
|-----------|------|
| `mckay1998_isomorph_free_generation.pdf` | McKay, Canonical augmentation 汎用フレームワーク |
| `hebert-johnson2023_counting_chordal.pdf` | Chordal グラフの多項式時間数え上げ |
| `galvin2022_threshold_enum.pdf` | Threshold, quasi-threshold グラフの列挙 |
| `chauve2017_distance_hereditary_enum.pdf` | Distance-hereditary グラフの正確な数え上げ |
| `kawahara2024_bdd_intersection_graphs.pdf` | BDD ベース交差グラフ列挙 (interval, permutation 等) |
| `kiyomi2019_chordal_bipartite_enum.pdf` | Chordal bipartite 誘導部分グラフ列挙 |
| `conte2022_proximity_search.pdf` | Proximity search 汎用フレームワーク |
| `brinkmann2007_plantri.pdf` | plantri: 平面グラフ高速生成 |
| `bodirsky2007_outerplanar_enum.pdf` | Outerplanar グラフの数え上げ・漸近解析 |
| `bahrani2017_cactus_enum.pdf` | Cactus グラフの列挙・ランダム生成 |

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
| Johnston, 2020 | 円グラフの canonical deletion 列挙 |
| Chudnovsky, Scott, Seymour, Spirkl, JACM 67(1), 2020 | 奇数穴検出 O(n^9) |
| Lai, Lu, Thorup, STOC 2020 | 偶数穴検出 O(n^9) |
| Brandstädt, Le, Sritharan, ACM Trans. Algorithms, 2008 | 4-leaf power の線形時間認識 |
| Lafond, ACM Trans. Algorithms, 2023 | 一般 k-leaf power の多項式時間認識 |

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
| 高 | **k-Tree (2-tree)** | Beineke-Pippert 公式 + 再帰的クリーク拡張 |
| 高 | **Maximal Planar** | plantri で毎秒 500 万グラフ以上 |
| ~~高~~ | ~~**4-Leaf Power**~~ | ~~線形時間認識、chordal 逆探索 + フィルタ (3-leaf power と同アプローチ)~~ |
| 中 | **Cubic (3-正則)** | snarkhunter で高速生成 |
| 中 | **k-Regular** | GENREG で任意の k に対応 |
| 中 | **Circle** | canonical deletion で n=13 まで計算済み |
| 中 | **Triangle-Free** | geng -t で効率的な枝刈り |
| 中 | **Unicyclic** | 閉路 + 根付き木の構成的列挙 |
| 中 | **5-Leaf Power** | 線形時間認識、chordal 逆探索 + フィルタ |
| 低 | **Parity** | O(n+m) 認識、split-decomposition ベースの列挙可能性 |
| 低 | **Meyniel** | O(n^2) 認識、フィルタ方式で実用的 |
| 低 | **Self-Complementary** | 閉じた公式あり (Read 1963) |

フィルタ方式 (`geng n | ./recognizer`) は全クラスに適用可能で、n ≤ 12 程度まで実用的。
