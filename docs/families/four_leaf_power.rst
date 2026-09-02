4-leaf power グラフ
========================================

グラフが 4-leaf power かどうかを判定する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_650.html>`_ を参照。

認識
----------

.. doxygenstruct:: graph_recognition::FourLeafPowerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_four_leaf_power
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::FourLeafPowerLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_four_leaf_power_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/four_leaf_power.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_four_leaf_power(g);

       std::cout << std::boolalpha << result.is_four_leaf_power << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/four_leaf_power_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_four_leaf_power_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* A. Brandstädt, V. B. Le, R. Sritharan. "Structure and linear-time recognition of 4-leaf powers."
  *ACM Transactions on Algorithms*, 5(1):11:1--11:22, 2008.
  `DOI:10.1145/1435375.1435386 <https://doi.org/10.1145/1435375.1435386>`_
