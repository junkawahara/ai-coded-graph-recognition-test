# ai-coded-graph-recognition-test

**75 超のグラフクラスに対する認識・列挙アルゴリズム**を提供する C++11 ヘッダオンリーライブラリです。弦グラフ、インターバルグラフ、平面グラフ、順列グラフなど多数のクラスに対応しています。認識器はグラフが特定のクラスに属するかを判定し (証明書付き)、列挙器は n 頂点上のグラフを全列挙します (多くのクラスはラベル付き、一部は非同型列挙)。

本ライブラリの**コードはすべて AI** (Claude 4.6 Opus および Codex 5.2) が生成しました。AI コーディングの限界を探る実験として開発されており、人間の役割はタスク指示とコードレビューのみで、コードは一切手書きしていません。

[English version (README.md)](README.md)

## 動機

このプロジェクトは、AI が非自明なアルゴリズムをゼロから実装できる限界を調査するものです。グラフクラス認識を題材に選んだ理由は、以下の能力が要求されるためです:

- 構造的グラフ理論の形式的定義・定理の理解
- 既知アルゴリズム (MCS, PEO, BFS, DFS, バックトラッキング等) の正確な実装
- 微妙なエッジケース (空グラフ、非連結グラフ、補グラフの閉包性) の処理
- 正しさを検証するためのテストインフラの設計・実行

実験は進行中です。知見がまとまり次第ドキュメント化します。

## ドキュメント

API ドキュメント: **https://junkawahara.github.io/ai-coded-graph-recognition-test/ja**

## 特徴

- **ヘッダオンリー**: `#include` するだけで使用可能。リンク不要
- **C++11 互換**: 標準的なコンパイラで動作
- **75 超のグラフクラス** に対して認識・列挙またはその両方を提供
- **76 種の認識器**: 複数のアルゴリズムバリアント (YES/NO + 証明書)
- **107 種の列挙器**: 指定された頂点数 n のグラフを全列挙 (多くはラベル付き。tree/forest/caterpillar/halin/fullerene/chain/cochain/threshold/unicyclic/simple quadrangulation/proper interval/trivially perfect/cograph/cluster/triangle-free/bipartite/permutation/circle/eulerian/biconnected/chordal/co-chordal/cubic/k-regular/snark/Laman/split/planar/self-complementary/distance-hereditary/Ptolemaic/3-leaf power/interval/co-interval/outerplanar/series-parallel/cactus/bipartite permutation/maximal planar/polyhedral/cubic planar/tournament/digraph/poset (ラベルなし) などは非同型列挙)
- **CLI ツール**: 全認識器・列挙器にコマンドラインインターフェースを提供
- **グラフ分解を第一級の部品として提供**: modular decomposition、split decomposition (Cunningham)、SPQR 木、cotree、クリーク木と木分解、ブロックカット木、PQ-tree、推移的向き付け、平面埋め込み、および認識器が内部で用いる各種消去順序・レイアウト
- **テストインフラ**: 静的テストケース、ランダム差分テスト (property テスト)、アルゴリズム間の差分テスト
- **Python バインディング**: pybind11 による NetworkX 連携対応

## 対応グラフクラス

