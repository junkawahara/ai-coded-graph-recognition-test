3-leaf power
=========================

グラフが 3-leaf power かどうかを判定する。
Brandstadt & Le (2006) により (bull, dart, gem)-free な弦グラフと同値であり、
critical clique graph が森であることを確認することで認識できる。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_651.html>`_ を参照。

認識
----------

.. doxygenstruct:: graph_recognition::ThreeLeafPowerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_three_leaf_power
   :project: graph_recognition


列挙
----------

.. doxygenstruct:: graph_recognition::ThreeLeafPowerEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_three_leaf_power_graphs_reverse_search
   :project: graph_recognition


使用例
----------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "three_leaf_power.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_three_leaf_power(g);

       std::cout << std::boolalpha << result.is_three_leaf_power << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "three_leaf_power_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_three_leaf_power_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* N. Nishimura, P. Ragde, D. M. Thilikos. "On graph powers for leaf-labeled trees."
  *Journal of Algorithms*, 42(1):69--108, 2002.
  `DOI:10.1006/jagm.2001.1195 <https://doi.org/10.1006/jagm.2001.1195>`_

* A. Brandstädt, V. B. Le. "Structure and linear-time recognition of 3-leaf powers."
  *Information Processing Letters*, 98(4):133--138, 2006.
  `DOI:10.1016/j.ipl.2006.01.004 <https://doi.org/10.1016/j.ipl.2006.01.004>`_

* M. Dom, J. Guo, F. Hüffner, R. Niedermeier. "Error compensation in leaf power problems."
  *Algorithmica*, 44(4):363--381, 2006.
  `DOI:10.1007/s00453-005-1180-z <https://doi.org/10.1007/s00453-005-1180-z>`_
