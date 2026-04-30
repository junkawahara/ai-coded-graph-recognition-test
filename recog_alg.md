# 列挙専用クラスの認識アルゴリズム一覧

README で「列挙専用クラス (Enumeration-Only Classes)」として列挙されているグラフクラスについて、
認識アルゴリズムの存在・計算量・参考文献をまとめる。
`[ ]` は未実装、`[x]` は実装完了を示す。

---

## 自明な認識 (定義から直接判定可能)

### [x] Tree (木)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n + m) |
| アルゴリズム | 連結性チェック (BFS/DFS) + m = n - 1 の確認 |
| 実装 | `include/tree.h` — `check_tree()` |
| 備考 | 同値条件: 連結かつ閉路なし、連結かつ m = n - 1、任意の 2 頂点間にちょうど 1 本のパス |

### [x] Forest (森)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n + m) |
| アルゴリズム | 閉路がないことを確認 (DFS) 、または m = n - (連結成分数) を確認 |
| 実装 | `include/forest.h` — `check_forest()` |
| 備考 | 同値条件: 閉路を含まない (各連結成分が木) |

### [x] Unicyclic (一閉路グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n + m) |
| アルゴリズム | 連結性チェック + m = n の確認 |
| 実装 | `include/unicyclic.h` — `check_unicyclic()` |
| 備考 | 連結グラフで辺数が頂点数と等しい ⟺ ちょうど 1 つの閉路を持つ |

### [x] k-regular (k-正則グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n + m) |
| アルゴリズム | 全頂点の次数が同一であることを確認 |
| 実装 | `include/kregular.h` — `check_kregular()` |
| 備考 | 正則であれば次数 k を出力。必要条件: nk が偶数 |

### [x] Cubic (三正則グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n + m) |
| アルゴリズム | 全頂点の次数が 3 であることを確認 |
| 実装 | `include/cubic.h` — `check_cubic()` |
| 備考 | k = 3 の正則グラフ。必要条件: n が偶数 |

### [x] Tournament (トーナメント)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n²) |
| アルゴリズム | 有向グラフにおいて、全ての頂点ペア (u, v) に対しちょうど 1 本の有向辺が存在することを確認 |
| 実装 | `include/tournament.h` — `check_tournament()` |
| 備考 | 完全グラフ Kn の向き付け。m = n(n-1)/2 が必要条件 |

### [x] Directed graph (有向グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | — |
| アルゴリズム | 認識問題として自明 (任意の有向辺集合が有向グラフ) |
| 実装 | `include/digraph.h` — `check_digraph()` |
| 備考 | 列挙は全有向グラフの非同型生成。認識は入力形式の妥当性検証のみ |

---

## 簡単な認識 (既存性質の組み合わせ)

### [x] Caterpillar (毛虫グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n) |
| アルゴリズム | (1) 木であることを確認、(2) 全ての葉 (次数 1 の頂点) を除去、(3) 残りが道 (全頂点の次数 ≤ 2) または空であることを確認 |
| 実装 | `include/caterpillar.h` — `check_caterpillar()` |
| 参考文献 | Harary, Schwenk, "The number of caterpillars," Discrete Mathematics 6, 1973 |

### [x] Maximal planar (極大平面グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n) |
| アルゴリズム | (1) 平面性チェック (Boyer-Myrvold O(n))、(2) m = 3n - 6 (n ≥ 3) の確認 |
| 実装 | `include/maximal_planar.h` — `check_maximal_planar()` |
| 備考 | 全ての面が三角形である平面グラフ。n ≤ 2 は自明なケース |

### [x] Cubic planar (三正則平面グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n) |
| アルゴリズム | (1) 全頂点の次数が 3 であることを確認、(2) 平面性チェック (Boyer-Myrvold O(n)) |
| 実装 | `include/cubic_planar.h` — `check_cubic_planar()` |
| 備考 | 三正則 + 平面の交差。必要条件: n が偶数 |

### [x] Polyhedral (多面体グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n + m) |
| アルゴリズム | (1) 平面性チェック O(n)、(2) 3-連結性チェック |
| 実装 | `include/polyhedral.h` — `check_polyhedral()` |
| 参考文献 | Steinitz の定理: 多面体グラフ ⟺ 3-連結平面グラフ |
| 備考 | 3-連結性は Hopcroft-Tarjan の SPQR 木構築 O(n + m) で判定可能 |

### [x] Simple quadrangulation (単純四角分割)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n + m) |
| アルゴリズム | (1) m = 2n - 4 の確認、(2) 三角形フリー確認、(3) 3-連結性チェック、(4) 平面性チェック |
| 実装 | `include/simple_quadrangulation.h` — `check_simple_quadrangulation()` |
| 備考 | 3-連結平面 + m = 2n-4 + 三角形フリー ⟺ 全面が四角形。Euler の公式より平均面サイズ = 4、三角形なしなら全面 = 4 |

---

## 非自明な多項式時間認識