**認識の計算量** 列は既定で実行される*認識*アルゴリズムの時間計算量です
(n = 頂点数、m = 辺数、Δ = 最大次数)。列挙のコストは表していません。
*指数時間* は最悪ケースが指数時間であることを示します。詳細と実用上の
規模の目安は [性能に関する注意](#性能に関する注意) を参照してください。

**引用** 列は各実装が従っている特徴づけまたはアルゴリズムを示します
(`認識:` が認識、`列挙:` が列挙)。実装がクラスの定義をそのままなぞって
いる場合は、その特徴づけを与えた論文 (または当該問題の標準的な文献) を
挙げています。`—` は該当する文献がないことを意味し、構成が folklore で
あるか、そのクラスに列挙器がないかのいずれかです。書誌情報は
[参考文献](#参考文献) にまとめてあります。

### 弦グラフ系

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| 弦グラフ (Chordal) | `chordal.h` | O(n+m) | 認識: [Fulkerson+ 65], [Rose+ 76], [Tarjan+ 84]<br>列挙: [Kiyomi+ 06], [McKay 98] | 長さ 4 以上の誘導閉路を持たない |
| 強弦グラフ (Strongly chordal) | `strongly_chordal.h` | O(nmΔ) | 認識: [Farber 83], [Dahlhaus+ 87]<br>列挙: [Avis+ 96] | 弦グラフ + 長さ 6 以上の偶閉路が奇弦を持つ |
| 固有弦グラフ (Proper chordal) | `proper_chordal.h` | *指数時間* | 認識: [Paul+ 24]<br>列挙: [Avis+ 96] | 弦グラフ + indifference tree-layout を許容 |
| スプリットグラフ (Split) | `split.h` | O(n) | 認識: [Földes+ 77], [Hammer+ 81]<br>列挙: [Cheng+ 16], [Troyka 19], [McKay 98] | 頂点集合をクリークと独立集合に分割可能 |
| 閾値グラフ (Threshold) | `threshold.h` | O(n) | 認識: [Chvátal+ 77], [Mahadev+ 95]<br>列挙: [Chvátal+ 77] | 孤立頂点または全域頂点の反復除去で空にできる |
| 弱弦グラフ (Weakly chordal) | `weakly_chordal.h` | O(n⁶) | 認識: [Hayward 85], [Spinrad+ 95]<br>列挙: [Kiyomi thesis 06] | G と補グラフのいずれにも長さ 5 以上の誘導閉路がない |
| ブロックグラフ (Block) | `block.h` | O(n+m) | 認識: [Harary 63], [Tarjan 72]<br>列挙: [Avis+ 96] | 全ての二重連結成分がクリーク |
| プトレマイオスグラフ (Ptolemaic) | `ptolemaic.h` | O(n³ log n) | 認識: [Howorka 81]<br>列挙: [Avis+ 96], [Bandelt+ 86] | 弦グラフ + 距離遺伝グラフ |
| 自明完全グラフ (Trivially perfect) | `trivially_perfect.h` | O(n(n+m)) | 認識: [Wolk 62], [Golumbic 78]<br>列挙: [Golumbic 78], [Beyer+ 80] | 弦グラフ + コグラフ (= 準閾値グラフ) |
| 準閾値グラフ (Quasi-threshold) | `quasi_threshold.h` | O(n(n+m)) | 認識: [Wolk 62], [Yan+ 96]<br>列挙: — | 自明完全グラフの別名 (薄いラッパー) |
| k-木 (k-tree) | `ktree.h` | O(n² + nk²) | 認識: [Rose 74]<br>列挙: [Beineke+ 69], [Avis+ 96] | K_{k+1} から k-クリークへの頂点追加で構成されるグラフ |

### インターバルグラフ / 円弧グラフ系

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| インターバルグラフ (Interval) | `interval.h` | O(n³) | 認識: [Lekkerkerker+ 62]<br>列挙: [Kiyomi-Kijima+ 06]<br>列挙 (ラベルなし): [McKay 98] | 実数直線上の区間の交差グラフ |
| 固有インターバルグラフ (Proper interval) | `proper_interval.h` | O(n³ + nΔ³) | 認識: [Roberts 69]<br>列挙: [Avis+ 96], [Saitoh+ 10] | 区間間に包含関係がないインターバルグラフ |
| 単位インターバルグラフ (Unit interval) | `unit_interval.h` | O(n³ + nΔ³) | 認識: [Roberts 69]<br>列挙: — | 等長区間のインターバルグラフ (= 固有インターバルグラフ) |
| 余インターバルグラフ (Co-interval) | `co_interval.h` | O(n³) | 認識: [Lekkerkerker+ 62]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | 補グラフがインターバルグラフ |
| 円弧グラフ (Circular-arc) | `circular_arc.h` | *指数時間* | 認識: [Tucker 80], [McConnell 03]<br>列挙: [Avis+ 96] | 円周上の弧の交差グラフ |
| 固有円弧グラフ (Proper circular-arc) | `proper_circular_arc.h` | *指数時間* | 認識: [Tucker 74]<br>列挙: [Avis+ 96] | 弧間に包含関係がない円弧グラフ |

### 順列グラフ / 比較可能性グラフ系

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| 順列グラフ (Permutation) | `permutation.h` | O(n³) | 認識: [Pnueli+ 71], [Gallai 67]<br>列挙: [Avis+ 96], [McKay 98] | G と補グラフの両方が比較可能性グラフ |
| 比較可能性グラフ (Comparability) | `comparability.h` | O(nm) | 認識: [Gallai 67], [Golumbic 80]<br>列挙: [Avis+ 96] | 辺に推移的向き付けが可能 |
| 余比較可能性グラフ (Co-comparability) | `co_comparability.h` | O(n³) | 認識: [Gallai 67]<br>列挙: [Avis+ 96] | 補グラフが比較可能性グラフ |
| 二部順列グラフ (Bipartite permutation) | `bipartite_permutation.h` | O(n³) | 認識: [Spinrad+ 87]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | 二部グラフ + 順列グラフ |
| 台形グラフ (Trapezoid) | `trapezoid.h` | O(n⁴) | 認識: [Dagan+ 88], [Cogis 82]<br>列挙: [Avis+ 96] | 二平行線間の台形の交差グラフ (順列グラフの一般化) |

### 二部グラフ系

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| 二部グラフ (Bipartite) | `bipartite.h` | O(n+m) | 認識: [König 36]<br>列挙: [Avis+ 96], [McKay 98] | 2 彩色可能 (奇閉路なし) |
| 弦二部グラフ (Chordal bipartite) | `chordal_bipartite.h` | O(m²Δ²) | 認識: [Golumbic+ 78]<br>列挙: [Avis+ 96] | 二部グラフ + 長さ 6 以上の誘導閉路なし |
| チェーングラフ (Chain) | `chain.h` | O(n+m) | 認識: [Yannakakis 82]<br>列挙: — | 二部グラフ + 近傍が包含順序で全順序 |
| 余チェーングラフ (Co-chain) | `cochain.h` | O(n²) | 認識: [Yannakakis 82]<br>列挙: — | 補グラフがチェーングラフ |
| 凸二部グラフ (Convex bipartite) | `convex_bipartite.h` | O(nm) | 認識: [Booth+ 76]<br>列挙: [Avis+ 96] | 二部グラフ + 片側が連続近傍性質を持つ |
| 双凸二部グラフ (Biconvex bipartite) | `biconvex_bipartite.h` | O(nm) | 認識: [Abbas+ 00], [Booth+ 76]<br>列挙: [Avis+ 96] | 二部グラフ + 両側が連続近傍性質を持つ |

### 平面グラフ系

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| 平面グラフ (Planar) | `planar.h` | O(n+m) | 認識: [Kuratowski 30], [de Fraysseix+ 06], [Brandes 09]<br>列挙: [Avis+ 96], [McKay 98] | K5 および K3,3 マイナーを持たない |
| 外平面グラフ (Outerplanar) | `outer_planar.h` | O(n+m) | 認識: [Chartrand+ 67], [de Fraysseix+ 06]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | K4 および K2,3 マイナーを持たない |
| カクタスグラフ (Cactus) | `cactus.h` | O(n+m) | 認識: [Harary+ 53], [Tarjan 72]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | 各二重連結成分が辺 1 本または単純閉路 |
| 直並列グラフ (Series-parallel) | `series_parallel.h` | O(n+m) | 認識: [Duffin 65], [Valdes+ 82]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | K4 マイナーを持たない (2-退化) |
| 頂点グラフ (Apex) | `apex.h` | O(n(n+m)) | 認識: [Robertson+ 95], [de Fraysseix+ 06]<br>列挙: [Avis+ 96] | 1 頂点の除去で平面グラフになる |
| 極大平面 (Maximal planar) | `maximal_planar.h` | O(n+m) | 認識: [Kuratowski 30], [de Fraysseix+ 06]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | 全面が三角形の平面グラフ |
| 三正則平面 (Cubic planar) | `cubic_planar.h` | O(n+m) | 認識: [de Fraysseix+ 06]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | 3-正則平面グラフ |
| 多面体 (Polyhedral) | `polyhedral.h` | O(n²(n+m)) | 認識: [Steinitz 22], [de Fraysseix+ 06]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | 3-連結平面グラフ (Steinitz の定理) |
| 単純四角分割 (Simple quadrangulation) | `simple_quadrangulation.h` | O(n³) | 認識: [Steinitz 22], [de Fraysseix+ 06]<br>列挙: [Avis+ 96] | 全面が四角形の 3-連結平面グラフ |
| ハリングラフ (Halin) | `halin.h` | O(n⁴) | 認識: [Halin 71]<br>列挙: [Halin 71], [Wright+ 86] | 木 + 外周閉路からなる平面グラフ |
| フラーレン (Fullerene) | `fullerene.h` | O(n⁴) | 認識: [de Fraysseix+ 06]<br>列挙: [Avis+ 96] | 五角形と六角形の面を持つ 3-正則平面グラフ |

### 完全グラフ / 構造クラス

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| 完全グラフ (Perfect) | `perfect.h` | *指数時間* | 認識: [Chudnovsky+ 06]<br>列挙: [Avis+ 96] | 奇穴も奇反穴も持たない (SPGT) |
| コグラフ (Cograph) | `cograph.h` | O(n(n+m)) | 認識: [Seinsche 74], [Corneil+ 81], [Corneil+ 85]<br>列挙: [Corneil+ 81], [Jones+ 18] | 誘導部分グラフとして P4 を含まない |
| 距離遺伝グラフ (Distance-hereditary) | `distance_hereditary.h` | O(n³) | 認識: [Howorka 77], [Bandelt+ 86]<br>列挙: [Avis+ 96], [Bandelt+ 86] | 全連結誘導部分グラフで頂点間距離が保存される |
| AT-free グラフ | `at_free.h` | O(n³) | 認識: [Lekkerkerker+ 62], [Corneil+ 97]<br>列挙: [Avis+ 96] | 小惑星三つ組 (asteroidal triple) を持たない |
| 余弦グラフ (Co-chordal) | `co_chordal.h` | O(n²) | 認識: [Fulkerson+ 65], [Tarjan+ 84]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | 補グラフが弦グラフ |
| 線グラフ (Line graph) | `line_graph.h` | *指数時間* | 認識: [Whitney 32], [Krausz 43]<br>列挙: [Avis+ 96] | 別のグラフの辺交差グラフ |
| 円グラフ (Circle) | `circle.h` | 多項式時間 | 認識: [Naji 85], [Gasse 97], [Geelen+ 20]<br>列挙: [Avis+ 96], [McKay 98] | 円の弦の交差グラフ |
| Meyniel グラフ | `meyniel.h` | *指数時間* | 認識: [Meyniel 76], [Burlet+ 84a]<br>列挙: [Avis+ 96] | 長さ 5 以上の奇閉路が少なくとも 2 本の弦を持つ |
| パリティグラフ (Parity) | `parity.h` | *指数時間* | 認識: [Burlet+ 84b]<br>列挙: [Avis+ 96] | 同じ端点間の誘導パスが全て同じパリティ |
| 偶穴フリー (Even-hole-free) | `even_hole_free.h` | *指数時間* | 認識: [Conforti+ 02]<br>列挙: [Avis+ 96] | 長さ 4 以上の偶数誘導閉路を持たない |
| 奇穴フリー (Odd-hole-free) | `odd_hole_free.h` | *指数時間* | 認識: [Chudnovsky+ 20]<br>列挙: [Avis+ 96] | 長さ 5 以上の奇数誘導閉路を持たない |
| クラスターグラフ (Cluster) | `cluster.h` | O(n+m) | 認識: [Brandstädt+ 99]<br>列挙: — | 完全グラフの非連結和 |
| 自己補グラフ (Self-complementary) | `self_complementary.h` | *指数時間* | 認識: [Sachs 62], [Ringel 63]<br>列挙: [Sachs 62], [Ringel 63] | 補グラフと同型 |

### 禁止誘導部分グラフクラス

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| クローフリー (Claw-free) | `claw_free.h` | O(nΔ³) | 認識: [Faudree+ 97]<br>列挙: [Avis+ 96] | 誘導 K1,3 を含まない |
| ダイヤモンドフリー (Diamond-free) | `diamond_free.h` | O(m²) | 認識: [Brandstädt+ 99]<br>列挙: [Avis+ 96] | 誘導 K4 - e を含まない |
| 三角形フリー (Triangle-free) | `triangle_free.h` | O(mΔ) | 認識: [Itai+ 78]<br>列挙: [Avis+ 96], [McKay 98] | K3 を含まない |
| ブルフリー (Bull-free) | `bull_free.h` | O(mΔ²) | 認識: [Chudnovsky 12]<br>列挙: [Avis+ 96] | 誘導ブルグラフを含まない |
| P5-free | `p5_free.h` | O(nΔ⁴) | 認識: [Brandstädt+ 99]<br>列挙: [Avis+ 96] | 誘導 P5 (長さ 5 の道) を含まない |
| ジェムフリー (Gem-free) | `gem_free.h` | O(nΔ⁴) | 認識: [Brandstädt+ 99]<br>列挙: [Avis+ 96] | 誘導ジェム (fan) グラフを含まない |

### 葉べき乗グラフ (Leaf Power)

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| 3-leaf power | `three_leaf_power.h` | O(n + m log n) | 認識: [Nishimura+ 02], [Brandstädt+ 06], [Dom+ 06]<br>列挙: [Avis+ 96], [Bandelt+ 86] | 距離閾値 3 の葉べき乗グラフ |
| 4-leaf power | `four_leaf_power.h` | *指数時間* | 認識: [Brandstädt+ 08]<br>列挙: [Avis+ 96] | 距離閾値 4 の葉べき乗グラフ |
| 5-leaf power | `five_leaf_power.h` | *指数時間* | 認識: [Chang+ 07], [Lafond 22]<br>列挙: [Avis+ 96] | 距離閾値 5 の葉べき乗グラフ |

### 木 / 森系

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| 木 (Tree) | `tree.h` | O(n+m) | 認識: —<br>列挙: [Beyer+ 80], [Wright+ 86] | 連結な閉路を含まないグラフ |
| 森 (Forest) | `forest.h` | O(n+m) | 認識: —<br>列挙: [Wright+ 86] | 閉路を含まないグラフ (木の非連結和) |
| 毛虫グラフ (Caterpillar) | `caterpillar.h` | O(n+m) | 認識: [Harary+ 73]<br>列挙: [Harary+ 73] | 全頂点がパスから距離 1 以内の木 |
| 一閉路グラフ (Unicyclic) | `unicyclic.h` | O(n+m) | 認識: —<br>列挙: [Beyer+ 80] | ちょうど 1 つの閉路を持つ連結グラフ |

### 連結性 / 正則性

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| 二重連結 (Biconnected) | `biconnected.h` | O(n+m) | 認識: [Tarjan 72]<br>列挙: [Avis+ 96], [McKay 98] | 2-連結 (頂点数 3 以上、カット頂点なし) |
| 三重連結 (Triconnected) | `triconnected.h` | O(n²(n+m)) | 認識: [Hopcroft+ 73]<br>列挙: — | 3-連結 |
| オイラーグラフ (Eulerian) | `eulerian.h` | O(n) | 認識: [Euler 1741]<br>列挙: [McKay 98] | 全頂点の次数が偶数 |
| k-正則 (k-regular) | `kregular.h` | O(n) | 認識: —<br>列挙: [Meringer 99]<br>列挙 (ラベルなし): [Meringer 99], [McKay 98] | 全頂点の次数が k |
| 三正則 (Cubic) | `cubic.h` | O(n) | 認識: [Petersen 1891]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | 3-正則グラフ |
| 強正則 (Strongly regular) | `strongly_regular.h` | O(n²Δ) | 認識: [Bose 63]<br>列挙: [Bose 63] | 正則で隣接数が一様 |
| スナーク (Snark) | `snark.h` | *指数時間* | 認識: [Isaacs 75]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98], [Brinkmann+ 13] | 内周 5 以上・巡回 4-辺連結な三正則グラフで彩色指数 4 |
| Laman グラフ | `laman.h` | O(n²) | 認識: [Laman 70], [Jacobs+ 97]<br>列挙: [Avis+ 96]<br>列挙 (ラベルなし): [McKay 98] | 2D で最小剛性を持つグラフ |

### 有向グラフクラス

| グラフクラス | ヘッダ | 認識の計算量 | 引用 | 説明 |
|---|---|---|---|---|
| トーナメント (Tournament) | `tournament.h` | O(n²) | 認識: [Moon 68]<br>列挙: —<br>列挙 (ラベルなし): [McKay 98] | 完全有向グラフ (Kn の向き付け) |
| 有向グラフ (Directed graph) | `digraph.h` | O(m log m) | 認識: —<br>列挙: —<br>列挙 (ラベルなし): [McKay 98] | 全単純有向グラフ |
| 半順序集合 (Poset) | `poset.h` | O(m(n+m)) | 認識: —<br>列挙: [Brinkmann+ 02]<br>列挙 (ラベルなし): [Brinkmann+ 02], [McKay 98] | ハッセ図 (半順序集合) |

## ビルド

```bash
make          # 全認識器・列挙器を bin/ 以下にビルド
make clean    # バイナリを削除
```

C++11 対応の g++ が必要です。`CXX`, `CXXFLAGS` でカスタマイズ可能。

## 使い方

### 認識

各認識器は標準入力からグラフを読み込み、結果を出力します:

```bash
echo "4 4
1 2
2 3
3 4
4 1" | ./bin/chordal
# 出力: NO
```

**入力形式:**
```
n m
u1 v1
u2 v2
...
```
`n` = 頂点数, `m` = 辺数。頂点は 1-indexed。

**出力形式:** `YES` または `NO`。グラフクラス固有の付加情報が続く場合があります (例: インターバルモデル)。

### 列挙

各列挙器は指定された頂点数 n のグラフを全列挙します (多くのクラスはラベル付き):

```bash
echo "4" | ./bin/chordal_labeled_enum
# 出力: 1行目にグラフ数、続いて辺リスト
```

インターバルグラフ列挙器は既定で Kiyomi--Kijima--Uno の専用逆探索を
使用します。K_n から辺を 1 本ずつ削除し、一意な辺追加親が現在のグラフに
戻る子だけを辿ります。従来の弦グラフ探索木 + interval フィルタは
`IntervalLabeledEnumAlgorithm::LEGACY_CHORDAL_FILTER` で選択できます。
集計や逐次書き出しには、全グラフを保持しない
`enumerate_interval_labeled_graphs_reverse_search_cb` も利用できます。

弱弦グラフ列挙器は既定で Kiyomi のクラス専用辺追加逆探索を使用します。
K_n の空の全域部分グラフから始め、追加した辺が、削除後も弱弦性を保つ辺の
うち最も若い辺である子だけを辿ります。従来の遺伝的クラス用最大ラベル
頂点追加探索は
`WeaklyChordalLabeledEnumAlgorithm::GENERIC_VERTEX_AUGMENTATION` で選択でき、
逐次出力には `enumerate_weakly_chordal_labeled_graphs_reverse_search_cb` を利用できます。

### 部分グラフ列挙

部分グラフ列挙器は頂点数ではなく**ホストグラフ**を受け取り、そこに含まれる
そのクラスの部分グラフを全列挙します:

```bash
# C4: 自身以外のすべての辺部分集合が弦グラフなので 15 個
printf '4 4\n1 2\n2 3\n3 4\n4 1\n' | ./bin/chordal_subgraph_enum
# 出力: 1行目にグラフ数、続いて辺リスト
```

これが Kiyomi--Uno の原論文が扱っている問題そのもので、`chordal_labeled_enum` は
その `G = K_n` の特殊ケースにあたります。アルゴリズムは同じ逆探索で、子の
生成をホストの隣接関係で絞り込むだけです。親の定義が辺の削除しか行わないため、
固定したホストの部分グラフ族は親操作について閉じており、これで正しく列挙できます。

出力は全域部分グラフです。頂点集合は固定され孤立頂点も保持されるため、ホストの
辺部分集合のうち弦グラフになるものと 1 対 1 に対応し、空の辺集合も常に含まれます。
個数は頂点数ではなく辺数に支配され (森の部分グラフはすべて弦グラフなので、辺数
`m` のホストは最大 `2^m` 個)、逐次出力には
`enumerate_chordal_subgraphs_cb` を利用できます。

```cpp
#include "chordal_subgraph_enum.h"

graph_recognition::Graph host(4, {{1,2}, {2,3}, {3,4}, {4,1}});
auto result = graph_recognition::enumerate_chordal_subgraphs(host);
// result.graphs.size() == 15
```

### 誘導部分グラフ列挙

誘導部分グラフ列挙器も**ホストグラフ**を受け取りますが、解は `G[X]` がその
クラスに属する頂点部分集合 `X` です:

```bash
# C6: 全体以外のすべての頂点部分集合が弦二部なので 63 個
printf '6 6\n1 2\n2 3\n3 4\n4 5\n5 6\n6 1\n' \
  | ./bin/chordal_bipartite_induced_subgraph_enum
# 出力: 1行目に個数、続いて頂点リスト
```

アルゴリズムは Kurita--Wasa--Arimura--Uno の ECB 逆探索で、彼らの特徴づけに
基づきます。すなわち、グラフが弦二部であることは、*weak-simplicial* 頂点
(近傍が独立集合であり、かつ包含関係で全順序をなす頂点) を繰り返し取り除いて
空にできることと同値です。

誘導部分グラフは頂点集合で定まるため、出力は辺リストではなくソート済みの
頂点リストの族であり、空集合が常に先頭に来ます。個数は辺数ではなく頂点数に
支配されます (このクラスは遺伝的なので、ホスト自身が弦二部ならばどれだけ疎でも
`2^n` に達します)。逐次出力には
`enumerate_chordal_bipartite_induced_subgraphs_cb` を利用できます。

```cpp
#include "chordal_bipartite_induced_subgraph_enum.h"

graph_recognition::Graph host(6, {{1,2}, {2,3}, {3,4}, {4,5}, {5,6}, {6,1}});
auto result =
    graph_recognition::enumerate_chordal_bipartite_induced_subgraphs(host);
// result.vertex_sets.size() == 63
```

### ライブラリとしての使用

```cpp
#include "interval.h"

graph_recognition::Graph g(4, {{1,2}, {2,3}, {3,4}, {4,1}});
auto result = graph_recognition::check_interval(g);
if (result.is_interval) {
    // result.intervals[v] = {L, R}: 頂点 v の区間
}
```

多くの認識器はアルゴリズム選択をサポートしています:

```cpp
// デフォルトの AT-free ではなくバックトラッキングを使用
auto result = graph_recognition::check_interval(g,
    graph_recognition::IntervalAlgorithm::BACKTRACKING);
```

### 分解 (Decompositions)

認識器が内部で計算している構造そのものを、YES/NO とは別に取り出せる。
一覧は API ドキュメントのユーティリティの節を参照。

```cpp
#include "modular_decomposition.h"
#include "split_decomposition.h"

graph_recognition::Graph g(4, {{1,2}, {2,3}, {3,4}});

// P4 は非自明なモジュールを持たない: 葉 4 枚の上に PRIME ノード 1 つ
auto tree = graph_recognition::modular_decomposition(g);

// 距離遺伝的 ⟺ split decomposition のどのバッグも prime でない
auto sd = graph_recognition::split_decomposition(g);
if (sd.totally_decomposable) { /* ... */ }
```

認識の過程で構造を作る認識器は、それを返すようになった。
`SplitResult::side` は (K, S) 分割、`ThresholdResult::creation_order` は生成列、
`LineGraphResult::root_graph` は線グラフが入力になる根グラフ、など。
構造の構築が認識より高コストな場合は、認識のコストを据え置くために
別のビルダー関数にしてある (`build_cotree`, `build_pruning_sequence`,
`build_clique_tree`)。

### Python ラッパー

pybind11 バインディングによる Python パッケージを提供しています:

```bash
pip install "graph-recognition @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"

# NetworkX サポート付き
pip install "graph-recognition[networkx] @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"
```

```python
from graph_recognition import is_interval, is_chordal

# 辺リスト (1-indexed)
is_interval(4, [(1, 2), (2, 3), (3, 4)])  # True
is_chordal(4, [(1, 2), (2, 3), (3, 4), (4, 1)])  # False

# アルゴリズム選択
is_interval(4, [(1, 2), (2, 3), (3, 4)], algorithm="backtracking")

# NetworkX 連携 (任意のノード型をサポート)
import networkx as nx
is_interval(nx.path_graph(5))  # True
```

全 76 認識器クラスが `is_<type>()` および `recognize_<type>()` 関数として、36 クラスが `enumerate_<type>_graphs()` 関数として利用可能です。詳細は [python/README_ja.md](python/README_ja.md) を参照してください。

## 性能に関する注意

ほとんどの認識器は低次の多項式時間で動作し、数千頂点を扱えます。
例外として把握しておくべきものは以下の通りです。

- **Perfect** (`perfect.h`): odd antihole 判定が補グラフを明示的に構築するため、
  疎な入力でも Θ(n²) 辺のグラフに対する odd hole 探索が走ります。さらに誘導
  偶パス探索の DFS は最悪ケースで指数時間です。実測: 疎な弦グラフ n=400 で
  約 1.7 秒、K(200,200) で約 27 秒、K(400,400) で 60 秒超。実用上限は数百頂点
  です。`odd_hole_free.h` は補グラフを作らないため、疎グラフでは大幅に高速です。
- **Odd-hole-free / Even-hole-free** (`odd_hole_free.h`, `even_hole_free.h`):
  DFS ベースの誘導パス探索で、最悪ケースは指数時間ですが典型的な入力では
  高速です。文献にある多項式時間アルゴリズム
  (Chudnovsky–Scott–Seymour–Spirkl) は実装されていません。
- **Circle** (`circle.h`): 既定の Naji 線形システム法は多項式時間です
  (密グラフ n=300 で約 1 秒)。オプションの `DOW_BACKTRACKING` 証明書付き
  アルゴリズムは指数時間で、実用上限は n ≈ 9 です。
- **4-leaf / 5-leaf power** (`four_leaf_power.h`, `five_leaf_power.h`):
  前段の strongly chordal 判定は多項式時間ですが、その後の (Steiner) root
  実現探索がセンター/木の割り当てをバックトラッキングするため、最悪ケース
  は指数時間です。
- **Self-complementary** (`self_complementary.h`): 補グラフとの同型判定を
  次数による枝刈り付きバックトラッキングで行うため、最悪ケース (正則な
  自己補グラフなど) は指数時間です。
- **Snark** (`snark.h`): 彩色指数の判定が 3-辺彩色のバックトラッキング探索
  のため最悪ケースは指数時間です (内周と巡回的辺連結度の判定は多項式時間)。
- **Proper chordal** (`proper_chordal.h`): nested convexity 判定がブロック内
  の頂点順序を総当たりで列挙するため、文献のアルゴリズムは O(n⁴) ですが
  実装の最悪ケースは指数時間です。
- **Circular-arc / proper circular-arc** (`circular_arc.h`,
  `proper_circular_arc.h`): いずれもバックトラッキング (円環クリーク順序 /
  弧端点順序) に依存し、最悪ケースは指数時間です。典型的な入力では多項式
  時間の経路で処理されます。
- **Line graph** (`line_graph.h`): O(mΔ) のフィルタでほとんどの入力を判定
  できますが、曖昧なケースでは Krausz 分割のバックトラッキング探索へ
  フォールバックし、最悪ケースは指数時間です。
- **Meyniel / parity** (`meyniel.h`, `parity.h`): いずれも辺または頂点対
  ごとに (誘導) パスを列挙するため最悪ケースは指数時間ですが、典型的な
  入力では高速です。

## テスト

```bash
# gtest スイートを既定フィルタ付きで実行 (ビルド済みなら約 6 秒)
make test

# 部分実行
./gtest_all --gtest_filter='Interval*'

# ランダム Property テストも実行 (数分かかる列挙ケースのみ除外)
make test-quick
```

認識器と列挙器のテストは `tests/gtest/recognizers/` および `tests/gtest/enumerators/` に置かれ、`tests/<type>/` の静的ケースを引数化しています。ランダム差分テストは `tests/gtest/property/` です。旧来の Python/Bash テストインフラ (`tests/legacy/`) は削除済みで、git タグ `legacy-tests` から参照できます。

## プロジェクト構成

```
include/          ヘッダオンリーライブラリ (全アルゴリズム)
  graph.h           グラフ表現 (1-indexed, 隣接リスト + 隣接セット)
  chordal.h         弦グラフ認識
  interval.h        インターバルグラフ認識
  ...               (150 以上のヘッダ)
src/              CLI エントリポイント
python/           Python ラッパー (pybind11)
  src/              パッケージソース (graph_recognition)
  tests/            pytest テストスイート
tests/            テストインフラ
  <type>/           静的テストケース (.in / .exp)
  gtest/            gtest スイート (recognizers/, enumerators/, property/, helpers/)
docs/             Sphinx + Doxygen ドキュメント
```

## 使用した AI モデル

- **Claude 4.6 Opus** (Anthropic) — Claude Code CLI 経由
- **Codex 5.2** (OpenAI)

全コードはこれらのモデルが生成しました。人間はタスクの指示、出力のレビュー、バグ修正の指示を行いましたが、コードは一行も書いていません。

## 参考文献

[対応グラフクラス](#対応グラフクラス) の表で使っている引用キーの一覧です。
`認識:` は認識器が従っている特徴づけ・アルゴリズム、`列挙:` は列挙器に
ついて同じものを指します。列挙器の多くは [Avis+ 96] の意味での逆探索
(reverse search) であり、対応する認識器を所属判定のフィルタとして使って
います。クラスごとの詳しい説明は
[クラス別ドキュメント](https://junkawahara.github.io/ai-coded-graph-recognition-test/ja)
を参照してください。

- **[Abbas+ 00]** N. Abbas, L. K. Stewart. "Biconvex graphs: ordering and algorithms." *Discrete Applied Mathematics*, 103(1–3):1–19, 2000. [DOI:10.1016/S0166-218X(99)00217-6](https://doi.org/10.1016/S0166-218X(99)00217-6)
- **[Avis+ 96]** D. Avis, K. Fukuda. "Reverse search for enumeration." *Discrete Applied Mathematics*, 65(1–3):21–46, 1996. [DOI:10.1016/0166-218X(95)00026-N](https://doi.org/10.1016/0166-218X(95)00026-N)
- **[Bandelt+ 86]** H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs." *Journal of Combinatorial Theory, Series B*, 41(2):182–208, 1986. [DOI:10.1016/0095-8956(86)90043-2](https://doi.org/10.1016/0095-8956(86)90043-2)
- **[Beineke+ 69]** L. W. Beineke, R. E. Pippert. "The number of labeled k-dimensional trees." *Journal of Combinatorial Theory*, 6(2):200–205, 1969. [DOI:10.1016/S0021-9800(69)80120-1](https://doi.org/10.1016/S0021-9800(69)80120-1)
- **[Beyer+ 80]** T. Beyer, S. M. Hedetniemi. "Constant time generation of rooted trees." *SIAM Journal on Computing*, 9(4):706–712, 1980. [DOI:10.1137/0209055](https://doi.org/10.1137/0209055)
- **[Booth+ 76]** K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms." *Journal of Computer and System Sciences*, 13(3):335–379, 1976. [DOI:10.1016/S0022-0000(76)80045-1](https://doi.org/10.1016/S0022-0000(76)80045-1)
- **[Bose 63]** R. C. Bose. "Strongly regular graphs, partial geometries and partially balanced designs." *Pacific Journal of Mathematics*, 13(2):389–419, 1963. [DOI:10.2140/pjm.1963.13.389](https://doi.org/10.2140/pjm.1963.13.389)
- **[Brandes 09]** U. Brandes. "The left-right planarity test." Manuscript, University of Konstanz, 2009.
- **[Brandstädt+ 99]** A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.* SIAM Monographs on Discrete Mathematics and Applications, 1999. [DOI:10.1137/1.9780898719796](https://doi.org/10.1137/1.9780898719796)
- **[Brandstädt+ 06]** A. Brandstädt, V. B. Le. "Structure and linear time recognition of 3-leaf powers." *Information Processing Letters*, 98(4):133–138, 2006. [DOI:10.1016/j.ipl.2006.01.004](https://doi.org/10.1016/j.ipl.2006.01.004)
- **[Brandstädt+ 08]** A. Brandstädt, V. B. Le, R. Sritharan. "Structure and linear-time recognition of 4-leaf powers." *ACM Transactions on Algorithms*, 5(1):11:1–11:22, 2008. [DOI:10.1145/1435375.1435386](https://doi.org/10.1145/1435375.1435386)
- **[Brinkmann+ 02]** G. Brinkmann, B. D. McKay. "Posets on up to 16 points." *Order*, 19(2):147–179, 2002. [DOI:10.1023/A:1016543307592](https://doi.org/10.1023/A:1016543307592)
- **[Brinkmann+ 13]** G. Brinkmann, J. Goedgebeur, J. Hägglund, K. Markström. "Generation and properties of snarks." *Journal of Combinatorial Theory, Series B*, 103(4):468–488, 2013. [DOI:10.1016/j.jctb.2013.05.001](https://doi.org/10.1016/j.jctb.2013.05.001)
- **[Burlet+ 84a]** M. Burlet, J. Fonlupt. "Polynomial algorithm to recognize a Meyniel graph." *Annals of Discrete Mathematics*, 21:225–252, 1984. [DOI:10.1016/S0304-0208(08)72938-4](https://doi.org/10.1016/S0304-0208(08)72938-4)
- **[Burlet+ 84b]** M. Burlet, J. P. Uhry. "Parity graphs." *Annals of Discrete Mathematics*, 21:253–277, 1984. [DOI:10.1016/S0304-0208(08)72939-6](https://doi.org/10.1016/S0304-0208(08)72939-6)
- **[Chang+ 07]** M.-S. Chang, M.-T. Ko. "The 3-Steiner root problem." *Proceedings of WG 2007*, Lecture Notes in Computer Science 4769, pp. 109–120, 2007. [DOI:10.1007/978-3-540-74839-7_11](https://doi.org/10.1007/978-3-540-74839-7_11)
- **[Chartrand+ 67]** G. Chartrand, F. Harary. "Planar permutation graphs." *Annales de l'Institut Henri Poincaré B*, 3(4):433–438, 1967.
- **[Cheng+ 16]** C. Cheng, K. L. Collins, A. N. Trenk. "Split graphs and Nordhaus–Gaddum graphs." *Discrete Mathematics*, 339(9):2345–2356, 2016. [DOI:10.1016/j.disc.2016.04.001](https://doi.org/10.1016/j.disc.2016.04.001)
- **[Chudnovsky 12]** M. Chudnovsky. "The structure of bull-free graphs I — Three-edge-paths with centers and anticenters." *Journal of Combinatorial Theory, Series B*, 102(1):233–251, 2012. [DOI:10.1016/j.jctb.2011.07.003](https://doi.org/10.1016/j.jctb.2011.07.003)
- **[Chudnovsky+ 06]** M. Chudnovsky, N. Robertson, P. Seymour, R. Thomas. "The strong perfect graph theorem." *Annals of Mathematics*, 164(1):51–229, 2006. [DOI:10.4007/annals.2006.164.51](https://doi.org/10.4007/annals.2006.164.51)
- **[Chudnovsky+ 20]** M. Chudnovsky, A. Scott, P. Seymour, S. Spirkl. "Detecting an odd hole." *Journal of the ACM*, 67(1):5:1–5:12, 2020. [DOI:10.1145/3375720](https://doi.org/10.1145/3375720)
- **[Chvátal+ 77]** V. Chvátal, P. L. Hammer. "Aggregation of inequalities in integer programming." *Annals of Discrete Mathematics*, 1:145–162, 1977. [DOI:10.1016/S0167-5060(08)70731-3](https://doi.org/10.1016/S0167-5060(08)70731-3)
- **[Cogis 82]** O. Cogis. "On the Ferrers dimension of a digraph." *Discrete Mathematics*, 38(1):47–52, 1982. [DOI:10.1016/0012-365X(82)90167-4](https://doi.org/10.1016/0012-365X(82)90167-4)
- **[Conforti+ 02]** M. Conforti, G. Cornuéjols, A. Kapoor, K. Vušković. "Even-hole-free graphs, Part I: Decomposition theorem." *Journal of Graph Theory*, 39(1):6–49, 2002. [DOI:10.1002/jgt.10006](https://doi.org/10.1002/jgt.10006)
- **[Corneil+ 81]** D. G. Corneil, H. Lerchs, L. Stewart Burlingham. "Complement reducible graphs." *Discrete Applied Mathematics*, 3(3):163–174, 1981. [DOI:10.1016/0166-218X(81)90013-5](https://doi.org/10.1016/0166-218X(81)90013-5)
- **[Corneil+ 85]** D. G. Corneil, Y. Perl, L. K. Stewart. "A linear recognition algorithm for cographs." *SIAM Journal on Computing*, 14(4):926–934, 1985. [DOI:10.1137/0214065](https://doi.org/10.1137/0214065)
- **[Corneil+ 97]** D. G. Corneil, S. Olariu, L. Stewart. "Asteroidal triple-free graphs." *SIAM Journal on Discrete Mathematics*, 10(3):399–430, 1997. [DOI:10.1137/S0895480193250125](https://doi.org/10.1137/S0895480193250125)
- **[Dagan+ 88]** I. Dagan, M. C. Golumbic, R. Y. Pinter. "Trapezoid graphs and their coloring." *Discrete Applied Mathematics*, 21(1):35–46, 1988. [DOI:10.1016/0166-218X(88)90032-7](https://doi.org/10.1016/0166-218X(88)90032-7)
- **[Dahlhaus+ 87]** E. Dahlhaus, P. Duchet. "On strongly chordal graphs." *Ars Combinatoria*, 24B:23–30, 1987.
- **[de Fraysseix+ 06]** H. de Fraysseix, P. Ossona de Mendez, P. Rosenstiehl. "Trémaux trees and planarity." *International Journal of Foundations of Computer Science*, 17(5):1017–1029, 2006. [DOI:10.1142/S0129054106004248](https://doi.org/10.1142/S0129054106004248)
- **[Dom+ 06]** M. Dom, J. Guo, F. Hüffner, R. Niedermeier. "Error compensation in leaf power problems." *Algorithmica*, 44(4):363–381, 2006. [DOI:10.1007/s00453-005-1180-z](https://doi.org/10.1007/s00453-005-1180-z)
- **[Duffin 65]** R. J. Duffin. "Topology of series-parallel networks." *Journal of Mathematical Analysis and Applications*, 10(2):303–318, 1965. [DOI:10.1016/0022-247X(65)90125-3](https://doi.org/10.1016/0022-247X(65)90125-3)
- **[Euler 1741]** L. Euler. "Solutio problematis ad geometriam situs pertinentis." *Commentarii Academiae Scientiarum Petropolitanae*, 8:128–140, 1741 (presented 1736).
- **[Farber 83]** M. Farber. "Characterizations of strongly chordal graphs." *Discrete Mathematics*, 43(2–3):173–189, 1983. [DOI:10.1016/0012-365X(83)90154-1](https://doi.org/10.1016/0012-365X(83)90154-1)
- **[Faudree+ 97]** R. Faudree, E. Flandrin, Z. Ryjáček. "Claw-free graphs — A survey." *Discrete Mathematics*, 164(1–3):87–147, 1997. [DOI:10.1016/S0012-365X(96)00045-3](https://doi.org/10.1016/S0012-365X(96)00045-3)
- **[Földes+ 77]** S. Földes, P. L. Hammer. "Split graphs." *Congressus Numerantium*, 19:311–315, 1977.
- **[Fulkerson+ 65]** D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs." *Pacific Journal of Mathematics*, 15(3):835–855, 1965. [DOI:10.2140/pjm.1965.15.835](https://doi.org/10.2140/pjm.1965.15.835)
- **[Gallai 67]** T. Gallai. "Transitiv orientierbare Graphen." *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1–2):25–66, 1967. [DOI:10.1007/BF02020961](https://doi.org/10.1007/BF02020961)
- **[Gasse 97]** E. Gasse. "A proof of a circle graph characterization." *Discrete Mathematics*, 173(1–3):277–283, 1997. [DOI:10.1016/S0012-365X(97)00068-X](https://doi.org/10.1016/S0012-365X(97)00068-X)
- **[Geelen+ 20]** J. Geelen, E. Lee. "Naji's characterization of circle graphs." *Journal of Graph Theory*, 93(1):21–33, 2020. [DOI:10.1002/jgt.22466](https://doi.org/10.1002/jgt.22466)
- **[Golumbic 78]** M. C. Golumbic. "Trivially perfect graphs." *Discrete Mathematics*, 24(1):105–107, 1978. [DOI:10.1016/0012-365X(78)90178-4](https://doi.org/10.1016/0012-365X(78)90178-4)
- **[Golumbic 80]** M. C. Golumbic. *Algorithmic Graph Theory and Perfect Graphs.* Academic Press, 1980; 2nd edition, Annals of Discrete Mathematics 57, Elsevier, 2004. [DOI:10.1016/S0167-5060(04)80053-0](https://doi.org/10.1016/S0167-5060(04)80053-0)
- **[Golumbic+ 78]** M. C. Golumbic, C. F. Goss. "Perfect elimination and chordal bipartite graphs." *Journal of Graph Theory*, 2(2):155–163, 1978. [DOI:10.1002/jgt.3190020209](https://doi.org/10.1002/jgt.3190020209)
- **[Halin 71]** R. Halin. "Studies on minimally n-connected graphs." In D. J. A. Welsh (ed.), *Combinatorial Mathematics and its Applications*, Academic Press, pp. 129–136, 1971.
- **[Hammer+ 81]** P. L. Hammer, B. Simeone. "The splittance of a graph." *Combinatorica*, 1(3):275–284, 1981. [DOI:10.1007/BF02579333](https://doi.org/10.1007/BF02579333)
- **[Harary 63]** F. Harary. "A characterization of block-graphs." *Canadian Mathematical Bulletin*, 6(1):1–6, 1963. [DOI:10.4153/CMB-1963-001-x](https://doi.org/10.4153/CMB-1963-001-x)
- **[Harary+ 53]** F. Harary, G. E. Uhlenbeck. "On the number of Husimi trees, I." *Proceedings of the National Academy of Sciences*, 39(4):315–322, 1953. [DOI:10.1073/pnas.39.4.315](https://doi.org/10.1073/pnas.39.4.315)
- **[Harary+ 73]** F. Harary, A. J. Schwenk. "The number of caterpillars." *Discrete Mathematics*, 6(4):359–365, 1973. [DOI:10.1016/0012-365X(73)90067-8](https://doi.org/10.1016/0012-365X(73)90067-8)
- **[Hayward 85]** R. B. Hayward. "Weakly triangulated graphs." *Journal of Combinatorial Theory, Series B*, 39(3):200–208, 1985. [DOI:10.1016/0095-8956(85)90050-4](https://doi.org/10.1016/0095-8956(85)90050-4)
- **[Hayward 96]** R. B. Hayward. "Generating weakly triangulated graphs." *Journal of Graph Theory*, 21(1):67–69, 1996. [DOI:10.1002/(SICI)1097-0118(199601)21:1%3C67::AID-JGT9%3E3.0.CO;2-K](https://doi.org/10.1002/(SICI)1097-0118(199601)21:1%3C67::AID-JGT9%3E3.0.CO;2-K)
- **[Hopcroft+ 73]** J. Hopcroft, R. Tarjan. "Dividing a graph into triconnected components." *SIAM Journal on Computing*, 2(3):135–158, 1973. [DOI:10.1137/0202012](https://doi.org/10.1137/0202012)
- **[Howorka 77]** E. Howorka. "A characterization of distance-hereditary graphs." *The Quarterly Journal of Mathematics*, 28(4):417–420, 1977. [DOI:10.1093/qmath/28.4.417](https://doi.org/10.1093/qmath/28.4.417)
- **[Howorka 81]** E. Howorka. "A characterization of Ptolemaic graphs." *Journal of Graph Theory*, 5(3):323–331, 1981. [DOI:10.1002/jgt.3190050314](https://doi.org/10.1002/jgt.3190050314)
- **[Isaacs 75]** R. Isaacs. "Infinite families of nontrivial trivalent graphs which are not Tait colorable." *The American Mathematical Monthly*, 82(3):221–239, 1975. [DOI:10.2307/2319844](https://doi.org/10.2307/2319844)
- **[Itai+ 78]** A. Itai, M. Rodeh. "Finding a minimum circuit in a graph." *SIAM Journal on Computing*, 7(4):413–423, 1978. [DOI:10.1137/0207033](https://doi.org/10.1137/0207033)
- **[Jacobs+ 97]** D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation: the pebble game." *Journal of Computational Physics*, 137(2):346–365, 1997. [DOI:10.1006/jcph.1997.5809](https://doi.org/10.1006/jcph.1997.5809)
- **[Jones+ 18]** Á. A. Jones, F. Protti, R. R. Del-Vecchio. "Cograph generation with linear delay." *Theoretical Computer Science*, 713:1–10, 2018. [DOI:10.1016/j.tcs.2017.12.037](https://doi.org/10.1016/j.tcs.2017.12.037)
- **[Kiyomi+ 06]** M. Kiyomi, T. Uno. "Generating chordal graphs included in given graphs." *IEICE Transactions on Information and Systems*, E89-D(2):763–770, 2006. [DOI:10.1093/ietisy/e89-d.2.763](https://doi.org/10.1093/ietisy/e89-d.2.763)
- **[Kiyomi-Kijima+ 06]** M. Kiyomi, S. Kijima, T. Uno. "Listing chordal graphs and interval graphs." *Graph-Theoretic Concepts in Computer Science (WG 2006)*, LNCS 4271:68–77, 2006. [DOI:10.1007/11917496_7](https://doi.org/10.1007/11917496_7)
- **[Kiyomi thesis 06]** M. Kiyomi. *Studies on Subgraph and Supergraph Enumeration Algorithms.* Ph.D. thesis, The Graduate University for Advanced Studies, 2006, Section 4.1.5. [PDF](https://www.nii.ac.jp/graduate/wp-content/themes/nii_original/assets/pdf/students_thesis/18/kiyomi_Dr_thesis.pdf)
- **[König 36]** D. König. *Theorie der endlichen und unendlichen Graphen.* Akademische Verlagsgesellschaft, Leipzig, 1936.
- **[Krausz 43]** J. Krausz. "Démonstration nouvelle d'un théorème de Whitney sur les réseaux." *Matematikai és Fizikai Lapok*, 50:75–85, 1943.
- **[Kuratowski 30]** K. Kuratowski. "Sur le problème des courbes gauches en topologie." *Fundamenta Mathematicae*, 15(1):271–283, 1930. [DOI:10.4064/fm-15-1-271-283](https://doi.org/10.4064/fm-15-1-271-283)
- **[Lafond 22]** M. Lafond. "Recognizing k-leaf powers in polynomial time, for constant k." *Proceedings of SODA 2022*, pp. 1384–1410, 2022. [DOI:10.1137/1.9781611977073.58](https://doi.org/10.1137/1.9781611977073.58)
- **[Laman 70]** G. Laman. "On graphs and rigidity of plane skeletal structures." *Journal of Engineering Mathematics*, 4(4):331–340, 1970. [DOI:10.1007/BF01534980](https://doi.org/10.1007/BF01534980)
- **[Lekkerkerker+ 62]** C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line." *Fundamenta Mathematicae*, 51(1):45–64, 1962. [DOI:10.4064/fm-51-1-45-64](https://doi.org/10.4064/fm-51-1-45-64)
- **[Mahadev+ 95]** N. V. R. Mahadev, U. N. Peled. *Threshold Graphs and Related Topics.* Annals of Discrete Mathematics 56, North-Holland, 1995.
- **[McConnell 03]** R. M. McConnell. "Linear-time recognition of circular-arc graphs." *Algorithmica*, 37(2):93–147, 2003. [DOI:10.1007/s00453-003-1032-7](https://doi.org/10.1007/s00453-003-1032-7)
- **[McKay 98]** B. D. McKay. "Isomorph-free exhaustive generation." *Journal of Algorithms*, 26(2):306–324, 1998. [DOI:10.1006/jagm.1997.0898](https://doi.org/10.1006/jagm.1997.0898)
- **[Meringer 99]** M. Meringer. "Fast generation of regular graphs and construction of cages." *Journal of Graph Theory*, 30(2):137–146, 1999. [DOI:10.1002/(SICI)1097-0118(199902)30:2<137::AID-JGT7>3.0.CO;2-G](https://doi.org/10.1002/(SICI)1097-0118(199902)30:2%3C137::AID-JGT7%3E3.0.CO;2-G)
- **[Meyniel 76]** H. Meyniel. "On the perfect graph conjecture." *Discrete Mathematics*, 16(4):339–342, 1976. [DOI:10.1016/S0012-365X(76)80008-8](https://doi.org/10.1016/S0012-365X(76)80008-8)
- **[Moon 68]** J. W. Moon. *Topics on Tournaments.* Holt, Rinehart and Winston, New York, 1968.
- **[Naji 85]** W. Naji. "Reconnaissance des graphes de cordes." *Discrete Mathematics*, 54(3):329–337, 1985. [DOI:10.1016/0012-365X(85)90117-7](https://doi.org/10.1016/0012-365X(85)90117-7)
- **[Nishimura+ 02]** N. Nishimura, P. Ragde, D. M. Thilikos. "On graph powers for leaf-labeled trees." *Journal of Algorithms*, 42(1):69–108, 2002. [DOI:10.1006/jagm.2001.1195](https://doi.org/10.1006/jagm.2001.1195)
- **[Paul+ 24]** C. Paul, E. Protopapas. "Proper chordal graphs." *Proceedings of STACS 2024*, LIPIcs 289, 53:1–53:17, 2024. [DOI:10.4230/LIPIcs.STACS.2024.53](https://doi.org/10.4230/LIPIcs.STACS.2024.53)
- **[Petersen 1891]** J. Petersen. "Die Theorie der regulären Graphs." *Acta Mathematica*, 15:193–220, 1891. [DOI:10.1007/BF02392606](https://doi.org/10.1007/BF02392606)
- **[Pnueli+ 71]** A. Pnueli, A. Lempel, S. Even. "Transitive orientation of graphs and identification of permutation graphs." *Canadian Journal of Mathematics*, 23(1):160–175, 1971. [DOI:10.4153/CJM-1971-016-5](https://doi.org/10.4153/CJM-1971-016-5)
- **[Ringel 63]** G. Ringel. "Selbstkomplementäre Graphen." *Archiv der Mathematik*, 14(1):354–358, 1963. [DOI:10.1007/BF01234967](https://doi.org/10.1007/BF01234967)
- **[Roberts 69]** F. S. Roberts. "Indifference graphs." In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139–146, 1969.
- **[Robertson+ 95]** N. Robertson, P. D. Seymour. "Graph minors. XIII. The disjoint paths problem." *Journal of Combinatorial Theory, Series B*, 63(1):65–110, 1995. [DOI:10.1006/jctb.1995.1006](https://doi.org/10.1006/jctb.1995.1006)
- **[Rose 74]** D. J. Rose. "On simple characterizations of k-trees." *Discrete Mathematics*, 7(3–4):317–322, 1974. [DOI:10.1016/0012-365X(74)90042-9](https://doi.org/10.1016/0012-365X(74)90042-9)
- **[Rose+ 76]** D. J. Rose, R. E. Tarjan, G. S. Lueker. "Algorithmic aspects of vertex elimination on graphs." *SIAM Journal on Computing*, 5(2):266–283, 1976. [DOI:10.1137/0205021](https://doi.org/10.1137/0205021)
- **[Sachs 62]** H. Sachs. "Über selbstkomplementäre Graphen." *Publicationes Mathematicae Debrecen*, 9:270–288, 1962.
- **[Saitoh+ 10]** T. Saitoh, K. Yamanaka, M. Kiyomi, R. Uehara. "Random Generation and Enumeration of Proper Interval Graphs." *IEICE Transactions on Information and Systems*, E93-D(7):1816–1823, 2010. [DOI:10.1587/transinf.E93.D.1816](https://doi.org/10.1587/transinf.E93.D.1816)
- **[Seinsche 74]** D. Seinsche. "On a property of the class of n-colorable graphs." *Journal of Combinatorial Theory, Series B*, 16(2):191–193, 1974. [DOI:10.1016/0095-8956(74)90063-X](https://doi.org/10.1016/0095-8956(74)90063-X)
- **[Spinrad+ 87]** J. P. Spinrad, A. Brandstädt, L. Stewart. "Bipartite permutation graphs." *Discrete Applied Mathematics*, 18(3):279–292, 1987. [DOI:10.1016/S0166-218X(87)80003-3](https://doi.org/10.1016/S0166-218X(87)80003-3)
- **[Spinrad+ 95]** J. P. Spinrad, R. Sritharan. "Algorithms for weakly triangulated graphs." *Discrete Applied Mathematics*, 59(2):181–191, 1995. [DOI:10.1016/0166-218X(93)E0161-Q](https://doi.org/10.1016/0166-218X(93)E0161-Q)
- **[Steinitz 22]** E. Steinitz. "Polyeder und Raumeinteilungen." *Encyklopädie der mathematischen Wissenschaften*, Band 3, Heft 9, 1922.
- **[Tarjan 72]** R. Tarjan. "Depth-first search and linear graph algorithms." *SIAM Journal on Computing*, 1(2):146–160, 1972. [DOI:10.1137/0201010](https://doi.org/10.1137/0201010)
- **[Tarjan+ 84]** R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs." *SIAM Journal on Computing*, 13(3):566–579, 1984. [DOI:10.1137/0213035](https://doi.org/10.1137/0213035)
- **[Troyka 19]** J. M. Troyka. "Split graphs: combinatorial species and asymptotics." *Electronic Journal of Combinatorics*, 26(2):P2.42, 2019. [arXiv:1803.07248](https://arxiv.org/abs/1803.07248)
- **[Tucker 74]** A. Tucker. "Structure theorems for some circular-arc graphs." *Discrete Mathematics*, 7(1–2):167–195, 1974. [DOI:10.1016/S0012-365X(74)80027-0](https://doi.org/10.1016/S0012-365X(74)80027-0)
- **[Tucker 80]** A. Tucker. "An efficient test for circular-arc graphs." *SIAM Journal on Computing*, 9(1):1–24, 1980. [DOI:10.1137/0209001](https://doi.org/10.1137/0209001)
- **[Valdes+ 82]** J. Valdes, R. E. Tarjan, E. L. Lawler. "The recognition of series parallel digraphs." *SIAM Journal on Computing*, 11(2):298–313, 1982. [DOI:10.1137/0211023](https://doi.org/10.1137/0211023)
- **[Whitney 32]** H. Whitney. "Congruent graphs and the connectivity of graphs." *American Journal of Mathematics*, 54(1):150–168, 1932. [DOI:10.2307/2371086](https://doi.org/10.2307/2371086)
- **[Wolk 62]** E. S. Wolk. "The comparability graph of a tree." *Proceedings of the American Mathematical Society*, 13(5):789–795, 1962. [DOI:10.1090/S0002-9939-1962-0172273-0](https://doi.org/10.1090/S0002-9939-1962-0172273-0)
- **[Wright+ 86]** R. A. Wright, B. Richmond, A. Odlyzko, B. D. McKay. "Constant time generation of free trees." *SIAM Journal on Computing*, 15(2):540–548, 1986. [DOI:10.1137/0215039](https://doi.org/10.1137/0215039)
- **[Yan+ 96]** J.-H. Yan, J.-J. Chen, G. J. Chang. "Quasi-threshold graphs." *Discrete Applied Mathematics*, 69(3):247–255, 1996. [DOI:10.1016/0166-218X(96)00094-7](https://doi.org/10.1016/0166-218X(96)00094-7)
- **[Yannakakis 82]** M. Yannakakis. "The complexity of the partial order dimension problem." *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351–358, 1982. [DOI:10.1137/0603036](https://doi.org/10.1137/0603036)

## ライセンス

[MIT](LICENSE)
