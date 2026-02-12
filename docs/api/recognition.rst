認識アルゴリズム
================

各認識関数は ``enum class`` パラメータによりアルゴリズムを選択できます。
太字のアルゴリズムがデフォルトです。


.. _chordal-family:

弦グラフ系
----------

chordal.h -- 弦グラフ
^^^^^^^^^^^^^^^^^^^^^

グラフが弦グラフ (chordal graph) であるかを判定します。
長さ 4 以上の誘導閉路を持たないグラフが弦グラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalAlgorithm``
     - 説明
   * - ``MCS_PEO``
     - 優先度キュー MCS で PEO 候補を求め検証する。計算量: O(n + m log n)。
   * - ``BUCKET_MCS_PEO`` **(デフォルト)**
     - バケットソート MCS で PEO 候補を求め検証する。双方向リストでバケット管理し O(1) のキー更新を実現。計算量: O(n + m)。

.. doxygenfile:: chordal.h
   :project: graph_recognition


strongly_chordal.h -- 強弦グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが強弦グラフ (strongly chordal graph) であるかを判定します。
弦グラフかつ、すべての偶数長サイクル (長さ 6 以上) が奇弦 (odd chord) を持つことと同値です。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``StronglyChordalAlgorithm``
     - 説明
   * - ``STRONG_ELIMINATION``
     - 弦グラフ性を確認後、全頂点スキャンで simple vertex を除去。計算量: O(n⁴)。
   * - ``PEO_MATRIX`` **(デフォルト)**
     - 弦グラフ性を確認後、隣接行列 + 全スキャンで simple vertex を除去。O(1) 辺判定と次数ベースのソートにより高速化。計算量: O(n² + n·m·Δ)。

.. doxygenfile:: strongly_chordal.h
   :project: graph_recognition


split.h -- スプリットグラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフがスプリットグラフ (split graph) であるかを判定します。
頂点集合をクリークと独立集合に分割できるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SplitAlgorithm``
     - 説明
   * - ``DEGREE_SEQUENCE``
     - G と補グラフの弦性で判定する。計算量: O(n²)。
   * - ``HAMMER_SIMEONE`` **(デフォルト)**
     - Hammer-Simeone 次数列条件で判定。次数降順 d₁≥...≥dₙ で m=max{i: dᵢ≥i−1} とし、Σd[1..m]=m(m−1)+Σd[m+1..n] を検証。計算量: O(n + m)。

.. doxygenfile:: split.h
   :project: graph_recognition


threshold.h -- 閾値グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが閾値グラフ (threshold graph) であるかを判定します。
孤立頂点 (次数 0) または全域頂点 (全頂点に隣接) の除去を繰り返して空にできるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ThresholdAlgorithm``
     - 説明
   * - ``DEGREE_SEQUENCE``
     - 反復除去による判定。計算量: O(n·m)。
   * - ``DEGREE_SEQUENCE_FAST`` **(デフォルト)**
     - 次数列をカウンティングソートで降順に並べ、両端ポインタ + lazy offset でシミュレーション。計算量: O(n + m)。

.. doxygenfile:: threshold.h
   :project: graph_recognition


chordal_bipartite.h -- 弦二部グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが弦二部グラフ (chordal bipartite graph) であるかを判定します。
二部グラフかつ、長さ 6 以上の誘導閉路を持たないグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalBipartiteAlgorithm``
     - 説明
   * - ``CYCLE_CHECK``
     - 二部グラフ性を確認後、長さ 6 以上の誘導偶閉路をブルートフォースで検査する。
   * - ``BISIMPLICIAL``
     - bisimplicial 辺を全探索で検出し 1 本ずつ消去。計算量: O(m·n²)。
   * - ``FAST_BISIMPLICIAL`` **(デフォルト)**
     - bisimplicial 辺を隣接リスト + 隣接行列で検出し消去。計算量: O(m·deg²)。

.. doxygenfile:: chordal_bipartite.h
   :project: graph_recognition


weakly_chordal.h -- 弱弦グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが弱弦グラフ (weakly chordal graph) であるかを判定します。
G と補グラフ complement(G) のいずれにも長さ 5 以上の誘導閉路 (hole) が存在しないグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``WeaklyChordalAlgorithm``
     - 説明
   * - ``CO_CHORDAL_BIPARTITE``
     - 補グラフを明示的に構築し、G と補 G の両方で誘導閉路を検査する。計算量: O(n² + n·m)。
   * - ``COMPLEMENT_BFS`` **(デフォルト)**
     - G の hole は直接検出、anti-hole は隣接行列 + 補グラフ BFS で検出。補グラフの Graph 構築を回避。計算量: O(n·m)。

