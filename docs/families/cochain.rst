余チェーングラフ
========================================

グラフが co-chain グラフであるかを判定する。
補グラフが chain グラフであるグラフを指す。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CochainAlgorithm``
     - 説明
   * - ``COMPLEMENT``
     - 補グラフを構築し、chain グラフ認識を適用する。計算量: O(n^2)。
   * - ``DIRECT`` **(既定)**
     - 補グラフ BFS (連結リスト技法) により co-bipartite 性を検出し、
       その後に接尾辞性 (suffix property) を検証する。計算量: O(n^2)。

.. doxygenenum:: graph_recognition::CochainAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CochainResult
   :project: graph_recognition
   :members:

``CochainResult`` は 2 つのクリークへの分割 (``color``) と、非近傍が包含で
増加する順序 (``x_ordering`` / ``y_ordering``) を返す。これは補グラフの
chain 構造そのものである。``DIRECT`` は補グラフを構築せずにこれを得る。

.. doxygenfunction:: graph_recognition::check_cochain
   :project: graph_recognition


列挙
--------

.. doxygenstruct:: graph_recognition::CochainUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CochainUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cochain_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5, 6`` について、列挙された非同型 co-chain グラフの個数が、
chain グラフとの補グラフによる全単射を通じて
`OEIS A005418 <https://oeis.org/A005418>`_ と一致することを検証した:
``1, 2, 3, 6, 10, 20``。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cochain.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_cochain(g);

       std::cout << std::boolalpha << result.is_cochain << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cochain_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cochain_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* M. Yannakakis. "The complexity of the partial order dimension problem."
  *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351--358, 1982.
  `DOI:10.1137/0603036 <https://doi.org/10.1137/0603036>`_
