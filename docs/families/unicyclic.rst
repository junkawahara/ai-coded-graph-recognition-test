単閉路グラフ
========================

グラフが単閉路グラフであるかどうかを判定する。
単閉路グラフとはちょうど 1 つの閉路を持つ連結グラフであり、ちょうど n 本の辺を持つ
連結グラフと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_1202.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::UnicyclicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::UnicyclicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_unicyclic
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::UnicyclicUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::UnicyclicUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_unicyclic_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 3, 4, 5, 6, 7, 8, 9, 10`` に対して、列挙された非同型な連結単閉路グラフの個数が
`OEIS A001429 <https://oeis.org/A001429>`_ の ``1, 2, 5, 13, 33, 89, 240, 657`` と
一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "unicyclic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_unicyclic(g);

       std::cout << std::boolalpha << result.is_unicyclic << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "unicyclic_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_unicyclic_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
