5-leaf power グラフ
========================================

グラフが 5-leaf power であるかどうかを判定する。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_825.html>`_ を参照。

認識
----------

強弦グラフ判定を前段に置き、critical clique の商グラフに対する
3-Steiner root の探索で判定する。この探索は最悪ケースで指数時間となるため
ステップ数の予算下で実行され、予算を使い切った場合は (走り続けたり誤った
NO を黙って返したりする代わりに) ``std::runtime_error`` を送出する。

.. doxygenstruct:: graph_recognition::FiveLeafPowerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_five_leaf_power
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::FiveLeafPowerLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_five_leaf_power_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/five_leaf_power.h"

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
   #include "enumerators/five_leaf_power_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_five_leaf_power_labeled_graphs_reverse_search(4);
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
  *Proceedings of the 33rd Annual ACM-SIAM Symposium on Discrete Algorithms (SODA 2022)*,
  pp. 1384--1410, 2022.
  `DOI:10.1137/1.9781611977073.58 <https://doi.org/10.1137/1.9781611977073.58>`_
