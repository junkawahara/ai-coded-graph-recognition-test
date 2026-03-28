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
| **plantri** | 平面グラフ生成 | https://users.cecs.anu.edu.au/~bdm/plantri/ |
| **GENREG** | 正則グラフ生成 | https://www.mathe2.uni-bayreuth.de/markus/reggraphs.html |
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

### [ ] Cochain
| 項目 | 内容 |
|------|------|
| 備考 | chain グラフの補グラフ。chain の列挙から構築 |

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

### [ ] Three-Leaf Power (3-leaf power)
| 項目 | 内容 |
|------|------|
| OEIS | 未確認 |
| 列挙 | 専用アルゴリズムなし。ptolemaic の部分クラスのためフィルタ方式が実用的 |

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

---

## 実装優先度の提案

効率的な専用アルゴリズムが存在するクラス (実装価値が高い順):

1. **Threshold** - バイナリ文字列からの直接構築、最も単純
2. **Chain** - 閉じた公式あり、直接構築可能
3. **Proper Interval** - reverse search, O(1)/グラフ
4. **Bipartite Permutation** - reverse search, O(1)/グラフ
5. **Series-Parallel** - O(1)/グラフ (Kawano-Nakano)
6. **Cograph** - cotree の列挙に帰着
7. **Interval** - BDD ベースまたは多項式遅延
8. **Permutation** - BDD ベースまたは canonical deletion
9. **Planar** - plantri 連携
10. **Outerplanar** - plantri 連携

フィルタ方式 (`geng n | ./recognizer`) は全クラスに適用可能で、n ≤ 12 程度まで実用的。
