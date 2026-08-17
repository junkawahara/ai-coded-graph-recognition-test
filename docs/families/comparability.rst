比較可能グラフ
==================

グラフが比較可能グラフかどうかを判定する。
辺に推移的な向き付けを与えられるグラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_72.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ComparabilityAlgorithm``
     - 説明
   * - ``TRANSITIVE_ORIENTATION`` **(既定)**
     - permutation.h の内部関数を用いたバックトラッキングによる推移的向き付けソルバ。

.. doxygenenum:: graph_recognition::ComparabilityAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ComparabilityResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_comparability
   :project: graph_recognition


列挙
------------

現在の列挙器はラベル付きの頂点拡張探索を用いる。公開関数名には歴史的経緯から
``reverse_search`` 接尾辞が残っているが、探索木は最大ラベルの頂点を削除する
親写像によって定義される。空グラフから始めて頂点 ``1, 2, ..., n`` を順に追加し、
新しい頂点ごとに、既出の頂点のあらゆる部分集合を近傍として試し、候補を
``check_comparability`` でフィルタリングする。比較可能グラフは遺伝的
(hereditary) なので、ラベルの降順に頂点を削除することで、すべてのラベル付き
比較可能グラフにちょうど一度ずつ到達する。

.. doxygenenum:: graph_recognition::ComparabilityEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ComparabilityEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_comparability_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "comparability.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_comparability(g);

       std::cout << std::boolalpha << result.is_comparability << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "comparability_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_comparability_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
--------------

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_

* M. C. Golumbic. *Algorithmic Graph Theory and Perfect Graphs.*
  Academic Press, 1980; 2nd edition, Annals of Discrete Mathematics 57, Elsevier, 2004.
  `DOI:10.1016/S0167-5060(04)80053-0 <https://doi.org/10.1016/S0167-5060(04)80053-0>`_

* D. Avis, K. Fukuda. "Reverse search for enumeration."
  *Discrete Applied Mathematics*, 65(1--3):21--46, 1996.
  `DOI:10.1016/0166-218X(95)00026-N <https://doi.org/10.1016/0166-218X(95)00026-N>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
