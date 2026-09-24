極大平面グラフ
====================

**極大平面グラフ** (三角形分割) は、どの辺を追加しても平面性が失われる平面グラフである。
極大平面グラフでは、(外面を含む) すべての面が三角形になる。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_981.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::MaximalPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_maximal_planar
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::MaximalPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_planar_labeled_graphs_reverse_search
   :project: graph_recognition

両列挙器の既定は ``PLANTRI`` です。K4 から次数3・4・5の E3/E4/E5
局所拡張を行い、各頂点周りの辺順序で単純三角形分割の埋め込みを保持します。
向き付き辺からの BFS を両方向で行って正準化し、鏡映も同一視します。
正準埋め込み上の逆拡張で親の同型類を選び、兄弟の正準形集合で重複を除きます。
候補ごとの平面性認識と終端での辺数判定は不要です。ラベル付き版では、
各代表を ``n! / |Aut(G)|`` 個の異なるラベル付けに展開します。

従来方式と出力順・代表元は、ラベル付き版の
``MaximalPlanarLabeledEnumAlgorithm::REVERSE_SEARCH`` とラベルなし版の
``MaximalPlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION`` で維持します。
CLI は ``--algo plantri`` （既定）、``--algo reverse_search`` （ラベル付き）、
``--algo canonical_augmentation`` （ラベルなし）で選択できます。
Python の既存ラベルなし API は ``algorithm="plantri"`` または
``algorithm="canonical_augmentation"`` に対応します。

C++・CLI の上限は従来どおりラベル付き64頂点・ラベルなし63頂点で、
``n=0..3`` は各 K_n を1件返します。Python は1〜8頂点です。
非空の出力はすべて連結なので ``connected_only`` はありません。
k 頂点の親の候補数は O(k)、候補あたりの処理時間は O(k²) で、探索経路と
兄弟集合に O(n³) 領域を使います。全出力とラベル展開の訪問済み集合は別途保持します。
外部の plantri プログラムには依存せず、そのインプレース更新・軌道による候補削減は
実装していないため、同等の処理速度や多項式遅延は主張しません。

.. doxygenenum:: graph_recognition::MaximalPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::MaximalPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_planar_unlabeled_graphs
   :project: graph_recognition

カウント検証
--------------------------------

12頂点まで論文の非同型数と一致します（7,595件）。E5 が必要な正二十面体も
含まれます。小規模では両従来方式との出力集合一致を検証します。面の巡回、
局所操作の逆変換、ラベル交換・鏡映に対する正準化もテストしています。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/maximal_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_maximal_planar(g);

       std::cout << std::boolalpha << result.is_maximal_planar << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/maximal_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_planar_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/maximal_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_planar_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 14
       return 0;
   }


参考文献
------------

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
