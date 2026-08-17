5-Leaf Power Graph
========================================

グラフが 5-leaf power であるかどうかを判定する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_825.html>`_ を参照。

認識
----------

.. doxygenstruct:: graph_recognition::FiveLeafPowerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_five_leaf_power
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::FiveLeafPowerEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_five_leaf_power_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "five_leaf_power.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_five_leaf_power(g);

       std::cout << std::boolalpha << result.is_five_leaf_power << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "five_leaf_power_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_five_leaf_power_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* M.-S. Chang, M.-T. Ko. "The 3-Steiner root problem."
  *Proceedings of the 33rd International Workshop on Graph-Theoretic Concepts in Computer Science (WG 2007)*,
  Lecture Notes in Computer Science 4769, pp. 109--120, 2007.
  `DOI:10.1007/978-3-540-74839-7_11 <https://doi.org/10.1007/978-3-540-74839-7_11>`_

* M. Lafond. "Recognizing k-leaf powers in polynomial time, for constant k."
  *Proceedings of the 54th Annual ACM Symposium on Theory of Computing (STOC 2022)*,
  pp. 1349--1361, 2022.
  `DOI:10.1145/3519935.3520031 <https://doi.org/10.1145/3519935.3520031>`_
