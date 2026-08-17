弱弦グラフ
====================

グラフが **弱弦グラフ** (weakly chordal graph) であるとは、そのグラフ自身も
補グラフも長さ 5 以上の誘導サイクルを含まないことをいう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_14.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``WeaklyChordalAlgorithm``
     - 説明
   * - ``CO_CHORDAL_BIPARTITE``
     - Co-chordal-bipartite に基づく認識、O(n^2 + n m)
   * - ``COMPLEMENT_BFS`` **(既定)**
     - 補グラフ BFS による手法、O(n m)

.. doxygenenum:: graph_recognition::WeaklyChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::WeaklyChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_weakly_chordal_co
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_weakly_chordal_complement_bfs
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_weakly_chordal
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::WeaklyChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::WeaklyChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_weakly_chordal_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "weakly_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_weakly_chordal(g);

       std::cout << std::boolalpha << result.is_weakly_chordal << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "weakly_chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_weakly_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* R. B. Hayward. "Weakly triangulated graphs."
  *Journal of Combinatorial Theory, Series B*, 39(3):200--208, 1985.
  `DOI:10.1016/0095-8956(85)90050-4 <https://doi.org/10.1016/0095-8956(85)90050-4>`_

* J. P. Spinrad, R. Sritharan. "Algorithms for weakly triangulated graphs."
  *Discrete Applied Mathematics*, 59(2):181--191, 1995.
  `DOI:10.1016/0166-218X(93)E0161-Q <https://doi.org/10.1016/0166-218X(93)E0161-Q>`_
