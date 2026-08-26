クローフリーグラフ (爪フリー)
============================================

グラフがクローフリー (K_{1,3}-free) かどうかを判定する。
K_{1,3} (クロー) と同型な誘導部分グラフを含まない。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_62.html>`_ を参照。

認識
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ClawFreeAlgorithm``
     - 説明
   * - ``TRIPLE_LOOP``
     - 各頂点の近傍からサイズ 3 の独立集合を探索する。
       計算量: O(n * Delta^3)。
   * - ``EDGE_COUNT`` **(既定)**
     - 辺数のカウントにより近傍が完全かどうかを判定し、
       完全でない場合のみ詳細な探索を行う。フィルタ自体は O(m * Delta) だが、
       完全でない近傍に対する探索が O(deg^3) かかるため、最悪計算量は
       O(n * Delta^3)。

.. doxygenenum:: graph_recognition::ClawFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ClawFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_claw_free
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::ClawFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ClawFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_claw_free_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "claw_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_claw_free(g);

       std::cout << std::boolalpha << result.is_claw_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "claw_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_claw_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* R. Faudree, E. Flandrin, Z. Ryjáček. "Claw-free graphs --- A survey."
  *Discrete Mathematics*, 164(1--3):87--147, 1997.
  `DOI:10.1016/S0012-365X(96)00045-3 <https://doi.org/10.1016/S0012-365X(96)00045-3>`_
