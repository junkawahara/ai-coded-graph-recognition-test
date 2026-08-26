二部グラフ
====================

グラフが二部グラフであるかを判定する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_69.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartiteAlgorithm``
     - 説明
   * - ``BFS`` **(既定)**
     - BFS により 2 彩色を試み、最初の色衝突で失敗を報告する。
       二部グラフなら 2 彩色を、そうでなければ衝突辺を閉じる奇閉路を返す。
       計算量: O(n + m)。

.. doxygenenum:: graph_recognition::BipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_bipartite
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::BipartiteLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_labeled_graphs_reverse_search
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6`` について、列挙されたラベル付き二部グラフの個数が
`OEIS A047864 <https://oeis.org/A047864>`_ と一致することを検証した:
``2, 7, 41, 376, 5177``。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_bipartite(g);

       std::cout << std::boolalpha << result.is_bipartite << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "bipartite_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bipartite_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* D. König. *Theorie der endlichen und unendlichen Graphen.*
  Akademische Verlagsgesellschaft, Leipzig, 1936.
