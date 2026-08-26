Halin グラフ
====================

**Halin グラフ** は、次数 2 の頂点を持たない木を平面に埋め込み、
すべての葉をサイクルで結んで得られる平面グラフである。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_198.html>`_ を参照。

認識
----------

.. doxygenenum:: graph_recognition::HalinAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::HalinResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_halin
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::HalinUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::HalinUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_halin_unlabeled_graphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 4, 5, 6, 7, 8, 9, 10`` について、列挙された非同型 Halin グラフの個数が
`OEIS A346779 <https://oeis.org/A346779>`_ の値 ``1, 1, 2, 2, 4, 6, 13`` と
一致することを検証した。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "halin.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_halin(g);

       std::cout << std::boolalpha << result.is_halin << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "halin_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_halin_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* R. Halin. "Studies on minimally n-connected graphs."
  In D. J. A. Welsh (ed.), *Combinatorial Mathematics and its Applications*,
  Academic Press, pp. 129--136, 1971.
