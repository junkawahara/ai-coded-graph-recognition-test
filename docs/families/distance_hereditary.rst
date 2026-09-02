距離遺伝グラフ
============================

任意の連結誘導部分グラフにおいて頂点間の距離が元のグラフのまま保たれるとき、
そのグラフを **距離遺伝グラフ** という。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_80.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``DistanceHereditaryAlgorithm``
     - 説明
   * - ``HASHMAP_TWINS``
     - ハッシュマップによる双子頂点検出を用いた、ペンダント頂点 / 双子頂点の
       反復除去。
   * - ``SORTED_TWINS``
     - ソート済み隣接リストの比較による、ペンダント頂点 / 双子頂点の
       反復除去。決定的。
   * - ``HASH_TWINS`` **(既定)**
     - XOR ハッシュによるインクリメンタルな双子検出。各頂点にランダムな
       64 ビットの重みを割り当て、頂点の除去時に近傍のハッシュを O(1) で
       更新する。ハッシュの一致は隣接リストの厳密な比較で検証する。
       除去のたびに候補バケットを再走査するため、密なグラフでは最悪
       O(n\ :sup:`3`) となる。

   * - ``SPLIT_DECOMPOSITION``
     - Cunningham の split decomposition の全バッグが degenerate であることを
       確認する (Bandelt & Mulder 1986;
       :doc:`../api/utilities` の ``split_decomposition.h``)。低速だが、
       twin 消去系の 3 variant と実装を何も共有しないため、真に独立した確認。

.. doxygenenum:: graph_recognition::DistanceHereditaryAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_distance_hereditary
   :project: graph_recognition

刈り取り列の構成
------------------

距離遺伝的グラフは、端点頂点または双子 (twin) の一方を繰り返し削除して
1 頂点にできるグラフちょうどである (Bandelt & Mulder 1986)。
``build_pruning_sequence()`` はその列を構成し、各ステップの根拠となる頂点
(witness) も返す。返す前にグラフに対して再生して検証している。

``check_distance_hereditary()`` は列を構成しないため認識のコストは従来どおり
(witness の記録は高速な認識器が判定に払うコストより高い)。

.. doxygenstruct:: graph_recognition::PruningStep
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PruningSequenceResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::build_pruning_sequence
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::DistanceHereditaryLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_distance_hereditary_labeled_graphs_reverse_search
   :project: graph_recognition

上記の列挙器はラベル付きグラフを出力します。もう一方の列挙器は同型類ごとに
代表元を 1 つだけ出力します。アルゴリズムは Bandelt & Mulder (1986) の
1 頂点拡張による特徴づけ (ペンダント頂点の追加 / true twin の追加 /
false twin の追加) に基づきます。連結な距離遺伝グラフは、1 頂点少ない連結な
距離遺伝グラフにこの 3 種類の拡張のいずれかを施したものちょうどなので、
K1 からレベルごとに生成し、各レベルで正準形の集合により同型なものを除去
します。認識器の呼び出しは不要で、子は 1 グラフあたり 3(k-1) 個
(ラベル付きの逆探索が試す 2\ :sup:`k-1` 通りの近傍と対照的)。
非連結なものはクラスが非交和で閉じていることから、成分サイズの整数分割ごとに
連結代表元の多重集合として合成します。個数は ``connected_only`` 指定時に
OEIS A277862(n) (1, 1, 2, 6, 18, 73, 308, 1484, 7492, ...)、指定しない場合は
その Euler 変換 (1, 2, 4, 11, 31, 114, 454, 2078, 10168, ...) です。

.. doxygenenum:: graph_recognition::DistanceHereditaryUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DistanceHereditaryUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_distance_hereditary_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
------------------------------

非同型列挙については、``connected_only`` 指定時に ``n = 9`` まで
`OEIS A277862 <https://oeis.org/A277862>`_ の
``1, 1, 2, 6, 18, 73, 308, 1484, 7492`` と一致することを検証した。
全体 (非連結を含む) の個数 ``1, 2, 4, 11, 31, 114, 454, 2078, 10168`` は、
``n = 7`` までラベル付き列挙器の出力を正準化した同型類の集合と一致する
ことを確認した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/distance_hereditary.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_distance_hereditary(g);

       std::cout << std::boolalpha << result.is_distance_hereditary << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/distance_hereditary_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_distance_hereditary_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/distance_hereditary_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_distance_hereditary_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 31
       return 0;
   }


参考文献
----------------

* E. Howorka. "A characterization of distance-hereditary graphs."
  *The Quarterly Journal of Mathematics*, 28(4):417--420, 1977.
  `DOI:10.1093/qmath/28.4.417 <https://doi.org/10.1093/qmath/28.4.417>`_

* H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs."
  *Journal of Combinatorial Theory, Series B*, 41(2):182--208, 1986.
  `DOI:10.1016/0095-8956(86)90043-2 <https://doi.org/10.1016/0095-8956(86)90043-2>`_

* P. L. Hammer, F. Maffray. "Completely separable graphs."
  *Discrete Applied Mathematics*, 27(1--2):85--99, 1990.
  `DOI:10.1016/0166-218X(90)90131-U <https://doi.org/10.1016/0166-218X(90)90131-U>`_
