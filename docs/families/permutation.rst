順列グラフ
==============

グラフが順列グラフかどうかを判定する。
順列 pi に対し、頂点 i と j が隣接するのは、pi によって両者の順序が反転する場合に限る。
G と complement(G) がともに比較可能グラフであることと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_23.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PermutationAlgorithm``
     - 説明
   * - ``BACKTRACKING``
     - G と complement(G) の両方の推移的向き付けをバックトラッキングで探索する。
   * - ``CLASS_BASED`` **(既定)**
     - Gamma クラス (辺の同値類) ごとに向き付けの整合性を検査する。
       未向き付けの辺を貪欲に選択し、各クラス内で制約を伝播させる。

.. doxygenenum:: graph_recognition::PermutationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PermutationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_permutation
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::PermutationEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PermutationEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_permutation_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "permutation.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_permutation(g);

       std::cout << std::boolalpha << result.is_permutation << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "permutation_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_permutation_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
--------------

* A. Pnueli, A. Lempel, S. Even. "Transitive orientation of graphs and identification of permutation graphs."
  *Canadian Journal of Mathematics*, 23(1):160--175, 1971.
  `DOI:10.4153/CJM-1971-016-5 <https://doi.org/10.4153/CJM-1971-016-5>`_

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_
