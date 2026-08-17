弦二部グラフ
========================

グラフが **弦二部グラフ** (chordal bipartite graph) であるとは、二部グラフで
あり、かつ長さ 6 以上の誘導サイクルを含まないことをいう。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_79.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalBipartiteAlgorithm``
     - 説明
   * - ``CYCLE_CHECK``
     - 誘導偶サイクルの総当たり探索
   * - ``BISIMPLICIAL``
     - Bisimplicial 辺消去 (総当たり)、O(m n^4)
   * - ``FAST_BISIMPLICIAL`` **(既定)**
     - 高速な bisimplicial 辺消去、O(m^2 Delta^2)

.. doxygenenum:: graph_recognition::ChordalBipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_chordal_bipartite
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::ChordalBipartiteEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_bipartite_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "chordal_bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_chordal_bipartite(g);

       std::cout << std::boolalpha << result.is_chordal_bipartite << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "chordal_bipartite_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chordal_bipartite_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* M. C. Golumbic, C. F. Goss. "Perfect elimination and chordal bipartite graphs."
  *Journal of Graph Theory*, 2(2):155--163, 1978.
  `DOI:10.1002/jgt.3190020209 <https://doi.org/10.1002/jgt.3190020209>`_

* A. Lubiw. "Doubly lexical orderings of matrices."
  *SIAM Journal on Computing*, 16(5):854--879, 1987.
  `DOI:10.1137/0216057 <https://doi.org/10.1137/0216057>`_
