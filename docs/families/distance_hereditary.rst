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

.. doxygenenum:: graph_recognition::DistanceHereditaryAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_distance_hereditary
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::DistanceHereditaryEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_distance_hereditary_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "distance_hereditary.h"

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
   #include "distance_hereditary_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_distance_hereditary_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
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