### [x] k-tree (k-木)
| 項目 | 内容 |
|------|------|
| 計算量 | O(nk) (固定 k に対して線形時間) |
| アルゴリズム | **剥離法 (peeling)**: 次数がちょうど k で近傍がクリークをなす頂点を反復的に除去。最終的に (k+1)-クリーク 1 つに帰着すれば k-木 |
| 代替手法 | PEO ベース: MCS で完全除去順序を計算 → 弦グラフ確認 → 全極大クリークのサイズが k+1、全極小分離集合のサイズが k であることを確認 |
| 参考文献 | Patil, "The structure of k-trees," 1986; Beineke, Pippert, "The number of labeled k-dimensional trees," J. Combin. Theory 6, 1969; Rose, "On simple characterizations of k-trees," Discrete Mathematics 7, 1974 |
| 備考 | 必要条件: m = kn - k(k+1)/2。k = 1 は木、k = 2 は 2-木 |

### [x] Halin (ハリングラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n + m) |
| アルゴリズム | (1) 3-連結性と平面性を確認、(2) 平面埋め込みで外面に接するハミルトン閉路を検出、(3) 外面閉路の辺を除去して得られるグラフが木で、次数 2 の頂点を持たず、葉が外面閉路の頂点と一致することを確認 |
| 参考文献 | Cornuéjols, Naddef, Pulleyblank, 1983 (構造的特徴付け); Fomin, Golovach, Thilikos, 2009 |
| 備考 | 必要条件: 3-連結、平面、最小次数 3 |

### [x] Fullerene (フラーレングラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n) |
| アルゴリズム | (1) 三正則であることを確認、(2) 平面性チェック + 組み合わせ埋め込みの計算、(3) 全ての面が五角形または六角形であることを確認 |
| 参考文献 | Brinkmann, Dress, 1997 (フラーレンの生成); Boyer, Myrvold, 2004 (線形時間平面性) |
| 備考 | Euler の公式より、五角形はちょうど 12 個、六角形は n/2 - 10 個。必要条件: n ≥ 20、n は偶数 (n = 22 を除く) |

### [x] Strongly regular (強正則グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n³) (行列積)、O(n^ω) (高速行列乗算) |
| アルゴリズム | (1) 正則性の確認 (次数 k を取得)、(2) 隣接行列 A の二乗 A² を計算、(3) A² = (λ - μ)A + (k - μ)I + μJ を満たすか確認。ここで λ は隣接頂点ペアの共通近傍数、μ は非隣接頂点ペアの共通近傍数 |
| 参考文献 | パラメータ (n, k, λ, μ) の判定は標準的な代数的グラフ理論 |
| 備考 | パラメータが未知でも多項式時間: 任意の辺/非辺から λ, μ を取得し全ペアで一定か確認 |

### [x] Laman (Laman グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | O(n²) |
| アルゴリズム | **ペブルゲームアルゴリズム**: 各頂点に 2 個のペブルを配置し、辺を順に挿入。各辺挿入時に端点からペブルを集められるか BFS/DFS で確認。Laman 条件: m = 2n - 3 かつ全部分集合 S に対し \|E(S)\| ≤ 2\|S\| - 3 |
| 参考文献 | Lee, Streinu, "Pebble Game Algorithms and Sparse Graphs," Discrete Mathematics 308(8), 2008; Laman, "On graphs and rigidity of plane skeletal structures," J. Engineering Mathematics 4, 1970 |
| 備考 | 2D における最小剛性グラフ。ペブルゲームは全部分集合の列挙を回避する |

### [x] Poset / Hasse diagram (半順序集合 / ハッセ図)
| 項目 | 内容 |
|------|------|
| 計算量 | O(nm) |
| アルゴリズム | (1) 有向グラフが DAG であることを確認 (トポロジカルソート O(n + m))、(2) 推移的簡約であることを確認: 各辺 (u, v) に対し、u から v へ長さ 2 以上のパスが存在しないことを検証 |
| 備考 | ハッセ図は半順序の推移的簡約。推移的簡約の検証は各辺について到達可能性チェックが必要 |

---

## 困難な問題 (多項式時間アルゴリズム未知)

### [x] Snark (スナークグラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | **NP 困難** (彩色指数の決定がボトルネック) |
| アルゴリズム | 三正則性と巡回 4-辺連結性は多項式時間で判定可能。しかし彩色指数 (3 か 4 か) の判定が NP 完全 |
| 参考文献 | Holyer, "The NP-completeness of edge-coloring," SIAM J. Comput. 10(4), 1981 (三正則グラフでも NP 完全); Isaacs, "Infinite families of nontrivial trivalent graphs which are not Tait colorable," Amer. Math. Monthly 82, 1975 |
| 備考 | Vizing の定理より三正則グラフの彩色指数は 3 または 4。3-辺彩色の証明書は多項式時間で検証可能 (NP) だが、彩色指数 4 の判定は co-NP 困難。実用的には SAT ソルバ等を利用 |

### [x] Self-complementary (自己補グラフ)
| 項目 | 内容 |
|------|------|
| 計算量 | **GI 完全** (グラフ同型問題と等価、準多項式時間 exp(O((log n)^c))) |
| アルゴリズム | グラフ G とその補グラフ G̅ の同型判定。必要条件の事前チェック: n ≡ 0 or 1 (mod 4)、m = n(n-1)/4 |
| 参考文献 | Colbourn, Colbourn, "Graph Isomorphism and Self-Complementary Graphs," SIGACT News, 1978 (GI 完全性の証明); Babai, "Graph Isomorphism in Quasipolynomial Time," STOC 2016 |
| 実用手法 | nauty/Traces (McKay, Piperno, 2014) や Bliss による同型判定 |
| 備考 | 多項式時間アルゴリズムの存在は未解決問題 |
