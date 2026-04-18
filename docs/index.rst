Graph Recognition Library
=========================

グラフクラスの認識アルゴリズムを提供する C++11 ヘッダオンリーライブラリ。

.. toctree::
   :maxdepth: 2
   :caption: 目次

   usage
   api/index
   python


概要
----

本ライブラリは 75 以上のグラフクラスの認識アルゴリズムを実装しています。
各認識関数は ``enum class`` パラメータによりアルゴリズムを選択でき、
デフォルト値が設定されているため省略も可能です。

.. list-table:: 対応グラフクラス
   :header-rows: 1
   :widths: 25 75

   * - 系列
     - グラフクラス
   * - **弦グラフ系**
     - chordal, strongly chordal, proper chordal, split, threshold, weakly chordal, block, ptolemaic, trivially perfect, quasi-threshold, k-tree
   * - **インターバル / 円弧系**
     - interval, proper interval, unit interval, co-interval, circular-arc, proper circular-arc
   * - **順列 / 比較可能性系**
     - permutation, comparability, co-comparability, bipartite permutation, trapezoid
   * - **二部グラフ系**
     - bipartite, chordal bipartite, chain, co-chain, convex bipartite, biconvex bipartite
   * - **平面グラフ系**
     - planar, outerplanar, cactus, series-parallel, apex, maximal planar, cubic planar, polyhedral, simple quadrangulation, Halin, fullerene
   * - **完全 / 構造系**
     - perfect, cograph, distance-hereditary, AT-free, co-chordal, line graph, circle, Meyniel, parity, even-hole-free, odd-hole-free, cluster, self-complementary
   * - **禁止部分グラフ系**
     - claw-free, diamond-free, triangle-free, bull-free, P5-free, gem-free
   * - **Leaf Power**
     - 3-leaf power, 4-leaf power, 5-leaf power
   * - **木 / 森**
     - tree, forest, caterpillar, unicyclic
   * - **連結性 / 正則性**
     - biconnected, triconnected, Eulerian, k-regular, cubic, strongly regular, snark, Laman
   * - **有向グラフ**
     - tournament, digraph, poset

多くのグラフクラスは **列挙** (指定サイズのラベル付きグラフをすべて生成)
も逆探索によってサポートしています。


特徴
----

* **C++11 互換** -- モダンコンパイラ不要
* **ヘッダオンリー** -- ``#include`` するだけで使用可能
* **1-indexed** -- 頂点番号は 1 から n
* **名前空間** -- ``graph_recognition`` に統一
* **アルゴリズム選択** -- ``enum class`` パラメータで切り替え可能


インデックス
------------

* :ref:`genindex`
