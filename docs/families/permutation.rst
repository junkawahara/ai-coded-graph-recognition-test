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

順列図の構成
--------------

``build_permutation_realizer()`` は順列図そのものを構成する
(Pnueli, Lempel & Even 1971; Golumbic, *Algorithmic Graph Theory and Perfect
Graphs*, Ch. 7)。G の推移的向き付け F1 と補グラフの向き付け F2 に対し、
F1 ∪ F2 と F1 の逆向き ∪ F2 はいずれも推移的トーナメントになる。各頂点の
順位をその 2 つのトーナメントから読み取ると 2 本の線上の位置が定まる。

順位は比較関数によるソートではなく入次数として求める。n 頂点の推移的
トーナメントの入次数はちょうど 0..n-1 なので入次数そのものが位置であり、
定理の適用が正しいことに依存する比較関数を使わずに済む。

.. doxygenstruct:: graph_recognition::PermutationRealizerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::build_permutation_realizer
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::PermutationLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PermutationLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_permutation_labeled_graphs_reverse_search
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
   #include "permutation_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_permutation_labeled_graphs_reverse_search(4);
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