.. doxygenfile:: weakly_chordal.h
   :project: graph_recognition


.. _interval-family:

インターバルグラフ系
--------------------

interval.h -- インターバルグラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフがインターバルグラフ (interval graph) であるかを判定します。
各頂点を実数直線上の区間に対応させ、区間が重なる場合にのみ辺があるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``IntervalAlgorithm``
     - 説明
   * - ``BACKTRACKING``
     - 弦グラフ性を確認後、クリーク木上でクリークパス (各頂点のクリーク集合が連結部分木をなすパス順序) をバックトラッキングで探索する。
   * - ``AT_FREE`` **(デフォルト)**
     - 弦グラフかつ AT-free (asteroidal triple-free) であることを確認する。Lekkerkerker-Boland の定理に基づく。AT-free 判定は各頂点トリプルについて小惑星三つ組の存在を検査する。

.. doxygenfile:: interval.h
   :project: graph_recognition


proper_interval.h -- 固有インターバルグラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが固有インターバルグラフ (proper interval graph) であるかを判定します。
包含関係のない区間族で表現できるインターバルグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ProperIntervalAlgorithm``
     - 説明
   * - ``PQ_TREE``
     - インターバルグラフかつ claw (K₁,₃) を三重ループで検出。計算量: O(n·Δ³)。
   * - ``FAST_CLAW_CHECK`` **(デフォルト)**
     - インターバルグラフかつ claw を辺計数で検出。N(c) 内の辺数が d(d-1)/2 に満たない場合のみ詳細検索。計算量: O(m·Δ)。

.. doxygenfile:: proper_interval.h
   :project: graph_recognition


unit_interval.h -- 単位インターバルグラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが単位インターバルグラフ (unit interval graph) であるかを判定します。
すべての区間が同じ長さのインターバルグラフです。
Roberts の定理により、固有インターバルグラフと同値です。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``UnitIntervalAlgorithm``
     - 説明
   * - ``PROPER_INTERVAL`` **(デフォルト)**
     - 固有インターバルグラフ認識に帰着する (Roberts の定理)。

.. doxygenfile:: unit_interval.h
   :project: graph_recognition


co_interval.h -- 余インターバルグラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが余インターバルグラフ (co-interval graph) であるかを判定します。
補グラフがインターバルグラフであるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoIntervalAlgorithm``
     - 説明
   * - ``COMPLEMENT`` **(デフォルト)**
     - 補グラフを構築し、インターバルグラフ認識を適用する。

.. doxygenfile:: co_interval.h
   :project: graph_recognition


circular_arc.h -- 円弧グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが円弧グラフ (circular-arc graph) であるかを判定します。
各頂点を円周上の弧に対応させ、弧が重なる場合にのみ辺があるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CircularArcAlgorithm``
     - 説明
   * - ``MCCONNELL`` **(デフォルト)**
     - 円周上の端点順序をバックトラッキングで探索し、各頂点の向き (弧の方向) を 2-SAT で判定する。非隣接頂点の弧が交差しない制約と、隣接頂点の弧が交差する制約を同時に満たす配置を求める。

.. doxygenfile:: circular_arc.h
   :project: graph_recognition


.. _permutation-family:

順列グラフ系
------------

permutation.h -- 順列グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが順列グラフ (permutation graph) であるかを判定します。
順列 π に対し、(i, j) が辺 ⟺ i と j の順序が π で反転しているグラフです。
G と complement(G) の両方が比較可能性グラフであることと同値。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PermutationAlgorithm``
     - 説明
   * - ``BACKTRACKING``
     - G と complement(G) の両方に対してバックトラッキングで推移的向き付けを探索する。各辺に向きを付け、推移律を伝搬して矛盾が生じないかを検査する。
   * - ``CLASS_BASED`` **(デフォルト)**
     - Gamma クラス (辺の同値類) ごとに向き付けの整合性を検査する。連結成分のモジュラー分解を利用して、各クラスが推移的向き付けを許容するかを効率的に判定する。

.. doxygenfile:: permutation.h
   :project: graph_recognition


comparability.h -- 比較可能性グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが比較可能性グラフ (comparability graph) であるかを判定します。
辺に推移的な向き付けを与えられるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ComparabilityAlgorithm``
     - 説明
   * - ``MODULAR_DECOMPOSITION`` **(デフォルト)**
     - Gamma クラスごとの向き付け整合性を検査する。permutation.h の内部関数を利用して推移的向き付けの存在を判定する。

.. doxygenfile:: comparability.h
   :project: graph_recognition


co_comparability.h -- 余比較可能性グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが余比較可能性グラフ (co-comparability graph) であるかを判定します。
補グラフが比較可能性グラフであるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoComparabilityAlgorithm``
     - 説明
   * - ``COMPLEMENT`` **(デフォルト)**
     - 補グラフを構築し、比較可能性グラフ認識を適用する。

