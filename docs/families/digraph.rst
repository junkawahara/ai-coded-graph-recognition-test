有向グラフ
====================

一般の有向グラフに対する認識およびユーティリティ。

認識
----------

.. doxygenenum:: graph_recognition::DigraphAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DigraphResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_digraph
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::DigraphLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DigraphLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_digraphs
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4`` について、列挙されたラベル付き有向グラフの個数が
`OEIS A053763 <https://oeis.org/A053763>`_ (コメントで ``a(n) = 2^(n^2 - n)``
が ``n`` 個のラベル付き頂点上の自己ループを持たない単純有向グラフの個数である
と述べられている) と一致することを検証した: ``1, 4, 64, 4096``。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "digraph.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {2, 1}, {2, 3}};
       auto result = check_digraph(3, arcs);

       std::cout << std::boolalpha << result.is_digraph << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "digraph_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_digraphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
