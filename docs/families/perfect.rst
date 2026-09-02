パーフェクトグラフ
============================

すべての誘導部分グラフにおいて彩色数がクリーク数に等しいグラフを
**パーフェクトグラフ** という。強パーフェクトグラフ定理
(Chudnovsky-Robertson-Seymour-Thomas 2006) により、これは奇ホール
(長さ 5 以上の誘導奇サイクル) と奇アンチホール (長さ 5 以上の奇ホールの補グラフ)
をいずれも含まないことと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_56.html>`_ を参照。

認識
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - アルゴリズム
     - 説明
   * - **(既定)**
     - 各辺 (u,v) について制限グラフを構築し、まず BFS で二部性を調べて
       奇ホールを持ち得ないケースを除外する。残ったケースでは偶数長の誘導パスを
       バックトラッキング DFS で探索し、見つかれば奇ホールとなる。
       奇アンチホールは、同じ手続きを補グラフ上で実行して検出する。
       DFS は最悪ケースで指数時間であり、またアンチホール側は常に
       Θ(n^2) 本の辺を持つ補グラフを構築するため、実用上の上限は数百頂点程度。

.. doxygenstruct:: graph_recognition::PerfectResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_perfect
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::PerfectLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PerfectLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_perfect_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/perfect.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_perfect(g);

       std::cout << std::boolalpha << result.is_perfect << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/perfect_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_perfect_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* M. Chudnovsky, N. Robertson, P. Seymour, R. Thomas. "The strong perfect graph theorem."
  *Annals of Mathematics*, 164(1):51--229, 2006.
  `DOI:10.4007/annals.2006.164.51 <https://doi.org/10.4007/annals.2006.164.51>`_

* M. Chudnovsky, G. Cornuéjols, X. Liu, P. Seymour, K. Vušković. "Recognizing Berge graphs."
  *Combinatorica*, 25(2):143--186, 2005.
  `DOI:10.1007/s00493-005-0012-8 <https://doi.org/10.1007/s00493-005-0012-8>`_