.. doxygenfile:: co_comparability.h
   :project: graph_recognition


bipartite_permutation.h -- 二部順列グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが二部順列グラフ (bipartite permutation graph) であるかを判定します。
二部グラフかつ順列グラフであるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartitePermutationAlgorithm``
     - 説明
   * - ``CHAIN_BOTH_SIDES`` **(デフォルト)**
     - 二部グラフ認識と順列グラフ認識の両方を適用する。

.. doxygenfile:: bipartite_permutation.h
   :project: graph_recognition


.. _bipartite-family:

二部グラフ系
------------

bipartite.h -- 二部グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが二部グラフ (bipartite graph) であるかを判定します。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartiteAlgorithm``
     - 説明
   * - ``BFS`` **(デフォルト)**
     - BFS で 2-彩色を試みる。奇数長の閉路が検出されたら非二部グラフ。計算量: O(n + m)。

.. doxygenfile:: bipartite.h
   :project: graph_recognition


chain.h -- チェーングラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフがチェーングラフ (chain graph) であるかを判定します。
二部グラフかつ、各部集合内の頂点の近傍が包含順序で全順序をなすグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChainAlgorithm``
     - 説明
   * - ``NEIGHBORHOOD_INCLUSION``
     - 全ペア近傍包含チェック。計算量: O(n·m)。
   * - ``DEGREE_SORT`` **(デフォルト)**
     - L 側を次数昇順にカウンティングソートし、R 側各頂点の L 側隣接が接尾辞であることを検証。計算量: O(n + m)。

.. doxygenfile:: chain.h
   :project: graph_recognition


cochain.h -- 余チェーングラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが余チェーングラフ (co-chain graph) であるかを判定します。
補グラフがチェーングラフであるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CochainAlgorithm``
     - 説明
   * - ``COMPLEMENT``
     - 補グラフを構築しチェーングラフ認識を適用。計算量: O(n²)。
   * - ``DIRECT`` **(デフォルト)**
     - 補グラフ BFS (linked-list 技法) で co-bipartite 判定後、接尾辞性を検証。計算量: O(n + m)。

.. doxygenfile:: cochain.h
   :project: graph_recognition


.. _other-families:

その他のグラフクラス
--------------------

cograph.h -- コグラフ
^^^^^^^^^^^^^^^^^^^^^

グラフがコグラフ (cograph) であるかを判定します。
P4 (長さ 4 のパス) を誘導部分グラフとして含まないグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CographAlgorithm``
     - 説明
   * - ``COTREE``
     - 連結成分と補連結成分の再帰分解 (余木構築) により判定する。補連結成分探索で unvisited リスト全体をスキャンするため、密グラフで最悪 O(k²) per call。
   * - ``PARTITION_REFINEMENT`` **(デフォルト)**
     - 同じ再帰分解だが、補連結成分探索を高速化。双方向連結リストで remaining 集合を管理し、各 BFS ステップで隣接頂点を一時除去 → 残り全体を一括移動 → 復元する。これにより各ステップのコストが O(deg(v) + |移動数|) に改善され、各再帰レベルでの合計が O(n + m) になる。

.. doxygenfile:: cograph.h
   :project: graph_recognition


block.h -- ブロックグラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフがブロックグラフ (block graph) であるかを判定します。
すべての二重連結成分がクリークであるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BlockAlgorithm``
     - 説明
   * - ``DFS`` **(デフォルト)**
     - DFS で二重連結成分を列挙し、各成分がクリーク (完全グラフ) であるかを検査する。

.. doxygenfile:: block.h
   :project: graph_recognition


distance_hereditary.h -- 距離遺伝グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが距離遺伝グラフ (distance-hereditary graph) であるかを判定します。
任意の連結誘導部分グラフにおいて、2 頂点間の距離が元のグラフと等しいグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``DistanceHereditaryAlgorithm``
     - 説明
   * - ``HASHMAP_TWINS``
     - ペンダント/ツイン頂点を反復除去。文字列シグネチャのハッシュマップで検出。
   * - ``SORTED_TWINS``
     - ペンダント/ツイン頂点を反復除去。ソート済み近傍リストの一致比較で検出。確定的。
   * - ``HASH_TWINS`` **(デフォルト)**
     - XOR ハッシュによるインクリメンタルツイン検出。各頂点にランダム 64-bit weight を割当て、除去時に隣接頂点のハッシュを O(1) で更新。計算量: O(n + m) 期待。

.. doxygenfile:: distance_hereditary.h
   :project: graph_recognition


