AT-free グラフ
============================

アステロイダルトリプル --- 3 頂点であって、どの 2 頂点も残る 1 頂点の閉近傍を
避けるパスで結べるもの --- を含まないグラフを **AT-free** (asteroidal
triple-free) グラフという。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_61.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ATFreeAlgorithm``
     - 説明
   * - ``BRUTE_FORCE`` **(既定)**
     - 全ての 3 頂点組 (u, v, w) について、各ペアが残る 1 頂点の閉近傍を
       避けるパスで結べるかどうかを BFS で判定する。

.. doxygenenum:: graph_recognition::ATFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ATFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_at_free
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::ATFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ATFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_at_free_labeled_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "at_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_at_free(g);

       std::cout << std::boolalpha << result.is_at_free << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "at_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_at_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_

* D. G. Corneil, S. Olariu, L. Stewart. "Asteroidal triple-free graphs."
  *SIAM Journal on Discrete Mathematics*, 10(3):399--430, 1997.
  `DOI:10.1137/S0895480193250125 <https://doi.org/10.1137/S0895480193250125>`_
