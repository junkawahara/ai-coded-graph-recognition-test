ダイヤモンドフリーグラフ
================================

グラフがダイヤモンドフリー (K_4-e free) かどうかを判定する。
ダイヤモンド (K_4 から辺を 1 本除いたグラフ) と同型な誘導部分グラフを含まない。
同値に、辺を共有する任意の 2 つの三角形は K_4 をなさなければならない。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_441.html>`_ を参照。

認識
----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``DiamondFreeAlgorithm``
     - 説明
   * - ``BRUTE``
     - 4 頂点の部分集合を全列挙してダイヤモンドを探索する。計算量: O(n^4)。
   * - ``EDGE_PAIR`` **(既定)**
     - 各辺について、共通近傍がクリークをなすかどうかを辺数のカウントで確認する。
       クリーク判定は共通近傍の次数和 O(m) を走査するので、計算量: O(m^2)。

.. doxygenenum:: graph_recognition::DiamondFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DiamondFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_diamond_free
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::DiamondFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DiamondFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_diamond_free_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "diamond_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_diamond_free(g);

       std::cout << std::boolalpha << result.is_diamond_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "diamond_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_diamond_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
