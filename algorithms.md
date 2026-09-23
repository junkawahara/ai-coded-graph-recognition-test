# グラフクラス専用列挙アルゴリズムの実装候補

調査日: 2026-09-23

対象は、先に確認した「汎用の頂点追加候補を認識器で絞る列挙」と、その関連実装である。現行ソースと一次資料を照合し、**実装項目21件、適用範囲の確認・追加設計を伴う候補4件**を挙げる。チェックボックスは、このリポジトリで残っている作業を表す。

専用アルゴリズムも reverse search や局所的な認識判定を利用することがある。置き換える対象は [Avis–Fukuda の枠組み](https://doi.org/10.1016/0166-218X(95)00026-N)そのものではなく、グラフクラスの構造を使わずに近傍部分集合を列挙する部分である。

## 実装時に共通する条件

- 現行のラベル付きAPIは、頂点集合 `{1, ..., n}` 上の辺集合をすべて出力する。非同型代表だけを出力する論文のアルゴリズムでは、そのままの置き換えにならない。以下でいう「ラベル展開」は、非同型代表 `G` ごとに自己同型群 `Aut(G)` による同一視を行い、異なるラベル付きグラフ `n! / |Aut(G)|` 個を生成する追加処理を指す。これは本リポジトリ向けの実装上の適用であり、引用論文のラベル付き列挙の保証を意味しない。
- pendant/twin 追加や Henneberg 構成では、削除できる頂点が最大ラベルとは限らない。追加順をラベル順に固定するだけでは列挙漏れを起こすため、ラベル展開または完全性を証明した別の親規則を用いる。
- 「連結のみ」「根付き」「平面埋め込み付き」「頂点数が高々 `n`」「辺数が高々 `m`」を、現行APIの対象と区別する。非連結グラフを含むクラスでは連結成分の合成が必要になる。上限サイズまでの列挙から固定サイズだけを抜き出しても、元論文の遅延保証がそのまま成立するとは限らない。
- 論文の `O(1)` 出力時間は、文字列表現や差分出力の計算モデルに依存する。現行APIの辺リスト生成、ラベル展開、同型排除、結果の全件保持の費用は別途評価する。

## 1. 既存の専用生成法をラベル付き版へ適用する

- [x] **A01. Co-chordal — chordal 専用列挙の補グラフを出力する。**

  変更対象: [co_chordal_labeled_enum.h](include/enumerators/co_chordal_labeled_enum.h)。既存の [chordal_labeled_enum.h](include/enumerators/chordal_labeled_enum.h) の `KIYOMI_UNO` を呼び、同じ頂点ラベル上で辺集合を補集合に変換する。補グラフは全単射なので、追加の同型排除や co-chordal 認識は不要。ラベルなし版にはすでに補グラフ変換がある。

  実装済み: `KIYOMI_UNO` を既定とし、従来の頂点追加・認識方式は `REVERSE_SEARCH` で選択できる。Python は `algorithm="reverse_search"`、CLI は `--algo reverse_search` で従来方式を指定する。

  根拠: Kiyomi–Uno, *Generating Chordal Graphs Included in Given Graphs* (2006), [論文](https://doi.org/10.1093/ietisy/e89-d.2.763)。専用 chordal 列挙とクラス定義からの適用案。

- [x] **A02. Co-interval — interval 専用列挙の補グラフを出力する。**

  変更対象: [co_interval_labeled_enum.h](include/enumerators/co_interval_labeled_enum.h)。既存の [interval_labeled_enum.h](include/enumerators/interval_labeled_enum.h) の `KIYOMI_KIJIMA_UNO` を使い、各出力を補グラフにする。A01 と同じ全単射による置き換えで、ラベルなし版の補グラフ変換を参考にできる。

  実装済み: `KIYOMI_KIJIMA_UNO` を既定とし、従来の頂点追加・認識方式は `REVERSE_SEARCH` で選択できる。Python は `algorithm="reverse_search"`、CLI は `--algo reverse_search` で従来方式を指定する。補グラフへの変換は interval 列挙器のコールバック内で行う。

  根拠: Kiyomi–Kijima–Uno, *Listing Chordal Graphs and Interval Graphs* (WG 2006), [論文](https://doi.org/10.1007/11917496_7)。

- [x] **A03. Proper interval — Saitoh らの括弧列表現による列挙をラベル付き版に適用する。**

  変更対象: [proper_interval_labeled_enum.h](include/enumerators/proper_interval_labeled_enum.h)。[proper_interval_unlabeled_enum.h](include/enumerators/proper_interval_unlabeled_enum.h) に、連結グラフの括弧列と反転対称性、非連結グラフの成分合成は実装済み。これを再利用し、ラベル展開を追加する。論文の最適化された reverse search と、現行の Dyck 語DFSを同一実装とは扱わない。

  実装済み: `STRING_REPRESENTATION` を既定とし、ラベルなし列挙の各代表元を隣接ラベル交換で展開する。訪問済み辺集合によって自己同型による重複を除き、各代表元から `n! / |Aut(G)|` 個を出力する。反転・twin・同じ連結成分の交換もこの重複排除に含まれる。従来の弦グラフ逆探索・認識方式は `REVERSE_SEARCH`、Python の `algorithm="reverse_search"`、CLI の `--algo reverse_search` で選択できる。出力辺リストの構築・集合照合と結果の全件保持が必要であり、原論文の1件 `O(1)` 保証は主張しない。

  根拠: Saitoh–Yamanaka–Kiyomi–Uehara, *Random Generation and Enumeration of Proper Interval Graphs* (2010), [論文](https://doi.org/10.1587/transinf.E93.D.1816)。原論文は連結・ラベルなし、表現の列挙に対して1件 `O(1)`。

- [x] **A04. Distance-hereditary — pendant / true twin / false twin 追加による列挙を適用する。**

  変更対象: [distance_hereditary_labeled_enum.h](include/enumerators/distance_hereditary_labeled_enum.h)。[ラベルなし版](include/enumerators/distance_hereditary_unlabeled_enum.h) の構造を保つ3種類の追加操作と成分合成を再利用し、ラベル展開を実装する。さらに大規模化する際は、サイズごとの正準形集合を保持する現方式から、論文の DH-tree に基づく親判定へ移行する。

  実装済み: `VERTEX_INCREMENTAL` を既定とし、ラベルなし代表を隣接ラベル交換で展開する。ラベル展開は proper interval と共通の [labeled_expansion.h](include/util/labeled_expansion.h) を使い、twin・同型な成分の交換による重複を辺集合で排除する。従来の頂点追加・認識方式は `REVERSE_SEARCH`、Python の `algorithm="reverse_search"`、CLI の `--algo reverse_search` で選択できる。C++・CLI のビットマスクによる上限は新方式が63頂点、従来方式が64頂点。正準形集合・ラベル展開の訪問済み集合・全出力を保持する実装であり、DH-tree に基づく親判定や論文の遅延保証は実装していない。

  根拠: Yamazaki–Qian–Uehara, *Efficient enumeration of non-isomorphic distance-hereditary graphs and related graphs* (2024), [論文](https://doi.org/10.1016/j.dam.2023.09.002)。DH について確認できる定理は「高々 `n` 頂点の非同型グラフを `O(n^3)` 遅延」であり、現行コードの計算量ではない。

- [ ] **A05. Ptolemaic — chordality を保つ pendant / twin 追加による列挙を適用する。**

  変更対象: [ptolemaic_labeled_enum.h](include/enumerators/ptolemaic_labeled_enum.h)。[ラベルなし版](include/enumerators/ptolemaic_unlabeled_enum.h) の pendant・true twin 追加と、simplicial 頂点に限った false twin 追加を再利用する。A04 とラベル展開・成分合成を共有し、専用の正準親方式も検討する。

  根拠: [Yamazaki–Qian–Uehara (2024)](https://doi.org/10.1016/j.dam.2023.09.002) の Ptolemaic 列挙。現在すでにある構造的なラベルなし列挙を、新規未実装と数えない。

- [ ] **A06. 3-leaf power — critical clique の構造を保つ追加操作による列挙を適用する。**

  変更対象: [three_leaf_power_labeled_enum.h](include/enumerators/three_leaf_power_labeled_enum.h)。[ラベルなし版](include/enumerators/three_leaf_power_unlabeled_enum.h) の true twin 追加と、条件を満たす頂点への pendant 追加を再利用する。critical clique tree の対称性を扱う正準化、成分合成、ラベル展開を組み合わせる。

  根拠: [Yamazaki–Qian–Uehara (2024)](https://doi.org/10.1016/j.dam.2023.09.002) の 3-leaf power への拡張。4-leaf power・5-leaf power に同じ操作で拡張できるとは主張しない。

- [ ] **A07. Bipartite — 二部彩色から許容近傍だけを生成する。**

  変更対象: [bipartite_labeled_enum.h](include/enumerators/bipartite_labeled_enum.h)、[ラベルなし版](include/enumerators/bipartite_unlabeled_enum.h)。現在の各連結成分について、追加頂点の近傍を二部集合の片側の部分集合に限る。全近傍を作ってから認識する処理を、成分ごとの許容選択の直積に置き換える。空集合を両側から重複生成しないようにする。この頂点追加法はラベル順のまま全ラベル付き二部グラフを生成できる。

  根拠: [McKay (1998), §2–4](https://users.cecs.anu.edu.au/~bdm/papers/orderly.pdf) と既存ラベルなし版の二部性保存条件。別実装の参考として [nauty の `genbg`](https://users.cecs.anu.edu.au/~bdm/nauty/nug29.pdf) があるが、こちらは二部彩色付きの対象なので、採用する場合は色を忘れた後の重複も処理する。

- [ ] **A08. Triangle-free — 独立集合を近傍とする頂点追加で列挙する。**

  変更対象: [triangle_free_labeled_enum.h](include/enumerators/triangle_free_labeled_enum.h)、[ラベルなし版](include/enumerators/triangle_free_unlabeled_enum.h)。新頂点の近傍となる独立集合を直接列挙する。ラベルなし版の独立性判定は再利用できるが、現在の全 `2^k` 部分集合走査も併せて置き換える。ラベル付き版は追加ラベルを固定でき、ラベルなし版は正準構成経路と組み合わせる。

  根拠: [McKay, *Isomorph-free exhaustive generation* (1998), §2](https://users.cecs.anu.edu.au/~bdm/papers/orderly.pdf) の具体例。極大 triangle-free グラフだけを生成するアルゴリズムでは、このAPIの全グラフをカバーできない。

## 2. 文献にある専用列挙を新たに実装する

- [ ] **B01. Bipartite permutation — Saitoh らの線分表現・正準文字列による reverse search。**

  変更対象: [bipartite_permutation_labeled_enum.h](include/enumerators/bipartite_permutation_labeled_enum.h)、[ラベルなし版](include/enumerators/bipartite_permutation_unlabeled_enum.h)。現状は両方とも候補グラフを認識している。まず連結・非同型代表の専用列挙を実装し、成分合成とラベル展開を追加する。異なる線分表現が同じグラフを表す対称性を処理する。

  根拠: Saitoh–Otachi–Yamanaka–Uehara, *Random Generation and Enumeration of Bipartite Permutation Graphs* (J. Discrete Algorithms 10, 2012), [著者版・書誌情報](https://dspace.jaist.ac.jp/dspace/handle/10119/10720?locale=en)。連結・ラベルなしの表現を1件 `O(1)` で列挙する。

- [ ] **B02. Permutation — modular decomposition の正準表現と辺操作による列挙。**

  変更対象: [permutation_labeled_enum.h](include/enumerators/permutation_labeled_enum.h)、[ラベルなし版](include/enumerators/permutation_unlabeled_enum.h)。Yamazaki らの非同型列挙を実装する。modular decomposition に基づく正準化と、permutation 性を保つ辺追加による親規則を使い、その逆方向を探索する。認識器は局所的な辺候補の判定に残り得るが、任意の近傍部分集合走査からは脱却できる。ラベル付きAPIにはラベル展開を付ける。

  根拠: Yamazaki–Saitoh–Kiyomi–Uehara, *Enumeration of Nonisomorphic Interval Graphs and Nonisomorphic Permutation Graphs* (TCS 806, 2020), [論文](https://doi.org/10.1016/j.tcs.2019.04.017)、[会議版本文](https://dspace.jaist.ac.jp/dspace/bitstream/10119/15855/1/23994.pdf)。固定 `n` の非同型列挙に多項式遅延を保証する。

- [ ] **B03. Interval のラベルなし版 — Mikos の MPQ-tree による列挙。**

  変更対象: [interval_unlabeled_enum.h](include/enumerators/interval_unlabeled_enum.h)。現状の正準頂点追加と `check_interval` を、MPQ-tree と辺操作に基づく専用探索に置き換える。ラベル付き版の標準方式はすでに専用の Kiyomi–Kijima–Uno なので、ここはラベルなし版だけの新規実装項目。

  根拠: Patryk Mikos, *Efficient enumeration of non-isomorphic interval graphs* (DMTCS 23:1, 2021), [論文・本文](https://dmtcs.episciences.org/7246)。固定 `n` で `O(n^3 log n)` 遅延。Yamazaki らの `O(n^4)` 遅延を改善している。

- [ ] **B04. Block — BlockGraphGen による重み付き block tree の非同型列挙。**

  変更対象: [block_labeled_enum.h](include/enumerators/block_labeled_enum.h)。ラベルなし専用列挙器は新規追加する。ブロックと関節点の接続を色・重み付き木で表し、重み付き重心を使って同型な木を排除する。木から各クリークを復元し、非連結成分の合成とラベル展開を追加する。

  根拠: Ekim–Shalom–Yirik, *Generation of weighted trees, block trees and block graphs* (2024), [本文](https://arxiv.org/html/2401.09764v1)。連結 block graph の非同型生成を扱う。木表現の生成時間と、密なクリークの辺を明示する費用を分けて評価する。

- [ ] **B05. Biconnected — Stolee の ear augmentation / EarSearch。**

  変更対象: [biconnected_labeled_enum.h](include/enumerators/biconnected_labeled_enum.h)、[ラベルなし版](include/enumerators/biconnected_unlabeled_enum.h)。サイクルから ear を追加し、正準な ear 削除で重複を排除する。途中状態から2頂点連結性を保ち、終端での認識に依存する探索を置き換える。固定 `n` までの内部頂点数管理とラベル展開を実装する。

  根拠: Derrick Stolee, *Isomorph-free generation of 2-connected graphs with applications* (2011), [論文](https://arxiv.org/abs/1104.5261)、[著者実装 EarSearch](https://github.com/derrickstolee/EarSearch)。対象はラベルなし2頂点連結グラフ。辺数等を制限した場合の効果も測定する。

- [ ] **B06. Laman — Henneberg I / II 構成による生成。**

  変更対象: [laman_labeled_enum.h](include/enumerators/laman_labeled_enum.h)、[ラベルなし版](include/enumerators/laman_unlabeled_enum.h)。`K2` から、2頂点への新頂点接続と、辺の細分・第3頂点への接続を用いて生成する。現在の部分集合に対する `(2,3)`-sparsity 管理から、Laman 性を保つ構成へ移す。異なる構成履歴の同型重複を排除し、ラベル展開を追加する。正準削除を設計する場合、逆 Henneberg II の可否判定には pebble game 等を使う。

  根拠: Taufiqurrachman–Yamauchi–Kijima–Yamashita, *Enumeration of Henneberg decompositions of Laman graphs and M-Circuits*, [著者らの本文 §3.1](https://www.ipsj-kyushu.jp/page/ronbun/hinokuni/1001/B-1/B-1-3.pdf)。分解列挙だけでなく、全 Laman グラフを構成して非同型なものを得る実装も記載されている。多項式遅延はこの資料からは主張しない。

## 3. 平面グラフ・正則グラフに特化した生成を実装する

以下の plantri 系では、埋め込みの各頂点周りの辺順序を管理する共通基盤が必要。3連結平面グラフは鏡映も同一視すれば埋め込みの一意性を利用できるが、1・2連結では異なる埋め込みが同じ抽象グラフになるため、その同型重複も除く。ラベル付きAPIにはさらにラベル展開を付ける。[plantri 公式説明・マニュアル](https://users.cecs.anu.edu.au/~bdm/plantri/)

- [ ] **C01. Maximal planar — plantri の単純三角形分割生成。**

  変更対象: [maximal_planar_labeled_enum.h](include/enumerators/maximal_planar_labeled_enum.h)、[ラベルなし版](include/enumerators/maximal_planar_unlabeled_enum.h)。三角形分割を保つ局所拡張と正準縮約を実装する。候補ごとの平面性判定と `m = 3n - 6` による絞り込みを置き換える。小さい `n` の扱いは現行APIに合わせる。

  根拠: Brinkmann–McKay, *Fast generation of planar graphs* (MATCH 58, 2007), [公式配布](https://users.cecs.anu.edu.au/~bdm/plantri/)、[保存済み本文](references/brinkmann2007_plantri_full.pdf)。

- [ ] **C02. Polyhedral — plantri の3連結単純平面グラフ生成。**

  変更対象: [polyhedral_labeled_enum.h](include/enumerators/polyhedral_labeled_enum.h)、[ラベルなし版](include/enumerators/polyhedral_unlabeled_enum.h)。三角形分割を起点とした辺削除と、3連結性を維持する専用の適用条件・正準親判定を実装する。C01 の埋め込み基盤を共有する。

  根拠: [Brinkmann–McKay (2007) / plantri](https://users.cecs.anu.edu.au/~bdm/plantri/)。3連結の場合は、埋め込みを忘れた際の追加重複が生じないという利点がある。

- [ ] **C03. Cubic planar — plantri の双対三角形分割による生成。**

  変更対象: [cubic_planar_labeled_enum.h](include/enumerators/cubic_planar_labeled_enum.h)、[ラベルなし版](include/enumerators/cubic_planar_unlabeled_enum.h)。双対側の三角形分割を生成して3正則平面グラフに戻す。C01 の単純三角形分割の双対だけでは3連結なものに限られる。現行の全クラスをカバーするには、1・2連結の場合に対応する双対側のループ・多重辺の扱い、原グラフの単純性、成分合成まで実装する。

  根拠: [plantri マニュアル](https://users.cecs.anu.edu.au/~bdm/plantri/plantri-guide.txt) の連結度別 cubic planar 生成。

- [ ] **C04. Planar — plantri の一般単純平面グラフ生成と成分合成。**

  変更対象: [planar_labeled_enum.h](include/enumerators/planar_labeled_enum.h)、[ラベルなし版](include/enumerators/planar_unlabeled_enum.h)。三角形分割からの専用辺削除を、必要な連結度まで含めて実装する。埋め込みを忘れた後の同型排除と非連結グラフの合成を加える。平面地図の件数を抽象グラフの件数として返さないこと。

  根拠: [Brinkmann–McKay (2007) / plantri](https://users.cecs.anu.edu.au/~bdm/plantri/)。C01–C03 と共通化し、連結度の高い部分から実装する。

- [ ] **C05. Simple quadrangulation — 3連結四角形分割の専用局所拡張。**

  変更対象: [simple_quadrangulation_unlabeled_enum.h](include/enumerators/simple_quadrangulation_unlabeled_enum.h)。現在の4正則候補を認識して双対化する方法を、四角形分割そのものの構成に置き換える。論文の **Theorem 3** に従い、pseudo-double wheel を種として `P1`・`P3` 拡張と正準縮約を実装する。`P1` は3連結性を保つ適用条件を守る。

  根拠: Brinkmann–Greenberg–Greenhill–McKay–Thomas–Wollan, *Generation of simple quadrangulations of the sphere* (2005), [論文](https://doi.org/10.1016/j.disc.2005.10.005)、[著者版](https://users.cecs.anu.edu.au/~bdm/papers/quad-fixed.pdf)。このリポジトリの [認識器](include/recognizers/simple_quadrangulation.h) は3連結性を要求するため、論文の「すべての単純四角形分割」の生成法とは区別する。

- [ ] **C06. Fullerene — buckygen の `L` / `B` 拡張と正準縮約。**

  変更対象: [fullerene_unlabeled_enum.h](include/enumerators/fullerene_unlabeled_enum.h)。次数5・6の双対候補を列挙して平面性を調べる方式から、fullerene を保つパッチ置換へ移す。`C20`・`C28` からの拡張と、別途生成する `(5,0)` nanotube 系列を含め、完全性を確保する。

  根拠: Brinkmann–Goedgebeur–McKay, *The Generation of Fullerenes* (2012), [論文](https://arxiv.org/abs/1207.7010)、[DOI](https://doi.org/10.1021/ci3003107)。非同型 fullerene の専用生成で、既存ファイルの双対化部分は再利用候補。

- [ ] **C07. Snark — snarkhunter の拡張と辺彩色に基づく先読み。**

  変更対象: [snark_labeled_enum.h](include/enumerators/snark_labeled_enum.h)、[ラベルなし版](include/enumerators/snark_unlabeled_enum.h)。専用 cubic 拡張・正準縮約と、3辺彩色可能な出力になる拡張を避ける先読みを組み合わせる。現在の次数・girth の制限と終端の性質検査だけの探索から変更する。現行の snark の定義に合わせて、girth と cyclic edge-connectivity の条件を設定する。

  根拠: Brinkmann–Goedgebeur–Hägglund–Markström, *Generation and properties of Snarks* (2013), [論文](https://arxiv.org/abs/1206.6690)、[著者配布](https://caagt.ugent.be/cubic/)。大きな girth を対象にする場合は Brinkmann–Goedgebeur, *Generation of cubic graphs and snarks with large girth* (2017), [論文](https://arxiv.org/abs/1512.05944) の拡張も採用候補。

## 4. 適用範囲の確認・追加設計を伴う候補

ここは、現行APIをそのまま置き換えられると確定した項目とは分ける。文献の生成対象が異なるもの、構造的な仕様から全列挙器を設計するもの、圧縮表現を追加するものを含む。

- [ ] **D01. Outerplanar — Wang–Nagamochi の根付き・彩色付き列挙を適用する。**

  変更候補: [outer_planar_labeled_enum.h](include/enumerators/outer_planar_labeled_enum.h)、[ラベルなし版](include/enumerators/outer_planar_unlabeled_enum.h)。原論文のブロック構造を用いる生成を実装し、1色への特殊化、根を忘れた同型排除、固定 `n` の出力、必要な成分合成、ラベル展開を設計する。

  根拠: Wang–Nagamochi, *Constant Time Generation of Rooted and Colored Outerplanar Graphs* (2010), [著者所属機関の TR 2010-007](https://www.amp.i.kyoto-u.ac.jp/tecrep/TR2010.html)。確認できた保証は「高々 `n` 頂点、根付き・彩色付き」に対する定数時間生成・線形領域。現行の根なしAPIへの変換後の保証は別に評価する。

- [ ] **D02. Series-parallel — Kawano–Nakano の専用列挙の定義を確認して適用する。**

  変更候補: [series_parallel_labeled_enum.h](include/enumerators/series_parallel_labeled_enum.h)、[ラベルなし版](include/enumerators/series_parallel_unlabeled_enum.h)。まず全文で端点の指定、ラベル、単純性、同型の定義を確認する。現行認識器は非連結も含む `K4`-minor-free 単純グラフを対象とするので、二端子版等であれば端点を忘れる処理、ブロック・成分の合成を追加する。

  根拠: Kawano–Nakano, *Generating All Series-Parallel Graphs* (2005), [出版社の抄録](https://globals.ieice.org/en_transactions/fundamentals/10.1093/ietfec/e88-a.5.1129/_p)、[2004年の研究報告](https://ipsj.ixsq.nii.ac.jp/records/31863)。専用列挙の存在と「高々 `m` 辺、平均定数時間」は確認できたが、今回本文を取得できず、現行APIとの定義の一致は未確認。

- [ ] **D03. Cactus — split-decomposition の文法から全列挙器を構成する。**

  変更候補: [cactus_labeled_enum.h](include/enumerators/cactus_labeled_enum.h)、[ラベルなし版](include/enumerators/cactus_unlabeled_enum.h)。Bahrani–Lumbroso の木構造・文法を基に、サイズを指定した構造生成、木の正準化、サイクルの回転・反転の同一視を設計する。非連結成分の合成とラベル展開も必要。

  根拠: Bahrani–Lumbroso, *Split-Decomposition Trees with Prime Nodes: Enumeration and Random Generation of Cactus Graphs* (ANALCO 2018), [本文](https://arxiv.org/abs/1711.10647)、[保存済みPDF](references/bahrani2017_cactus_enum.pdf)。文法とランダム生成の実装を確認した。これは文法から導く全列挙の実装候補であり、既成の定数時間・重複なし全列挙器をそのまま移植できるという意味ではない。

- [ ] **D04. Proper interval / bipartite permutation — BDD による非同型グラフ族の圧縮列挙。**

  変更候補: A03・B01 の別バックエンド。正準な文字列全体を BDD として構築し、必要に応じて列挙・計数・条件指定を行う。各グラフを順に返す現行APIと、グラフ族を圧縮して保持するAPIを分けて設計する。

  根拠: Kawahara–Saitoh–Takeda–Yoshinaka–Yoshioka, *Efficient non-isomorphic graph enumeration algorithms for several intersection graph classes* (TCS 1003, 2024), [出版社](https://www.sciencedirect.com/science/article/am/pii/S0304397524002068)、[2022年プレプリント](https://arxiv.org/abs/2212.07119)。多項式時間での BDD 構築を、全グラフの明示出力が多項式時間で終わるという意味に解釈しない。perfect グラフ全体を生成するアルゴリズムでもない。

## 5. すでに標準方式が専用アルゴリズムになっているもの

次の5クラスは旧フィルタ方式が選択肢に残るが、ラベル付き版の標準方式は専用実装である。同じ置き換えを新規作業として重複登録しない。B03 は interval のラベルなし版に関する別項目。

| クラス | 現行の標準方式 | 確認したソース |
| --- | --- | --- |
| Strongly chordal | `KIYOMI_EDGE_ADDITION` | [strongly_chordal_labeled_enum.h](include/enumerators/strongly_chordal_labeled_enum.h) |
| Proper chordal | `INDIFFERENCE_EDGE_ADDITION` | [proper_chordal_labeled_enum.h](include/enumerators/proper_chordal_labeled_enum.h) |
| Interval | `KIYOMI_KIJIMA_UNO` | [interval_labeled_enum.h](include/enumerators/interval_labeled_enum.h) |
| Split | `KS_PARTITION_CANONICAL` | [split_labeled_enum.h](include/enumerators/split_labeled_enum.h) |
| Weakly chordal | `KIYOMI_EDGE_ADDITION` | [weakly_chordal_labeled_enum.h](include/enumerators/weakly_chordal_labeled_enum.h) |

## 6. 今回、同じ列挙問題への専用置き換えを確認できなかったクラス

以下は「専用アルゴリズムが存在しない」という結論ではない。今回確認した一次資料から、現行の固定 `n` の全グラフ列挙を置き換える実装項目として確定できなかったもの。

| クラス | 保留理由・混同を避ける対象 |
| --- | --- |
| Chordal bipartite | Kurita–Wasa–Arimura–Uno の既存実装は、与えられたグラフ内の**誘導部分グラフ**列挙。固定 `n` の全 chordal bipartite グラフ生成とは異なる。また、DH 論文の `(6,2)`-chordal bipartite は狭い部分クラス。 |
| Circular-arc、proper circular-arc、trapezoid、circle | 認識・表現の生成・ランダム生成と、グラフを重複なく全列挙する問題を区別する必要がある。circle では弦図の非同型列挙だけで交差グラフの非同型性は保証されない。 |
| Comparability、co-comparability | 半順序や推移的向き付けの列挙は同じ比較可能性グラフを重複して生成し得る。co-comparability の補グラフ変換も、元の comparability 専用列挙が確定してから扱う。 |
| Convex bipartite、biconvex bipartite | 行列や頂点順序の生成と、同じグラフの複数表現の処理が必要。現行の全クラスに一致する専用生成法を未確認。 |
| Line graph、apex | 元グラフからの線グラフ化、平面グラフへの apex 追加という構造的な生成案はあるが、今回は重複処理まで備えた専用列挙の一次資料を確認できなかった。 |
| Perfect、AT-free、Meyniel、parity、even-hole-free、odd-hole-free | 部分クラスの生成、性質の認識、クラス内の別の対象の列挙を、グラフクラス全体の生成と混同しない。 |
| Claw-free、diamond-free、bull-free、P5-free、gem-free | 禁止誘導部分グラフを認識して剪定する汎用方式を超える、今回採用できる専用列挙を未確認。 |
| 4-leaf power、5-leaf power | 認識や leaf root の構成と、全非同型グラフの列挙は別問題。A06 の3-leaf power 用操作をそのまま使う根拠はない。 |

Chordal bipartite の区別は、[保存済み論文](references/kurita2019_chordal_bipartite_induced_subgraph_enum.pdf)と [chordal_bipartite_induced_subgraph_enum.h](include/enumerators/chordal_bipartite_induced_subgraph_enum.h)で確認した。完全グラフをホストにした**誘導**部分グラフ列挙では、任意の辺集合を生成できない。

今回確認した古い実装説明・書誌情報・計算量の適用範囲は、[notes/enum_alg.md](notes/enum_alg.md)、README、該当ヘッダの文献コメントにも反映した。実装時は本リストの一次資料とアルゴリズム本文を基準にし、文献の保証を現行実装にそのまま付与しない。

## 完了の判定

各実装項目は、列挙の完全性と重複のなさを説明でき、小さい `n` で旧方式との出力集合の一致を確認してから完了とする。非同型版は正準形集合、ラベル付き版は辺集合で比較する。連結制約、孤立点、空グラフ等の境界は各APIの既存仕様に合わせる。速度比較では候補数・認識器呼び出し数・同型判定数・作業領域と、辺リストを出力する時間を区別する。
