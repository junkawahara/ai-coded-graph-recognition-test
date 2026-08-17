Chain Graph
========================

グラフが chain グラフであるかを判定する。
各部集合内の頂点の近傍が包含関係により全順序をなす二部グラフを指す。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_442.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChainAlgorithm``
     - 説明
   * - ``NEIGHBORHOOD_INCLUSION``
     - 全ての頂点対について近傍の包含関係を検査する。計算量: O(n * m)。
   * - ``DEGREE_SORT`` **(default)**
     - L 側の頂点を次数でソートし (計数ソート)、R 側の各頂点の L 側近傍が
       接尾辞 (suffix) をなすことを検証する。計算量: O(n + m)。

.. doxygenenum:: graph_recognition::ChainAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChainResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_chain
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::ChainEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChainEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ChainEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chain_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5, 6`` について、列挙された非同型 chain グラフの個数が
`OEIS A005418 <https://oeis.org/A005418>`_ と一致することを検証した
(同エントリのコメントによれば、この数列は Peled & Sun による ``n`` 頂点の
difference グラフ、すなわち二部かつ 2K_2-free なグラフの個数である):
``1, 2, 3, 6, 10, 20``。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "chain.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_chain(g);

       std::cout << std::boolalpha << result.is_chain << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "chain_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chain_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* M. Yannakakis. "The complexity of the partial order dimension problem."
  *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351--358, 1982.
  `DOI:10.1137/0603036 <https://doi.org/10.1137/0603036>`_
