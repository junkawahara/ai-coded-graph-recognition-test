木
==========

グラフが木であるかどうかを判定する。
木とは閉路を持たない連結グラフであり、ちょうど n-1 本の辺を持つ連結グラフと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_342.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::TreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_tree
   :project: graph_recognition


列挙
----------

.. doxygenenum:: graph_recognition::TreeUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TreeUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TreeUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_tree_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12`` に対して、列挙された非同型な木の個数が
`OEIS A000055 <https://oeis.org/A000055>`_ の ``1, 1, 1, 2, 3, 6, 11, 23, 47, 106, 235,
551`` と一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "tree.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_tree(g);

       std::cout << std::boolalpha << result.is_tree << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "tree_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_tree_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* A. Cayley. "A theorem on trees."
  *Quarterly Journal of Pure and Applied Mathematics*, 23:376--378, 1889.
