余比較可能グラフ
========================================

グラフが co-comparability グラフであるかを判定する。
補グラフが comparability グラフであるグラフを指す。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_147.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoComparabilityAlgorithm``
     - 説明
   * - ``COMPLEMENT`` **(既定)**
     - 補グラフを構築し、comparability グラフ認識を適用する。

.. doxygenenum:: graph_recognition::CoComparabilityAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoComparabilityResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_comparability
   :project: graph_recognition


列挙
--------

現在の列挙器はラベル付き頂点拡張探索を用いる。公開関数名には歴史的経緯により
``reverse_search`` 接尾辞が残っているが、探索木は最大ラベルの頂点を削除する
親写像によって定義される。空グラフから始めて頂点 ``1, 2, ..., n`` を順に追加し、
新しい頂点ごとに既存頂点の全部分集合を近傍として試し、候補を
``check_co_comparability`` でフィルタする。co-comparability グラフは遺伝的
(hereditary) なので、各ラベル付き co-comparability グラフにはラベルの降順に
頂点を削除する経路でちょうど一度だけ到達する。

.. doxygenenum:: graph_recognition::CoComparabilityLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoComparabilityLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_comparability_labeled_graphs_reverse_search
   :project: graph_recognition

上の列挙器はラベル付きグラフを出力する。もう 1 つの列挙器は同型類ごとに
代表元を 1 つ出力する。非同型な比較可能グラフの列挙 (McKay の canonical
construction path、``check_comparability`` による枝刈り) を行い、各グラフの
補グラフを出力する。補グラフ操作は頂点の付け替えと可換なので同型類上の
全単射であり、個数は比較可能グラフの個数 (OEIS A123416) と一致する。

.. doxygenstruct:: graph_recognition::CoComparabilityUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CoComparabilityUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_comparability_unlabeled_graphs
   :project: graph_recognition


OEIS カウント検証
--------------------------------

非同型列挙については、``n = 8`` までの個数が
`OEIS A123416 <https://oeis.org/A123416>`_ (比較可能グラフの個数;
``1, 2, 4, 11, 33, 144, 824, 6793``) と一致することを検証し、``n = 6`` までは
ラベル付き列挙の出力を正準形化して得られる同型類の集合と一致することも
検証した。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "co_comparability.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_co_comparability(g);

       std::cout << std::boolalpha << result.is_co_comparability << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "co_comparability_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_comparability_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

非同型列挙の例
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "co_comparability_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_comparability_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 144 = A123416(6)
       return 0;
   }


参考文献
------------

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_

* D. Avis, K. Fukuda. "Reverse search for enumeration."
  *Discrete Applied Mathematics*, 65(1--3):21--46, 1996.
  `DOI:10.1016/0166-218X(95)00026-N <https://doi.org/10.1016/0166-218X(95)00026-N>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* R. H. Möhring. "Almost all comparability graphs are UPO."
  *Discrete Mathematics*, 50:63--70, 1984.
  `DOI:10.1016/0012-365X(84)90035-6 <https://doi.org/10.1016/0012-365X(84)90035-6>`_
