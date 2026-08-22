パリティグラフ
============================

任意の 2 頂点間の全ての誘導パスの長さの偶奇が一致する (全て偶数長、または
全て奇数長である) グラフを **パリティグラフ** という。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_75.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ParityAlgorithm``
     - 説明
   * - ``DIRECT_CHECK`` **(既定)**
     - 各頂点対の間の全ての誘導パスの偶奇が一致することを直接検証する。

.. doxygenenum:: graph_recognition::ParityAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ParityResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_parity_direct
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_parity
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::ParityEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ParityEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_parity_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "parity.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_parity(g);

       std::cout << std::boolalpha << result.is_parity << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "parity_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_parity_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* M. Burlet, J. P. Uhry. "Parity graphs."
  *Annals of Discrete Mathematics*, 21:253--277, 1984.
  `DOI:10.1016/S0304-0208(08)72939-6 <https://doi.org/10.1016/S0304-0208(08)72939-6>`_

* A. Bouchet. "Reducing prime graphs and recognizing circle graphs."
  *Combinatorica*, 7(3):243--254, 1987.
  `DOI:10.1007/BF02579301 <https://doi.org/10.1007/BF02579301>`_