ptolemaic.h -- プトレマイオスグラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフがプトレマイオスグラフ (Ptolemaic graph) であるかを判定します。
弦グラフかつ距離遺伝グラフであるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PtolemaicAlgorithm``
     - 説明
   * - ``DH_HASHMAP``
     - 弦グラフ性を確認後、ハッシュマップ方式の距離遺伝グラフ判定を適用する。
   * - ``DH_SORTED`` **(デフォルト)**
     - 弦グラフ性を確認後、ソート済み方式の距離遺伝グラフ判定を適用する。

.. doxygenfile:: ptolemaic.h
   :project: graph_recognition


trivially_perfect.h -- 自明完全グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが自明完全グラフ (trivially perfect graph) であるかを判定します。
弦グラフかつコグラフであるグラフです。同値な条件として、
DFS 木の祖先-子孫関係が辺と一致することが知られています。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TriviallyPerfectAlgorithm``
     - 説明
   * - ``DFS`` **(デフォルト)**
     - DFS 木を構築し、すべての辺が祖先-子孫関係に対応するかを検査する。

.. doxygenfile:: trivially_perfect.h
   :project: graph_recognition


quasi_threshold.h -- 準閾値グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが準閾値グラフ (quasi-threshold graph) であるかを判定します。
自明完全グラフの別名であり、同じ判定を行います。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``QuasiThresholdAlgorithm``
     - 説明
   * - ``DFS`` **(デフォルト)**
     - DFS 木を構築し、すべての辺が祖先-子孫関係に対応するかを検査する。

.. doxygenfile:: quasi_threshold.h
   :project: graph_recognition


at_free.h -- AT-free グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが AT-free (asteroidal triple-free) であるかを判定します。
小惑星三つ組 (asteroidal triple) が存在しないグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ATFreeAlgorithm``
     - 説明
   * - ``BRUTE_FORCE`` **(デフォルト)**
     - 全三つ組 (u, v, w) について、各ペアが第三の頂点の近傍を避けたパスで接続可能かを BFS で検査する。

.. doxygenfile:: at_free.h
   :project: graph_recognition


co_chordal.h -- 余弦グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが余弦グラフ (co-chordal graph) であるかを判定します。
補グラフが弦グラフであるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoChordalAlgorithm``
     - 説明
   * - ``COMPLEMENT`` **(デフォルト)**
     - 補グラフを構築し、弦グラフ認識を適用する。

.. doxygenfile:: co_chordal.h
   :project: graph_recognition


.. _planar-family:

平面グラフ系
------------

planar.h -- 平面グラフ
^^^^^^^^^^^^^^^^^^^^^^^

グラフが平面グラフ (planar graph) であるかを判定します。
Kuratowski の定理により、K5 および K3,3 をマイナーとして持たないグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PlanarAlgorithm``
     - 説明
   * - ``MINOR_CHECK`` **(デフォルト)**
     - 辺数上界 (3n - 6) を事前チェック後、K5 マイナーと K3,3 マイナーの存在をバックトラッキングで検査する。

.. doxygenfile:: planar.h
   :project: graph_recognition


outer_planar.h -- 外平面グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが外平面グラフ (outerplanar graph) であるかを判定します。
K4 および K2,3 をマイナーとして持たないグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``OuterPlanarAlgorithm``
     - 説明
   * - ``MINOR_CHECK`` **(デフォルト)**
     - 辺数上界 (2n - 3) を事前チェック後、K4 マイナーと K2,3 マイナーの存在をバックトラッキングで検査する。

.. doxygenfile:: outer_planar.h
   :project: graph_recognition


cactus.h -- カクタスグラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフがカクタスグラフ (cactus graph) であるかを判定します。
各二重連結成分が 1 辺または単純サイクルであるグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CactusAlgorithm``
     - 説明
   * - ``DFS`` **(デフォルト)**
     - DFS で二重連結成分を列挙し、各成分の辺数と頂点数を検査する。成分が 1 辺か、辺数 = 頂点数かつ各頂点の成分内次数が 2 であれば単純サイクルと判定。

.. doxygenfile:: cactus.h
   :project: graph_recognition


series_parallel.h -- 直並列グラフ
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

グラフが直並列グラフ (series-parallel graph) であるかを判定します。
K4 をマイナーとして持たないグラフです。

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SeriesParallelAlgorithm``
     - 説明
   * - ``MINOR_CHECK``
     - 次数 2 以下の頂点を全スキャンで反復除去。計算量: O(n²)。
   * - ``QUEUE_REDUCTION`` **(デフォルト)**
     - 次数 ≤ 2 の頂点をキューで管理し反復除去。2-退化性テスト。計算量: O(n + m)。

.. doxygenfile:: series_parallel.h
   :project: graph_recognition
