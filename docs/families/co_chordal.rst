Co-chordal グラフ
================================

補グラフが弦グラフであるグラフを **co-chordal** グラフという。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_145.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoChordalAlgorithm``
     - 説明
   * - ``COMPLEMENT`` **(既定)**
     - 補グラフを構築し、弦グラフ認識を適用する。

.. doxygenenum:: graph_recognition::CoChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_chordal
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::CoChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_chordal_graphs_reverse_search
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 2, 3, 4, 5, 6`` に対して、列挙されたラベル付き co-chordal グラフの個数が、
弦グラフとの補グラフによる全単射を通じて
`OEIS A058862 <https://oeis.org/A058862>`_ の ``2, 8, 61, 822, 18154`` と
一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "co_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_co_chordal(g);

       std::cout << std::boolalpha << result.is_co_chordal << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "co_chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs."
  *Pacific Journal of Mathematics*, 15(3):835--855, 1965.
  `DOI:10.2140/pjm.1965.15.835 <https://doi.org/10.2140/pjm.1965.15.835>`_

* R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs."
  *SIAM Journal on Computing*, 13(3):566--579, 1984.
  `DOI:10.1137/0213035 <https://doi.org/10.1137/0213035>`_
